#ifndef SAO_H
#define SAO_H
#include <OPTIMUS/optimizer.h>

class SAO : public Optimizer
{
private:
    int dimension, population, maxIterations, iteration;
    double localSearchRate, bestF2x;
    vector<double> fitnessArray, bestSamply;
    vector<vector<double>> samples;
    chrono::time_point<chrono::system_clock> before, after;
    double randomDouble(double, double);
    double evaporationRate;
    vector<double> smellIntensity;
    double calculateSmellIntensity(double fitnessValue);
    void evaporateSmellIntensity();
public:
    SAO();
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void done();
    virtual void showDebug();
    virtual ~SAO();
};

#endif // SAO_H
