#ifndef AOA_H
#define AOA_H
#include <OPTIMUS/optimizer.h>

class AOA : public Optimizer
{
private:
    int dimension, population, maxIterations, iteration;
    double localSearchRate, bestF2x;
    vector<double> fitnessArray, bestSamply;
    vector<vector<double>> samples;
    chrono::time_point<chrono::system_clock> before, after;
    double randomDouble(double, double);
    double adaptiveFactor;
    double calculateAdaptiveFactor(int iteration);
public:
    AOA();
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void done();
    virtual void showDebug();
    virtual ~AOA();
};

#endif // AOA_H
