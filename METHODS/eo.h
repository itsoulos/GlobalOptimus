#ifndef EO_H
#define EO_H

#include <OPTIMUS/optimizer.h>
#include <unordered_map>
#include <functional>
#include <vector>

using namespace std;

class EO : public Optimizer
{
private:
    int dimension, population, maxIterations, iteration, reject, technique;
    double localSearchRate, bestFitness, worstFitness, initialDecay, finalDecay, reflectionFactor;
    vector<double> fitnessArray, bestSample, worstSample;
    vector<vector<double>> positions;
    vector<vector<double>> bestEchoes;
    chrono::time_point<chrono::system_clock> before, after;

    // Μνήμη αντιλάλων
    vector<vector<double>> echoMemory;
    vector<double> echoMemoryFitness;
    const size_t maxMemorySize = 1000;
    size_t memoryHits = 0;
    bool isInMemory(const vector<double>& solution, double& storedFitness);

    double randomDouble(double min, double max);
    vector<double> generateEcho(const vector<double> &source, double currentDecay);
    void updateBestSolution();
    double approximateFitness(const vector<double> &x, const vector<double> &best, double decay);
    bool MDM(const vector<double>& sample);

public:
    EO();
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void done();
    virtual void showDebug();
    virtual ~EO();
};

#endif // EO_H
