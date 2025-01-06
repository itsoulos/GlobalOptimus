#ifndef HYBRIDMETHOD_H
#define HYBRIDMETHOD_H
#include <OPTIMUS/optimizer.h>

class HybridMethod : public Optimizer
{
private:
    int dimension, population, maxIterations, iteration,calculation, rejection;
    double localSearchRate, bestF2x;
    vector<double> fitnessArray, bestSamply;
    vector<vector<double>> samples, closeSamples, samplesCopy;
    QString lineSearchMethod;
    vector<pair<Data, Data>> closestSamples;

    vector<double> findSamply(const vector<double> &, const vector<double> &, double);
    double goldenSectionSearch(const vector<double> &, const vector<double> &, double);
    double armijoSearch(const vector<double>&, const vector<double>&, double);
    double euclideanDistance(const Data &, const Data &);
    void findTwoClosestSamplesForEachSamply();
    double rDouble(double, double);
    Data makeChild(Data&, Data&);
    chrono::time_point<chrono::system_clock> before, after;
public:
    HybridMethod();
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void done();
    virtual void showDebug();
    virtual ~HybridMethod();


};

#endif // HYBRIDMETHOD_H
