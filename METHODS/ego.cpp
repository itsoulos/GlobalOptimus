#include "ego.h"
#include <METHODS/gradientdescent.h>
#include <METHODS/bfgs.h>
double rand_double(double low, double high) {
    return low + static_cast<double>(rand()) / RAND_MAX * (high - low);
}
double Distance(Data &x1,Data &x2)
{
    double sum=0.0;
    for(int i=0;i<x1.size();i++)
        sum+=(x1[i]-x2[i])*(x1[i]-x2[i]);
    return sqrt(sum);
}
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
    addParam(Parameter("mod1", 1, 2, 3, "..."));
    addParam(Parameter("mod2", 1, 2, 3, "..."));
    addParam(Parameter("mod3", 1, 2, 3, "..."));
}

void EGO::init()
{
    t = 0;
    mod1 = getParam("mod1").getValue().toInt();
    mod2 = getParam("mod2").getValue().toInt();
    mod3 = getParam("mod3").getValue().toInt();
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

    upper = myProblem->getRightMargin();
    lower = myProblem->getLeftMargin();
    bestX.resize(D);
    kambili.resize(maxGenerations, numeric_limits<double>::max());
}

double EGO::evaluate(vector<double> &solution, double &grouperBestFitness)
{
    double fitness= myProblem->statFunmin(solution);

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
        if (solution[i] > upper[i] || solution[i] < lower[i])
        {
            return false;
        }
    }
    return true;
}
void EGO::step()
{

    generation++;
    if(generation==1)
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
        if(!Feasibility(Theseis[i]))
        {
            Theseis[i]=X;
            continue;
        }
   newFitness = evaluate(Theseis[i], grouperBestFitness);
   if (newFitness< grouperBestFitness)
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
            if (mod1 == 1)
                p = (double)rand() / RAND_MAX;
            else if (mod1 == 2)
                p =  2.0*rand()*1.0/RAND_MAX-1.0;// (-1.0/2.0) + 2.0 * ((double)rand() / RAND_MAX);  //mod


            distance2grouper = fabs(C2 * grouperBestThesi[j] - Theseis[i][j]);
            distance2eel = fabs(Theseis[i][j] - C2 * eelThesi[j]);

            X1 = C1 * distance2eel * exp(b * r3) * sin(r3 * 2 * M_PI) + eelThesi[j];
            X2 = grouperBestThesi[j] + C1 * distance2grouper;
            double f1, f2;
            if (mod2 == 1){
                f1 = 0.8;
                f2 = 0.2;
            }else if (mod2 == 2)
            {
                f1 = rand_double(0.0, 2.0);  //////mod
                f2 = rand_double(-2.0, 0.0);  //////mod
            }
            //if (X1 < X2)
            if (p < 0.5)
            {
                Theseis[i][j] = (f1 * X1 + f2 * X2) / 2;
            }
            else
            {
                Theseis[i][j] = (f2 * X1 + f1 * X2) / 2;
            }
        }
    }


    for (unsigned long j = 0; j < Theseis.size(); ++j)
    {
        for (unsigned long k = 0; k < lower.size(); ++k)
        {
            if (Theseis[j][k] > upper[k])
            {
                if (mod3==1)
                    Theseis[j][k] = upper[k];
                else if (mod3==2)
                    continue;
            }
            else if (Theseis[j][k] < lower[k])
            {
                if (mod3==1)
                    Theseis[j][k] = lower[k];
                else if (mod3==2)
                    continue;
            }
        }


    }

   ++t;
        kambili[t] = grouperBestFitness;

    for(int i=0;i<Theseis.size();i++)
    {
          if (localsearchRate > 0.0)
            {
                double r = rand() * 1.0 / RAND_MAX;
                if (r < localsearchRate)
                {
                    fitness[i] = localSearch(Theseis[i]);
                }
            }
    }


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
