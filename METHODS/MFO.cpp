#include "MFO.h"
#include <METHODS/gradientdescent.h>
#include "bfgs.h"

MFO::MFO()  {
    addParam(Parameter("mfo_count", 200,10,10000, "Number of chromosomes"));
    addParam(Parameter("mfo_maxiters", 200,10,10000, "Maximum number of generations"));
    addParam(Parameter("mfo_lrate", 0.05,0.0,1.0, "Local search rate"));
    addParam(Parameter("mfo_iters", 1,2,3, "Number of iterations"));
    QStringList list;
    list<<"aggressive"<<"conservative"<<"mixed";
    addParam(Parameter("mfo_strategy_mode", "mixed",list, "Movement strategy mode. Available: aggressive, conservative, mixed"));
}

void MFO::init() {
    maxGenerations = getParam("mfo_maxiters").getValue().toInt();
    localsearchRate = getParam("mfo_lrate").getValue().toDouble();
    terminationMethod = getParam("opt_termination").getValue();
    N = getParam("mfo_count").getValue().toInt();
    T = getParam("mfo_iters").getValue().toInt();
    strategyMode = getParam("mfo_strategy_mode").getValue().toStdString();
    bestFitness = std::numeric_limits<double>::max();
    generation = 0;
    D = myProblem->getDimension();

  /*  for (int i = 1; i < N; i++) {
       population[i].resize(D);
    }*/
    sampleFromProblem(N, population, fitness);
    lower = myProblem->getLeftMargin();
    upper = myProblem->getRightMargin();
    bestValues.resize(T);

}

void MFO::Selection() {
    Data itemp;
    double dtemp;
    itemp.resize(myProblem->getDimension());

    for (size_t i = 1; i < population.size(); i++) {
        for (size_t j = 0; j < population.size() - 1; j++) {
            if (fitness[j + 1] < fitness[j]) {
                itemp = population[j];
                population[j] = population[j + 1];
                population[j + 1] = itemp;
                dtemp = fitness[j];
                fitness[j] = fitness[j + 1];
                fitness[j + 1] = dtemp;
            }
        }
    }
}


double MFO::evaluate( vector<double>& solution, double& bestFitness) {
    double fitness= myProblem->statFunmin(solution);

    if (fitness < bestFitness) {
        bestFitness = fitness;
        bestX = solution;
    }
    return fitness;
}



void MFO::CalcFitnessArray() {

    for (int i = 0; i < N; i++) {
       // fitness[i] = myProblem->statFunmin(population[i]);

        if (localsearchRate > 0.0) {
            double r = rand() * 1.0 / RAND_MAX;
            if (r < localsearchRate) {
                fitness[i] = localSearch(population[i]);
            }
        }

        if (fitness[i] < bestFitness) {
            bestFitness = fitness[i];
            bestX = population[i];
        }
    }


 }


void MFO::MFO1(int N, int T, vector<double>& bestValues) {

    for (int t = 1; t <= T; ++t) {
        for (int i = 0; i < N; ++i) {

            vector<vector<double>> candidateSeabirds;
            for (int k = 0; k < N; ++k) {
                if (k != i && fitness[k] < fitness[i]) {
                    candidateSeabirds.push_back(population[k]);
                }
            }

            // === Δημιουργία νέας θέσης newThesi ===
            vector<double> newThesi(D);
            if (!candidateSeabirds.empty()) {
                selectedIdx = rand() % candidateSeabirds.size();
                SS = candidateSeabirds[selectedIdx];

                if (strategyMode == "aggressive") {
                    for (int j = 0; j < D; ++j) {
                        r = (double)rand() / RAND_MAX;
                        I = (rand() % 2) + 1;
                        newThesi[j] = population[i][j] + 2.0 * (r - 0.5) * (SS[j] - I * population[i][j]);
                    }
                } else if (strategyMode == "conservative") {
                    for (int j = 0; j < D; ++j) {
                        r = (double)rand() / RAND_MAX;
                        newThesi[j] = population[i][j] + 0.3 * (r - 0.5) * (bestX[j] - population[i][j]);
                    }
                } else { // mixed
                    double opChoice = (double)rand() / RAND_MAX;
                    if (opChoice < 0.5) {
                        for (int j = 0; j < D; ++j) {
                            r = (double)rand() / RAND_MAX;
                            I = (rand() % 2) + 1;
                            newThesi[j] = population[i][j] + 2.0  *(r - 0.5) * (SS[j] - I * population[i][j]);
                        }
                    } else {
                        for (int j = 0; j < D; ++j) {
                            r = (double)rand() / RAND_MAX;
                            newThesi[j] = population[i][j] + 0.3  *(r - 0.5) * (bestX[j] - population[i][j]);
                        }
                    }
                }

        /*
                // Έλεγχος ορίων
                for (int j = 0; j < D; ++j) {
                    if (newThesi[j] < lower[j]) newThesi[j] = population[i][j];// lower[j];
                   if (newThesi[j] > upper[j]) newThesi[j] =  population[i][j];//upper[j];
              }*/

                // Αξιολόγηση
                if (myProblem->isPointIn(newThesi)) {
                    //newFitness = myProblem->statFunmin(newThesi);
                    if (newFitness < fitness[i]) {
                        population[i] = newThesi;
                        fitness[i] = newFitness;
                    }
                }
            }

            // === Δεύτερη φάση (dive) προς το καλύτερο ===
            vector<double> bestSolution = population[0];
            for (int k = 1; k < N; ++k) {
                if (fitness[k] < fitness[0]) {
                    bestSolution = population[k];
                }
            }

            vector<double> newThesiDive(D);
            for (int j = 0; j < D; ++j) {
                r = (double)rand() / RAND_MAX;
                newThesiDive[j] = population[i][j] + (1 - 2 * r)  * (bestSolution[j] - population[i][j]) / t;
            }

            // Έλεγχος ορίων
           for (int j = 0; j < D; ++j) {
               if (newThesiDive[j] < lower[j]) newThesiDive[j] = lower[j];
               if (newThesiDive[j] > upper[j]) newThesiDive[j] = upper[j];
            }

            if (myProblem->isPointIn(newThesiDive)) {
               // newFitnessDive = myProblem->statFunmin(newThesiDive);
                if (newFitnessDive < fitness[i]) {
                    population[i] = newThesiDive;
                    fitness[i] = newFitnessDive;
                }
            }

            // Ενημέρωση bestValues[t - 1]
            if (fitness[i] < bestValues[t - 1]) {
                bestValues[t - 1] = fitness[i];
                bestSolution = population[i];
            }
        }
    }

    finalBestValue = *min_element(bestValues.begin(), bestValues.end());
}

void MFO::step() {
    ++generation;
    CalcFitnessArray();
    Selection();
    MFO1(N, T, bestValues);

    if (population.size() > 0 && fitness.size() > 0 && myProblem) {
        vector<double> x0 = population[0];
        double y0 = fitness[0];

        vector<double> x_min = x0;
        double y_min = y0;

        Bfgs* local = new Bfgs();
        local->setProblem(myProblem);
        local->setParam("opt_debug", "no");
        local->setParam("bfgs_maxiters", "50");

        local->setPoint(x_min, y_min);
        local->solve();

        vector<double> x_star(D, 0.0);
        double y_star = 9999;
        local->getPoint(x_star, y_star);
        delete local;
        if (y_star < bestFitness && myProblem->isPointIn(x_star)) {
            bestFitness = y_star;
            bestX = x_star;

            if (getParam("opt_debug").getValue() == "yes") {
                printf("[BFGS] Improved best from %.6f to %.6f\n", y_min, y_star);
            }
        }
    }
    if (terminated()) {
        done();
    }
}

bool MFO::terminated()
{
    double besty;
    besty = fitness[0];

    if(generation>=maxGenerations) return true;

    if(terminationMethod=="doublebox")
        return doubleBox.terminate(besty);
    else
        if(terminationMethod=="similarity")
            return similarity.terminate(besty);
    return false;
}






void MFO:: done()
{
    int bestindex=0;
    double besty;
    besty = fitness[0];
    for(unsigned long i=0;i<fitness.size();i++)
    {
        if(fitness[i]<besty)
        {
            besty = fitness[i];
            bestindex = i;
        }
    }

    if(bestX.size()==0)
    {
        bestX=population[bestindex];
    }
    besty  = localSearch(bestX);
    if(getParam("opt_debug").getValue()=="yes")
        printf("MFO. terminate: %lf bestFitness: %lf \n",besty,bestFitness);
}

MFO::~ MFO()
{

}
