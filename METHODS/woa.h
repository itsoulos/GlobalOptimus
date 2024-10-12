#ifndef WOA_H
#define WOA_H
#include <OPTIMUS/optimizer.h>

class WOA : public Optimizer
{
private:
    int dimension, population, maxIterations, iteration;
    double localSearchRate, bestF2x, a_min, a_max, b;
    vector<double> fitnessArray, bestSamply;
    vector<vector<double>> samples;
    chrono::time_point<chrono::system_clock> before, after;
    double rand_double(double, double);

public:
    WOA();
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void done();
    virtual void showDebug();
    virtual ~WOA();
};

#endif // WOA_H
