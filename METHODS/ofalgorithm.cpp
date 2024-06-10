#include "ofalgorithm.h"
# include <METHODS/gradientdescent.h>


OFAlgorithm::OFAlgorithm() {
    addParam(Parameter("ofa_count","200","Number of chromosomes"));
    addParam(Parameter("ofa_maxiters","200","Maximum number of generations"));
    addParam(Parameter("ofa_lrate","0.0005","Localsearch rate"));
    addParam(Parameter("gen_termination","similarity","Termination method. Avaible values: maxiters,similarity,doublebox"));

}


void OFAlgorithm::init()
{
    M=getParam("ofa_count").getValue().toInt();
    N=getParam("ofa_count").getValue().toInt();
    maxGenerations=getParam("ofa_maxiters").getValue().toInt();
    localsearchRate=getParam("ofa_lrate").getValue().toDouble();
    terminationMethod=getParam("gen_termination").getValue();
    generation=0;
     M= N/2;
    population.resize(N);
    MergePopulation.resize(M);
    fitness.resize(N);
    D= myProblem->getDimension();
    for(int i=0;i<N;i++)
        population[i].resize(D);
    sampleFromProblem(N,population,fitness);
    a = myProblem->getLeftMargin();
    b = myProblem->getRightMargin();
   // population = selectOptimalSolutions(population, QOP);
    CalcFitnessArray();
}



// Αντικειμενική συνάρτηση
double OFAlgorithm::evaluate( vector<double>& solution, double& bestFitness) {
    double fitness = myProblem->statFunmin(solution);

    // έλεγχος  bestFitness
    if (fitness < bestFitness) {
        bestFitness = fitness;
    }
    return fitness;
}
void OFAlgorithm:: quicksort(vector<vector<double>>& population, vector<double>& fitness, int left, int right) {
    if (left < right) {
        int pivotIndex = PartitionPopulation(population, fitness, left, right);
        quicksort(population, fitness, left, pivotIndex - 1);
        quicksort(population, fitness, pivotIndex + 1, right);
    }
}
int OFAlgorithm::PartitionPopulation(vector<vector<double>>& population, vector<double>& fitness, int left, int right) {
    double pivot = fitness[right];
    int i = left - 1;
    for (int j = left; j < right; ++j) {
        if (fitness[j] < pivot) {
            ++i;
            swap(population[i], population[j]);
            swap(fitness[i], fitness[j]);
        }
    }
    return i + 1;
}

void OFAlgorithm::Selection() {
    // Συγχώνευση των πληθυσμών P και QOP
    vector<vector<double>> MergePopulation = population;
    MergePopulation.insert(MergePopulation.end(), QOP.begin(), QOP.end());

    // Υπολογισμός της ποιότητας των λύσεων
    vector<double> MergeFitness;
    for (Data & solution : MergePopulation) {
        MergeFitness.push_back(evaluate(solution,bestFitness));
    }

    // Επιλογή των N λύσεων με την καλύτερη ποιότητα
    quicksort(MergePopulation, MergeFitness, 0, MergePopulation.size() - 1);

    // Επιλογή των πρώτων N λύσεων για τον πληθυσμό
    population.clear();
        for(int j=0;j<M;j++) {
        population.push_back(MergePopulation[j]);
    }
}




void OFAlgorithm::CalcFitnessArray()
{
    for(int i=0;i<N;i++)
    {

        fitness[i]=myProblem->statFunmin(population[i]);
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

bool OFAlgorithm::BetterSolution(double fitness_xt, double fitness_xt1, int generation) {
    // Υπολογισμός της τυχαίας τιμής λάμδα
    double lambda = (double) rand() / RAND_MAX;

    // Υπολογισμός των τιμών των εξισώσεων (2)
    double eq1 = lambda * fitness_xt1 / (1 + (generation + 1) * lambda);
    double eq2 = fitness_xt / (generation);

    // Επιστροφή true αν η λύση x1 είναι καλύτερη από την x2, αλλιώς επιστροφή false
    return eq1 < eq2;
}


// Υπολογισμός του παράγοντα K(t)
double OFAlgorithm::calculateK(int generation, int  maxGenerations) {
    return cos(M_PI / 2 * generation /  maxGenerations);
}

// Υπολογισμός νέας λύσης xt+1
vector<double> OFAlgorithm::calculateChildren(const vector<double>& xt, const vector<double>& x_best, double K_t, int D) {
    vector<double> xt_1(D); // Νέα λύση xt+1
    double r1 = rand() / (double)RAND_MAX; // Τυχαίος αριθμός r1
    double r2 = rand() / (double)RAND_MAX; // Τυχαίος αριθμός r2


    // Υπολογισμός της νέας λύσης xt+1 με βάση το Eq. (6)
    for (int i = 0; i < D; ++i) {
        xt_1[i] = xt[i] + K_t * (r1 - r2) * (xt[i] - x_best[i]);
    }

    return xt_1;
}

// Ενημέρωση πληθυσμού με βάση τη σύγκριση των παιδιών με τους γονείς
void OFAlgorithm::updatePopulationByComparison(vector<vector<double>>& population, vector<double>& fitness,  vector<vector<double>>& children) {
    for (size_t i = 0; i < population.size(); ++i) {
        // Συγκριση παιδί με γονέα
        if (children[i] < population[i]) {
            //Εάν το παιδί είναι καλύτερο, αντικαταστήστε τον γονέα με το παιδί
            population[i] = children[i];
            fitness[i] = myProblem->statFunmin(children[i]);
        }

    }
}


// απόγονοι
void OFAlgorithm::ChildrenArray() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < D; ++j) {
            if (population[i][j] < a[j] || population[i][j] > b[j]) {
                population[i][j] = a[j] + (b[j] - a[j]) * (rand() / (double)RAND_MAX);
            }
        }
    }
}

// ενημέρωση πληθυσμού
void OFAlgorithm::UpdatePopulation() {
    double bestFitness = fitness[0];
    for (int i = 0; i < N; ++i) {
        double fitness_children = evaluate(population[i], bestFitness);
        if (fitness_children < fitness[i]) {
            fitness[i] = fitness_children;
        }
    }

    // Εκτύπωση της bestSolution
    printf("Best Solution: ");
    for (int i = 0; i < D; ++i) {
        printf("%lf ", population[0][i]);
    }

}

// Έλεγχος της εφικτότητας των νέων λύσεων και ανανέωση του πληθυσμού
bool OFAlgorithm::CheckFeasibility(const vector<double>& solution) {
    for (int i = 0; i < D; ++i) {
        if (solution[i] < a[i] || solution[i] > b[i]) {
            return false;
        }
    }
    return true;
}

// Επιλέγει έναν τυχαίο αριθμό στο διάστημα [a, b]
//double OFAlgorithm::randomInRange() {
// Ρύθμιση του seed της rand() με τη χρήση του τρέχοντος χρόνου
// srand(time(0));
// for (int i = 0; i < N; ++i) {
// for (int j = 0; j < D; ++j) {
//       if (population[i][j] < a[j] || population[i][j] > b[j]) {
//return population[i][j] = a[j] + (b[j] - a[j]) * (rand() / (double)RAND_MAX);
//
//}
// }
// }
//}



// Στη συνάρτηση Step() εκτελούνται τα βήματα του αλγορίθμου
void OFAlgorithm::step() {
    generation++;
  //  CalcFitnessArray();
    vector<double> MergeFitness;
        for(int j=0;j<M;j++) {
        MergeFitness.push_back(fitness[j]);
        QOP.push_back(CalculateQOS(population[j], M));
    }

    vector<vector<double>> MergePopulation = selectOptimalSolutions(
                population,QOP);
    double K_t = calculateK(generation, maxGenerations);
    for (int j = 0; j < M; ++j) {
        vector<double> newX;
        bool feasible = false;


        //  δημιουργούμε νέες λύσεις μέχρι να βρούμε μια εφικτή
        while (!feasible) {
            newX = calculateChildren(MergePopulation[j], MergePopulation[0], K_t, D);
            feasible = CheckFeasibility(newX);
            if (!feasible) {
                // Αν η λύση δεν είναι εφικτή, χρησιμοποιούμε το GradientDescent
                GradientDescent* local = new GradientDescent();
                local->setProblem(myProblem);
                local->setParam("opt_debug", "no");
                ((GradientDescent*)local)->setParam("gd_linesearch", "armijo");
                ((GradientDescent*)local)->setParam("gd_maxiters","3");
                double y = myProblem->statFunmin(MergePopulation[j]);
                ((GradientDescent*)local)->setPoint(MergePopulation[j], y);
                local->solve();
                ((GradientDescent*)local)->getPoint(newX, y);

                delete local;

            }
        }

        double newFitness = evaluate(newX, bestFitness);

        if (BetterSolution(fitness[j], newFitness, generation)) {
            population[j] = newX;
            fitness[j] = newFitness;
            if(localsearchRate>0.0)
            {
                double r = rand()*1.0/RAND_MAX;
                if(r<localsearchRate)
                {
                    fitness[j]=localSearch(population[j]);
                }
            }
        }
    }
}


// Υπολογισμός των τιμών καταλληλότητας για τον αρχικό πληθυσμό
void OFAlgorithm::CalculateFitness() {
    double bestFitness = fitness[0];
    for (int i = 0; i < N; ++i) {
        fitness[i] = evaluate(population[i], bestFitness);
    }
}


//Υπολογισμός quasi-opposite ( οιονεί αντίθετη λύση) με βάση της εξίσωση Eq. (3)
vector<double> OFAlgorithm::CalculateQOS(vector<double>& xi, int N) {
    vector<double> xq(N);
    vector<double> c(N);
    vector<double> xo(N);



    for (int i = 0; i < N; ++i) {
        c[i] = (a[i] + b[i]) / 2.0;
    }

    for (int i = 0; i < N; ++i) {
        xo[i] = a[i] + b[i] - xi[i];
        xq[i] = c[i] * rand() / (double)RAND_MAX + (1 - c[i]) * xo[i];
    }

    return xq;
}

vector<vector<double>>  OFAlgorithm::selectOptimalSolutions(
        const vector<vector<double>>& P,

        const vector<vector<double>>& QOP) {
    // Συνδυασμός λύσεων από P και QOP
    vector<vector<double>>  MergePopulation = P;
    MergePopulation.insert( MergePopulation.end(), QOP.begin(), QOP.end());

    // Ταξινόμηση συνδυασμένου πληθυσμού με βάση τις τιμές φυσικής κατάστασης
    sort( MergePopulation.begin(),  MergePopulation.end(), [](const auto& a, const auto& b) {
        return a.back() < b.back();
    });

    // Επιλέξτε τις N πιο προσαρμοσμένες λύσεις(N fittest solutions)
    vector<vector<double>> newPopulation;
    for (unsigned long i = 0;  i <  MergePopulation.size(); ++i) {
        newPopulation.push_back( MergePopulation[i]);
    }

    return newPopulation;
}

bool OFAlgorithm::terminated() {
    double besty;
    besty = fitness[0];
    for(unsigned long i=0;i<fitness.size();i++)
        if(fitness[i]<besty) besty = fitness[i];
    if(getParam("opt_debug").getValue()=="yes")
        printf("OFA. iter = %d  besty = %lf maxGenerations = %d \n",generation,besty,maxGenerations);
    if(generation>=maxGenerations) return true;
    if(terminationMethod=="doublebox")
        return doubleBox.terminate(besty);
    else
        if(terminationMethod=="similarity")
            return similarity.terminate(besty);
    return false;
}

void OFAlgorithm:: done()
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
    besty = localSearch(population[bestindex]);
    if(getParam("opt_debug").getValue()=="yes")
        printf("OFA. terminate: %lf \n",besty);
}
