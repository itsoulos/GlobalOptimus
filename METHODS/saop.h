#ifndef SAOP_H
#define SAOP_H
#include <OPTIMUS/optimizer.h>

class SAOP : public Optimizer
{
private:
    int dimension, population, maxIterations, iteration, subPopulation, subPopEnable, sub_size, prop, propagationNumber, propagationRate;
    double localSearchRate, bF2x;
    vector<double> fitnessArray,bestF2x, bSamply, bestF2xOld;
    vector<vector<double>> samples, bestSamply;
    chrono::time_point<chrono::system_clock> before, after;
    double randomDouble(double, double);
    double evaporationRate;
    vector<double> smellIntensity;
    double calculateSmellIntensity(double fitnessValue);
    void evaporateSmellIntensity();
    bool checkCluster(int subCluster);
    vector<int> similarityCurrentCountBest;
    QString propagationMethod;
    void propagate();
    void propagateBetween(int sender, int receiver);
public:
    SAOP();
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void done();
    virtual void showDebug();
    virtual ~SAOP();
};

#endif // SAOP_H

