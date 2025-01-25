#ifndef AQUILA_H
#define AQUILA_H
#include <OPTIMUS/collection.h>
#include <OPTIMUS/optimizer.h>

class Aquila : public Optimizer
{
private:
    int dimension, population, maxIterations, iteration;
    double localSearchRate, bestF2x, a_min, a_max, b;
    vector<double> fitnessArray, bestSamply;
    vector<vector<double>> samples;
    chrono::time_point<chrono::system_clock> before, after;
    double randomDouble(double, double);

public:
    Aquila();
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void done();
    virtual void showDebug();
    virtual ~Aquila();
};

#endif // AQUILA_H
