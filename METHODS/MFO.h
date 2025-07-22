#ifndef MFO_H
#define MFO_H
# include <OPTIMUS/problem.h>
# include <OPTIMUS/optimizer.h>


class MFO: public Optimizer
{
private:
    double localsearchRate;
    int maxGenerations;
    vector<vector<double>> population;
     vector<double> x_star;
    vector<double> fitness;
    vector<double> lower;
    vector<double> upper;
    int noImprovementGenerations;
    int similaritySize;
    double similarityValue;
    int similarityCount;
    vector<double> bestValues;
      double finalBestValue;
    int N;
    int D;
    int T;
    int I;
    Optimizer *local=NULL;
    QString terminationMethod;
    int generation;
    Data bestX;
    double bestFitness;
    vector<double> best_solution;
    double r ;
    double newFitnessDive;
    double newFitness;
    vector<double>  SS;
    int selectedIdx ;
    vector<double> newX;


public:
    MFO();
    virtual void init();

    double evaluate( vector<double>& solution, double& bestFitness);
    void MFO1(int N, int T, vector<double>& bestValues);
    void CalcFitnessArray();
    virtual void step();
    virtual void   done();
    virtual bool terminated();
    virtual void Selection();
    ~MFO();
};
#endif // MFO_H
