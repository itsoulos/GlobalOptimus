#ifndef PEAO_H
#define PEAO_H
# include <OPTIMUS/collection.h>
# include <OPTIMUS/optimizer.h>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iostream>


class PEAO : public Optimizer{
private:
    QString propagationMethod;
    void propagate();
    void propagateBetween(int sender, int receiver);
    int dimension, propagationNumber, propagationRate;
    int population, prop;
    int maxIterations;
    int iteration, subPopEnable;
QString subPopEnableStr;
    int subPopulation;
    int sub_size;
    vector<Similarity> threadSimilarity;
    vector<DoubleBox> threadDoublebox;
    int communicationThreshold;
    int levyBetaTimes10;
    int communicationStrategyMode; /* 0=random, 1=strategy1, 2=strategy2, 3=strategy3 */

    double localSearchRate;
    double enzymeConcentration;

    QString termination;

    std::vector<double> fitnessArray;
    std::vector<std::vector<double>> samples;

    std::vector<double> bestF2x;
    std::vector<double> worstF2x;
    std::vector<int> bestIndex;
    std::vector<int> worstIndex;

    std::vector<std::vector<double>> bestSamply;
    std::vector<std::vector<double>> worstSamply;

    std::vector<double> bestF2xOld;
    std::vector<int> similarityCurrentCountBest;

    std::vector<double> bSamply;
    double bF2x;

    std::chrono::time_point<std::chrono::system_clock> before;
    std::chrono::time_point<std::chrono::system_clock> after;

private:
    double randomDouble(double minValue, double maxValue);
    double uniform01();
    double adaptiveFactor(void) const;
    double levyBeta(void) const;
    double levySigma(double beta) const;
    double levyScalar(double beta);
    std::vector<double> levyVector(int dimensionValue, double beta);
    double similarityValue;
    vector<int> similarityCurrentCount;
    int similarityCount;
    void updateLeadersForSubpop(int subPopIndex);
    void updateAllLeaders(void);

    std::vector<double> createCandidate1(
        const std::vector<double> &currentPosition,
        const std::vector<double> &globalBestPosition,
        double adaptiveFactorValue);

    std::vector<double> createCandidate2A(
        const std::vector<double> &currentPosition,
        const std::vector<double> &globalBestPosition,
        const std::vector<double> &differenceVector,
        double adaptiveFactorValue);

    std::vector<double> createCandidate2B(
        const std::vector<double> &currentPosition,
        const std::vector<double> &globalBestPosition,
        const std::vector<double> &differenceVector,
        double adaptiveFactorValue);

    std::vector<double> buildDifferenceVector(void);

    bool isFirstFitnessBetter(double firstFitness, double secondFitness) const;

    void communicate(void);
    void communicationStrategy1(void);
    void communicationStrategy2(void);
    void communicationStrategy3(void);

    bool checkSubCluster(int subCluster);

public:
    PEAO();
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void done();
    virtual void showDebug();
    virtual ~PEAO();
};

#endif /* PEAO_H */
