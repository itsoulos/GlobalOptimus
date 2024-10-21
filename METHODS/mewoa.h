#ifndef MEWOA_H
#define MEWOA_H
#include <OPTIMUS/optimizer.h>

class MEWOA : public Optimizer
{
private:

    int dimension, population, maxIterations, iteration, subPopulation, rate;
    double localSearchRate, bestF2x, a_min, a_max, b;
    vector<double> fitnessArray, bestSamply;
    vector<vector<double>> samples, bestInSub;
    chrono::time_point<chrono::system_clock> before, after;
    double rand_double(double, double);
    void exchange_info(vector<vector<double>> &population, int sub_size);
public:
    MEWOA();
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void done();
    virtual void showDebug();
    virtual ~MEWOA();
};

#endif // MEWOA_H
