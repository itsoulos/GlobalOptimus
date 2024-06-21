#include "ofalgorithm.h"
# include <METHODS/gradientdescent.h>
# include <METHODS/bfgs.h>


OFAlgorithm::OFAlgorithm() {
    addParam(Parameter("ofa_count","500","Number of chromosomes"));
    addParam(Parameter("ofa_count1","250","Number of chromosomes"));
    addParam(Parameter("ofa_maxiters","200","Maximum number of generations"));
    addParam(Parameter("ofa_lrate","0.05","Localsearch rate"));
    addParam(Parameter("ofa_bfgsiters","3","Iterations for bfgs method"));
    addParam(Parameter("ofa_termination","similarity","Termination method. Avaible values: maxiters,similarity,doublebox"));

}


void OFAlgorithm::init()
{
    M=getParam("ofa_count1").getValue().toInt();
    N=getParam("ofa_count").getValue().toInt();
    maxGenerations=getParam("ofa_maxiters").getValue().toInt();
    localsearchRate=getParam("ofa_lrate").getValue().toDouble();
    terminationMethod=getParam("ofa_termination").getValue();
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
    population = selectOptimalSolutions(population, QOP);

    bestX.resize(D);
}


// Αντικειμενική συνάρτηση
double OFAlgorithm::evaluate( vector<double>& solution, double& bestFitness) {
    double fitness = myProblem->statFunmin(solution);

    // έλεγχος  bestFitness
    if (fitness < bestFitness) {
        bestFitness = fitness;
	bestX = solution;
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
	if(fitness[i]<bestFitness)
	{
		bestFitness = fitness[i];
		bestX = population[i];
	}
    }
}

bool OFAlgorithm::BetterSolution(double fitness_xt, double fitness_xt1, int generation) {
    // Υπολογισμός της τυχαίας τιμής λάμδα
    double l = (double) rand() / RAND_MAX;

    // Υπολογισμός των τιμών των εξισώσεων (2)
    double xt = l * fitness_xt1 / (1 + (generation + 1) * l);
    double xt1 = fitness_xt / (generation);

    // Επιστροφή true αν η λύση xt είναι καλύτερη από την xt1, αλλιώς επιστροφή false
    return xt < xt1;
}


// Υπολογισμός του παράγοντα K(t)
double OFAlgorithm::calculateK(int generation, int  maxGenerations) {
    return cos(M_PI / 2 * generation /  maxGenerations);
}

// Υπολογισμός νέας λύσης xt+1
vector<double> OFAlgorithm::calculateChildren(const vector<double>& xt, const vector<double>& x_best, double K_t, int D) {
    vector<double> xt_1(D); // Νέα λύση xt+1
    double r1 = rand()*1.0 / (double)RAND_MAX; // Τυχαίος αριθμός r1
    double r2 = rand()*1.0 / (double)RAND_MAX; // Τυχαίος αριθμός r2


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

vector<vector<double>>  OFAlgorithm::selectOptimalSolutions(const vector<vector<double>>& P, const vector<vector<double>>& QOP) {
    // Συνδυασμός λύσεων από P και QOP
    vector<vector<double>>  MergePopulation = P;
    MergePopulation.insert( MergePopulation.end(), QOP.begin(), QOP.end());

    // Ταξινόμηση συνδυασμένου πληθυσμού με βάση τις τιμές fitness
    sort( MergePopulation.begin(),  MergePopulation.end(), [](const auto& a, const auto& b) {
        return a.back() < b.back();
    });

    // Επελέξε τις N πιο προσαρμοσμένες λύσεις
    vector<vector<double>> newPopulation;
    for (unsigned long i = 0;  i <  MergePopulation.size(); ++i) {
        newPopulation.push_back( MergePopulation[i]);
    }

    return newPopulation;
}

// Στη συνάρτηση step() εκτελούνται τα βήματα του αλγορίθμου
void OFAlgorithm::step() {
    generation++;
    CalcFitnessArray();

    // Προετοιμασε  τη συγχώνευση του τρέχοντος πληθυσμού με τον  αντίθετο πληθυσμό
    for (int i = 0; i < N; i++) {
        MergeFitness.push_back(fitness[i]);
        QOP.push_back(CalculateQOS(population[i], N));
    }


    vector<vector<double>> MergePopulation = selectOptimalSolutions(population, QOP);

    double K_t = calculateK(generation, maxGenerations);

    // Αρχικοποιησε έναν νέο πληθυσμό για την επόμενη επανάληψη
    vector<vector<double>> newPopulation(M, vector<double>(D));
    vector<double> newFitness(M);

    QString bfgs_iters=getParam("ofa_bfgsiters").getValue();
    for (int j = 0; j < M; ++j) {
        vector<double> newX = MergePopulation[j];
        bool feasible = false;
        while (!feasible && !terminated()) 
	{
            newX = calculateChildren(MergePopulation[j], MergePopulation[0], K_t, D);  // Generate new solution
            feasible = CheckFeasibility(newX);

	    
            if (!feasible) {
                Bfgs* local = new Bfgs();
                local->setProblem(myProblem);
                local->setParam("opt_debug", "no");
                ((Bfgs*)local)->setParam("bfgs_iters", bfgs_iters);
                double y = myProblem->statFunmin(MergePopulation[j]);
                ((Bfgs*)local)->setPoint(MergePopulation[j], y);
                local->solve();
                ((Bfgs*)local)->getPoint(newX, y);
                feasible = CheckFeasibility(newX);
    	delete local;
            }
            /*if (!feasible) {
                GradientDescent* local = new GradientDescent();
                local->setProblem(myProblem);
                local->setParam("opt_debug", "no");
                ((GradientDescent*)local)->setParam("gd_linesearch", "armijo");
                ((GradientDescent*)local)->setParam("gd_maxiters","3");
                double y = myProblem->statFunmin(MergePopulation[j]);
		newX = MergePopulation[j];
                ((GradientDescent*)local)->setPoint(newX, y);
                local->solve();
                ((GradientDescent*)local)->getPoint(newX, y);
                feasible = CheckFeasibility(newX);
    	delete local;
            }*/
	    
	/*if(!feasible)
	{
            	newPopulation[j] = MergePopulation[j];  // κρατα γονιό
		feasible = true;
		continue;
	}*/
        }

        double currentFitness = evaluate(MergePopulation[j], bestFitness);
        newFitness[j] = evaluate(newX, bestFitness);

        //Συγκρινε τη νέα λύση με τον γονέα
        if (BetterSolution(currentFitness, newFitness[j], generation)) {
            newPopulation[j] = newX;  // κρατα νέα λύση
        } else {
            newPopulation[j] = MergePopulation[j];  // κρατα γονιό
        }
    }



    // Ταξινόμηση του νέου πληθυσμού με βάση τo fitness
    vector<pair<double, vector<double>>> sortPopulationByFitness;
    for (int j = 0; j < M; ++j) {
        sortPopulationByFitness.push_back({newFitness[j], newPopulation[j]});
    }

    sort(sortPopulationByFitness.begin(), sortPopulationByFitness.end());

    // Ενημέρωση Population και fitness μετά την ταξινόμηση
    for (int j = 0; j < M; ++j) {
        fitness[j] = sortPopulationByFitness[j].first;
        population[j] = sortPopulationByFitness[j].second;

    }
}


bool OFAlgorithm::terminated() {
    double besty;
    besty = fitness[0];
    for(unsigned long i=0;i<fitness.size();i++)
        if(fitness[i]<besty) besty = fitness[i];
    besty = bestFitness;
    //printf("OFA. iter = %d  besty = %lf maxGenerations = %d \n",generation,besty,maxGenerations);
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
    //besty = localSearch(population[bestindex]);

    if(bestX.size()==0)
    {
	    bestX=population[bestindex];
    }
    besty  = localSearch(bestX);
    if(getParam("opt_debug").getValue()=="yes")
        printf("OFA. terminate: %lf bestFitness: %lf \n",besty,bestFitness);
}
