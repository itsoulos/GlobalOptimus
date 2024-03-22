#include "armadillo1.h"

Armadillo1::Armadillo1()  {
    addParam(Parameter("gao_count","100","Number of chromosomes"));
    addParam(Parameter("gao_maxiters","200","Maximum number of generations"));
    addParam(Parameter("gao_lrate","0.1","Localsearch rate"));
    addParam(Parameter("gao_iters","1","Number of iters"));
    addParam(Parameter("gen_termination","doublebox","Termination method. Avaible values: maxiters,similarity,doublebox"));
}


void Armadillo1::init()
{

    GaoCount=getParam("gao_count").getValue().toInt();
    maxGenerations=getParam("gao_maxiters").getValue().toInt();
    localsearchRate=getParam("gao_lrate").getValue().toDouble();
    terminationMethod=getParam("gen_termination").getValue();
    iters=getParam("gao_iters").getValue().toInt();

    generation=0;
    population.resize(GaoCount);
    fitness.resize(GaoCount);
    M= myProblem->getDimension();
    for(int i=0;i<GaoCount;i++)
        population[i].resize(M);
    sampleFromProblem(GaoCount,population,fitness);
    worstValue = numeric_limits<double>::lowest();
    worstSpeedWorst = numeric_limits<double>::lowest();
    max_speed = numeric_limits<double>::lowest();
    worstSpeed = numeric_limits<double>::max();
    bestValues.resize(Size, numeric_limits<double>::max());
    lower = myProblem->getLeftMargin();
    upper = myProblem->getRightMargin();
	bestValues.resize(iters);
}

void     Armadillo1::Selection()
{
    Data itemp;
    double dtemp;
    itemp.resize(myProblem->getDimension());
    for (unsigned long i = 0; i < population.size(); i++)
    {
        for(unsigned long j=0;j<population.size()-1;j++)
        {
            if (fitness[j + 1] < fitness[j])
            {
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


// Αντικειμενική συνάρτηση
double Armadillo1::evaluate(const vector<double>& solution,double ff, double& worstSpeed)
{
    if (solution.empty()) {
        worstSpeed = numeric_limits<double>::lowest();
        return numeric_limits<double>::max();
    }

    solutionCopy = solution;

    // Υπολογισμός της τιμής της Rastrigin συνάρτησης με τη χρήση της funmin
    fitness1 =ff;//myProblem->statFunmin(solutionCopy);

    for (size_t i = 0; i < solution.size(); ++i) {
        speed = solution[i];
        if (speed > max_speed) {
            max_speed = speed;
        }
        if (speed < worstSpeed) {
            worstSpeed = speed;
        }
    }

    return fitness1;}

// Αρχικοποίηση πληθυσμού
void Armadillo1::initialize(vector<vector<double>>& population, int GaoCount) {
    for (int i = 0; i < GaoCount; ++i) {
        vector<double> individual;
        for (int j = 0; j < M; ++j) {
            // Παραγωγή τυχαίων τιμών στα όρια που παρέχει το myProblem
            double randValue = lower[j] + (upper[j] - lower[j]) * ((double)rand() / RAND_MAX);
            individual.push_back(randValue);
        }
        population.push_back(individual);
    }
}

// Συνρτηση GAO
void Armadillo1::GAO(int GaoCount,  vector<double>& bestValues) {

    // Καλώ την initialize
 //   initialize(population, GaoCount);

    // GAO iterations
    for (int t = 1; t <= iters; ++t) {
        for (int i = 0; i < GaoCount; ++i) {
		fitness[i]=myProblem->statFunmin(population[i]);
            // Φάση 1: Επίθεση σε αναχώματα τερμιτών (φάση εξερεύνησης)
            vector<vector<double>> TM;
            for (int k = 0; k < GaoCount; k++) {
                if (k != i) {
                    fitness_k = fitness[k];//myProblem->statFunmin(population[k]);
		    fitness[k]=fitness_k;
                    if (fitness_k < evaluate(population[i], fitness[i],worstSpeedBest)) {
                        worstSpeedBest = worstSpeed;
                        TM.push_back(population[k]);
                    }
                }
            }

            if (!TM.empty()) {
                // Select random termite mounds
                x = rand() % TM.size();
                vector<double> STM = TM[x];

                // υπολογισμός νέας θέσης
                r = ((double)rand() / RAND_MAX);
                vector<double> newThesi(M);
                for (int d = 0; d < M; ++d) {
                    newThesi[d] = population[i][d] + r * (STM[d] - population[i][d]);
                }

                // Update ith GAO member

		double ff = myProblem->statFunmin(newThesi);
                if (evaluate(newThesi, ff,worstSpeedNew) < 
				evaluate(population[i], fitness[i],worstSpeedBest)) {
                    worstSpeedBest = worstSpeedNew;
                    population[i] = newThesi;
		    fitness[i]=ff;
                }
            }

            // Φάση 2: Σκάψιμο τερμιτών (φάση εκμετάλλευσης)
            vector<vector<double>> candidateMounds;
            for (int k = 0; k < GaoCount; ++k) {
                if (k != i) {

                    fitness_k = evaluate(population[k], fitness[k],worstSpeed);
                    if (fitness_k < evaluate(population[i],fitness[i], worstSpeedWorst)) {
                        worstSpeedWorst = worstSpeed;
                        candidateMounds.push_back(population[k]);
                    }
                }
            }

            if (!candidateMounds.empty()) {
                // Select the best candidate mound
                bestCF = numeric_limits<double>::max();
                vector<double> bestCM;

                for (unsigned long j = 0; j < candidateMounds.size(); j++) {
                    vector<double>& candidate = candidateMounds[j];

		    double ff=myProblem->funmin(candidate);
                    fitness1 = evaluate(candidate, ff,worstSpeed);
                    if (fitness1 < bestCF) {
                        bestCF = fitness1;
                        bestCM = candidate;
                    }
                }

                // Update ith GAO member using Equation
                r = 1.0 - pow(((double)rand() / RAND_MAX), 2.0);
                double t = 0.9999;
                for (int d = 0; d < M; ++d) {

                    population[i][d] = population[i][d] + (i -  2* r) * (upper[d] - lower[d]) / t;
                }
            }

            double Value = evaluate(population[i], fitness[i],worstSpeedWorst);

            // Ενημέρωση για best solution
            if (Value < bestValues[t - 1]) {
                bestValues[t - 1] = Value;
                bestSolution = population[i];
            }

            // Ενημέρωση για worst solution
            if (Value > worstValue) {
                worstValue = Value;
                worstSolution = population[i];
            }
        }
    }


    finalBestValue = *min_element(bestValues.begin(), bestValues.end());
    printf("Final Best Value: %f\n", finalBestValue);

}

void  Armadillo1::CalcFitnessArray()
{
    for(int i=0;i<GaoCount;i++)
    {

      //  fitness[i]=myProblem->statFunmin(population[i]);
        if(localsearchRate>0.0)
        {
            double r = rand()*1.0/RAND_MAX;
            if(r<localsearchRate)
            {
                fitness[i]=localSearch(population[i]);
            }
        }
    }
}

void Armadillo1::step() {
    ++generation;
    CalcFitnessArray();
    Selection();
    GAO(GaoCount, bestValues);


}

bool Armadillo1::terminated() {
        double besty;
        besty = fitness[0];
        //chromosomes.getBestWorstValues(besty,worsty);
        if(generation>=maxGenerations) return true;
        if(terminationMethod=="doublebox")
            return doubleBox.terminate(besty);
        else
            if(terminationMethod=="similarity")
                return similarity.terminate(besty);
        return false;
    }

void    Armadillo1::done()
{
    Data xpoint;
    double ypoint;
    // chromosomes.sort();
    // chromosomes.getPoint(0,xpoint,ypoint);
    // ypoint = localSearch(xpoint);
    fitness[0]=localSearch(population[0]);
    if(getParam("opt_debug").getValue()=="yes")
        printf("GENETIC. terminate: %lf \n",fitness[0]);
}
