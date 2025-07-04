#ifndef EO_H
#define EO_H

#include <OPTIMUS/optimizer.h>
#include <unordered_map>
#include <functional>
#include <vector>
#include <chrono>

using namespace std;

class EO : public Optimizer
{
private:
    int dimension, population, maxIterations, iteration, reject, technique;
    double localSearchRate, bestFitness, worstFitness, initialDelay, finalDelay, reflectionFactor;
    vector<double> fitnessArray, bestSample, worstSample;
    vector<vector<double>> positions;
    vector<vector<double>> bestEchoes;
    chrono::time_point<chrono::system_clock> before, after;

    // Μνήμη αντιλάλων
    vector<vector<double>> echoMemory;
    vector<double> echoMemoryFitness;
    const size_t maxMemorySize = 500;
    size_t memoryHits = 0;
    bool isInMemory(const vector<double>& solution, double& storedFitness);

    // Long-Term Memory
    vector<vector<double>> longTermMemory;
    vector<double> longTermMemoryFitness;
    const size_t maxLongTermMemorySize = 500;
    double longTermMemoryThreshold = 0.1;
    void updateLongTermMemory(const vector<double>& solution, double fitness);
    vector<double> getFromLongTermMemory();
    bool LTMA_t;
    int Pr;
    double randomDouble(double min, double max);
    vector<double> generateEcho(const vector<double> &source, double currentDecay);
    void updateBestSolution();
    double approximateFitness(const vector<double> &x, const vector<double> &best, double decay);
    bool MDM(const vector<double>& sample);

    // Βελτιωμένα μέτρα ποικιλομορφίας
    double explorationTotal = 0.0;
    double exploitationTotal = 0.0;
    int balanceUpdates = 0;
    vector<double> diversityHistory;
    vector<double> balanceHistory;  // Αποθήκευση ισορροπίας ανά βήμα
    double initialBestFitness;
    double initialDiversity;
    double calculateBalanceIndex();
    double calculatePopulationDiversity();
    vector<double> getDiversityHistory() const { return diversityHistory; }

    void printDiversityMetrics() const;
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
