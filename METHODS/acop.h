#ifndef ACOP_H
#define ACOP_H

#include <OPTIMUS/optimizer.h>

class ACOP : public Optimizer
{
private:
    int dimension, population, maxIterations, iteration, subPopulation, subPopEnable, sub_size, prop, propagationNumber, propagationRate;
    double localSearchRate, alpha, evaporationRate, beta, Q, bestF2xAll;
    vector<double> bestF2x, bestF2xOld, fitnessArray, bestAllSamply;
    vector<vector<double>> pheromones, antPositions, bestSamply;
    vector<int> similarityCurrentCountBest;
    QString propagationMethod;
    chrono::time_point<std::chrono::system_clock> before, after;

    void updatePheromones();
    void moveAnts();
    double randomDouble(double min, double max);
    bool checkCluster(int subCluster);
    void propagate();
    void propagateBetween(int sender, int receiver);
public:
    ACOP();
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void done();
    virtual void showDebug();
    virtual ~ACOP();
};

#endif // ACOP_H
