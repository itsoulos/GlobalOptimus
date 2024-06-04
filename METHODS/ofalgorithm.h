#ifndef OFALGORITHM_H
#define OFALGORITHM_H
# include <OPTIMUS/problem.h>
# include <OPTIMUS/optimizer.h>
class OFAlgorithm:  public Optimizer
{

private:
    double localsearchRate;
    int maxGenerations;
    vector<vector<double>> population;
    vector<double> fitness;
    long D;
    double besty;
    QString terminationMethod;
    vector<double> bestValues;
    int N;
    vector<double> a;
    vector<double> b;
    int similaritySize;
    double similarityValue;
    int similarityCount;
    double bestFitness;
    vector<vector<double>> QOP;
    int iters;
    int maxiters;
    int generation;
    vector<double> CalculateQOS(const vector<double>& xo, double c, int N);
    vector<double> calculateChildren(const vector<double>& xt, const vector<double>& x_best, double K_t, int D);
    vector<vector<double>> selectOptimalSolutions(const vector<vector<double>>& P, const vector<vector<double>>& QOP);
    vector<vector<double>> newPopulation;
public:
    OFAlgorithm();
    double evaluate( vector<double>& solution, double& bestFitness);
    double calculateK(int iters, int maxiters) ;
    virtual void init();
    void quicksort(vector<vector<double>>& population, vector<double>& fitness, int left, int right);
    void updatePopulationByComparison(vector<vector<double>>& population, vector<double>& fitness,  vector<vector<double>>& children);
    int PartitionPopulation(vector<vector<double>>& population, vector<double>& fitness, int left, int right);
    bool BetterSolution(double fitness_xt, double fitness_xt1, int iters);
    bool CheckFeasibility(const vector<double>& solution);
    void CalcFitnessArray();
    virtual void step();
    virtual void   done();
    virtual bool terminated();
    void Selection();
    void ChildrenArray();
    void UpdatePopulation();
    void CalculateFitness();
    ~OFAlgorithm();
};

#endif // OFALGORITHM_H



