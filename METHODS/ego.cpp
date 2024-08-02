#include "ego.h"
#include <METHODS/gradientdescent.h>
#include <METHODS/bfgs.h>

EGO::EGO()
{
    addParam(Parameter("ego_count", 50, 10, 10000, "Number of  SearchAgents"));
    addParam(Parameter("ego_maxiters", 200, 10, 10000, "Maximum number of generations"));
    addParam(Parameter("ego_lrate", 0.05, 0.0, 1.0, "Localsearch rate"));
    addParam(Parameter("ego_iters", 30, 0, 100, "Number of iters"));
    addParam(Parameter("ego_localiters", 3, 0, 100, "Number of local search iters"));
    QStringList opt_localmethod;
    opt_localmethod << "gradient" << "bfgs" << "none";
    addParam(Parameter("ego_localmethod", opt_localmethod[0], opt_localmethod, "Local method used here"));
}

void EGO::init()
{
    t = 0;
    SearchAgents = getParam("ego_count").getValue().toInt();
    maxGenerations = getParam("ego_maxiters").getValue().toInt();
    localsearchRate = getParam("ego_lrate").getValue().toDouble();
    terminationMethod = getParam("opt_termination").getValue();
    generation = 0;
    iters = getParam("ego_iters").getValue().toInt();

    Theseis.resize(SearchAgents);
    fitness.resize(SearchAgents);
    D = myProblem->getDimension();

    for (int i = 0; i < SearchAgents; i++)
        Theseis[i].resize(D);

    sampleFromProblem(SearchAgents, Theseis, fitness);

    upper = myProblem->getLeftMargin();
    lower = myProblem->getRightMargin();
    bestX.resize(D);

    fitness_old.resize(SearchAgents, vector<double>(maxGenerations, numeric_limits<double>::max()));
    position_old.resize(SearchAgents, vector<vector<double>>(maxGenerations, vector<double>(D, 0)));

    troxies.resize(SearchAgents, vector<double>(maxGenerations, 0));
    kambili.resize(maxGenerations, numeric_limits<double>::max());
}

double EGO::evaluate(vector<double> &solution, double &grouperBestFitness)
{
    double fitness = myProblem->statFunmin(solution);

    if (fitness < grouperBestFitness)
    {
        grouperBestFitness = fitness;
        bestX = solution;
    }
    return fitness;
}

void EGO::Selection()
{
    Data xtemp;
    double ytemp;
    xtemp.resize(myProblem->getDimension());
    for (int i = 0; i < SearchAgents; i++)
    {
        for (int j = 0; j < SearchAgents - 1; j++)
        {
            if (fitness[j + 1] < fitness[j])
            {
                xtemp = Theseis[j];
                Theseis[j] = Theseis[j + 1];
                Theseis[j + 1] = xtemp;
                ytemp = fitness[j];
                fitness[j] = fitness[j + 1];
                fitness[j + 1] = ytemp;
            }
        }
    }
    grouperBestThesi = Theseis[0];
    grouperBestFitness = fitness[0];
}

void EGO::CalcFitnessArray()
{
    for (int i = 0; i < SearchAgents; i++)
    {

        fitness[i] = myProblem->statFunmin(Theseis[i]);
        if (localsearchRate > 0.0)
        {
            double r = rand() * 1.0 / RAND_MAX;
            if (r < localsearchRate)
            {
                fitness[i] = localSearch(Theseis[i]);
            }
        }
        if (fitness[i] < grouperBestFitness)
        {
            grouperBestFitness = fitness[i];
            bestX = Theseis[i];
        }
    }
}
bool EGO::terminated()
{
    double besty;
    besty = fitness[0];
    for (unsigned long i = 0; i < fitness.size(); i++)
        if (fitness[i] < besty)
            besty = fitness[i];
    besty = grouperBestFitness;
    if (generation >= maxGenerations)
        return true;
    if (terminationMethod == "doublebox")
        return doubleBox.terminate(besty);
    else if (terminationMethod == "similarity")
        return similarity.terminate(besty);
    return false;
}

bool EGO::Feasibility(const vector<double> &solution)
{
    for (int i = 0; i < D; ++i)
    {
        if (solution[i] < upper[i] || solution[i] > lower[i])
        {
            return false;
        }
    }
    return true;
}
void EGO::step()
{
xxx:
    generation++;
    CalcFitnessArray();
    Selection();

    std::vector<double> X(D);
    double CurrentFitness;

    a = 2 - 2 * (t / (double)maxGenerations);
    starvation_rate = 100 * (t / (double)maxGenerations);

    for (unsigned long i = 0; i < Theseis.size(); ++i)
    {
        r1 = (double)rand() / RAND_MAX;
        r2 = (double)rand() / RAND_MAX;
        r3 = (a - 2) * r1 + 2;
        r4 = 100 * r2;
        C1 = (2 * a * r1) - a;
        C2 = 2 * r1;
        b = a * r2;
        X = Theseis[i];
        CurrentFitness = fitness[i];

        for (unsigned long j = 0; j < Theseis[i].size(); ++j)
        {
            double randLeader = rand() % SearchAgents;
            double D_X_rand = fabs(C2 * Theseis[i][j] - Theseis[randLeader][j]);
            Theseis[i][j] = Theseis[randLeader][j] + C1 * D_X_rand;
        }

        newFitness = evaluate(Theseis[i], grouperBestFitness);
        if (newFitness < grouperBestFitness)
        {
            grouperBestFitness = newFitness;
            grouperBestThesi = Theseis[i];
        }

        if (r4 <= starvation_rate)
        {
            eelThesi.resize(D);
            for (int j = 0; j < D; ++j)
            {
                eelThesi[j] = fabs(C2 * grouperBestThesi[j]);
            }
        }
        else
        {
            int randomIndex = rand() % SearchAgents;
            eelThesi.resize(D);
            for (int j = 0; j < D; ++j)
            {
                eelThesi[j] = C2 * Theseis[randomIndex][j];
            }
        }

        for (int j = 0; j < D; ++j)
        {
            p = (double)rand() / RAND_MAX;
            distance2eel = fabs(Theseis[i][j] - C2 * eelThesi[j]);

            X1 = C1 * distance2eel * exp(b * r3) * sin(r3 * 2 * M_PI) + eelThesi[j];
            distance2grouper = fabs(C2 * grouperBestThesi[j] - Theseis[i][j]);
            X2 = grouperBestThesi[j] + C1 * distance2grouper;

            if (p < 0.5)
            {
                Theseis[i][j] = (0.8 * X1 + 0.2 * X2) / 2;
            }
            else
            {
                Theseis[i][j] = (0.2 * X1 + 0.8 * X2) / 2;
            }
        }
        if (!Feasibility(Theseis[i]))
        {
            Theseis[i] = X;
        }
        else
            fitness[i] = myProblem->statFunmin(Theseis[i]);

        if (getParam("ego_localmethod").getValue() == "gradient")
        {
            GradientDescent *local = new GradientDescent();
            local->setProblem(myProblem);
            local->setParam("opt_debug", "no");
            ((GradientDescent *)local)->setParam("gd_linesearch", "armijo");
            ((GradientDescent *)local)->setParam("gd_maxiters", getParam("ego_localiters").getValue());
            CurrentFitness = fitness[i];
            ((GradientDescent *)local)->setPoint(Theseis[i], CurrentFitness);
            local->solve();
            ((GradientDescent *)local)->getPoint(X, CurrentFitness);
            delete local;
        }
        else if (getParam("ego_localmethod").getValue() == "bfgs")
        {
            Bfgs *local = new Bfgs();
            local->setProblem(myProblem);
            local->setParam("opt_debug", "no");
            ((Bfgs *)local)->setParam("bfgs_iters", getParam("ego_localiters").getValue());
            X = Theseis[i];
            CurrentFitness = fitness[i];
            ((Bfgs *)local)->setPoint(Theseis[i], CurrentFitness);
            local->solve();
            ((Bfgs *)local)->getPoint(X, CurrentFitness);
            delete local;
        }
        else // none case
        {
            // X = Theseis[i];
            // CurrentFitness=fitness[i];
        }

        bool feasible = Feasibility(X);
        if (feasible && CurrentFitness < fitness[i])
        {
            Theseis[i] = X;
            fitness[i] = CurrentFitness;
        }
    }

    for (unsigned long j = 0; j < Theseis.size(); ++j)
    {
        for (unsigned long k = 0; k < lower.size(); ++k)
        {
            if (Theseis[j][k] > upper[k])
            {
                //Theseis[j][k] = upper[k];

                continue;
            }
            else if (Theseis[j][k] < lower[k])
            {
                 //Theseis[j][k] = lower[k];
                continue;
            }
        }

        fitness[j] = evaluate(Theseis[j], grouperBestFitness);

        if (j < fitness_old.size() && t < fitness_old[j].size())
        {
            fitness_old[j][t] = fitness[j];
            position_old[j][t] = Theseis[j];
        }

        troxies[j][t] = Theseis[j][0];

        if (fitness[j] < grouperBestFitness)
        {
            grouperBestThesi = Theseis[j];
            grouperBestFitness = fitness[j];
        }
    }

    if (t < kambili.size())
    {
        kambili[t] = grouperBestFitness;
    }

    ++t;
}

EGO::~EGO()
{
}

void EGO::done()
{
    int bestindex = 0;
    double besty;
    besty = fitness[0];
    for (unsigned long i = 0; i < fitness.size(); i++)
    {
        if (fitness[i] < besty)
        {
            besty = fitness[i];
            bestindex = i;
        }
    }

    bestX = grouperBestThesi;
    besty = localSearch(bestX);
    if (getParam("opt_debug").getValue() == "yes")
        printf("EGO=>. terminate: %lf grouperBestFitness: %lf \n", besty, grouperBestFitness);
}
