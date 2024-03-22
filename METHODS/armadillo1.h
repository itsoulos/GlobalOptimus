#ifndef ARMADILLO1_H
#define ARMADILLO1_H
# include <OPTIMUS/problem.h>
# include <OPTIMUS/optimizer.h>


class Armadillo1: public Optimizer
{
private:
    double localsearchRate;
    int maxGenerations;
    vector<vector<double>> population;
    double bestCF;
    vector<double> fitness;
    double speed;
    double r;
    double besty;
    int x;
    long M;
    double bestSolutionValue;
    int bestSolutionIndex;
    QString terminationMethod;
    vector<double> bestValues;
    int generation;
    vector<double> solutionCopy;
    vector<double> bestSolution;
    vector<double> worstSolution;
    double fitness_k;
    double worstSpeed;
    double worstSpeedNew;
    double fitness1;
    int Size;
    double max_speed;
    double worstSpeedBest;
    double worstValue;
    double worstSpeedWorst;
    double finalBestValue;
    int GaoCount;
    vector<double> lower;
    vector<double> upper;
    int similaritySize;
    double similarityValue;
    int similarityCount;
    int iters;



public:
    Armadillo1();
    double evaluate(const vector<double>& solution, double& worstSpeed) ;
    void initialize(vector<vector<double>>& population, int GaoCount);
    void GAO(int GaoCount, vector<double>& bestValues);
    virtual void init();
    void CalcFitnessArray();
    virtual void step();
    virtual void   done();
    virtual bool terminated();
     virtual void Selection();
    ~Armadillo1();
};

#endif // ARMADILLO1_H




