#ifndef PARALLELBGWO_H
#define PARALLELBGWO_H

#include <OPTIMUS/optimizer.h>


class ParallelBGwo : public Optimizer
{
private:
    int dimension, population, maxIterations, iteration, subPopulation, subPopEnable, sub_size, prop, propagationNumber, propagationRate;
    double localSearchRate, bF2x;
    int similaritySize;
    QString subPopEnableStr;
    double similarityValue;
    int similarityCount;
     QString terminationMethod;
    vector<double> fitnessArray, bestF2x, bSamply, bestF2xOld;
    vector<vector<double>> samples, bestSamply;
    vector<double> bestF2x2, bestF2x3;
    vector<vector<double>> bestSamply2, bestSamply3;
    chrono::time_point<chrono::system_clock> before, after;
    double randomDouble(double, double);
    double k_coeff;
    double a1, a2, a3;
    double convergenceFactorA() const;
    void updateLeadersForSubpop(int k);
    bool checkCluster(int subCluster);
    vector<int> similarityCurrentCountBest;
    QString propagationMethod;
    void propagate();
    void propagateBetween(int sender, int receiver);

public:
    ParallelBGwo();
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void done();
    virtual void showDebug();
    virtual ~ParallelBGwo();
};

#endif // PARALLELGWO_H
