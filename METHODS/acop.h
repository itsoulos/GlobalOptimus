#ifndef ACOP_H
#define ACOP_H

#include <OPTIMUS/optimizer.h>

/**
 * @brief The ACOP class implements a parallel version for Ant Colony Optimization.
 */
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

    /**
     * @brief updatePheromones Updates the pheromones of ants.
     */
    void    updatePheromones();
    /**
     * @brief moveAnts Move ants to next locations.
     */
    void    moveAnts();
    /**
     * @brief randomDouble
     * @param min
     * @param max
     * @return  a new random value in range [min,max].
     */
    double  randomDouble(double min, double max);
    /**
     * @brief checkCluster
     * @param subCluster
     * @return  true if subCluster should be terminated.
     */
    bool    checkCluster(int subCluster);
    /**
     * @brief propagate Propagate best values to each thread.
     */
    void    propagate();
    /**
     * @brief propagateBetween Send the best values from sender to receiver (threads).
     * @param sender
     * @param receiver
     */
    void    propagateBetween(int sender, int receiver);
public:
    /**
     * @brief ACOP The main constructor of the class.
     */
    ACOP();
    /**
     * @brief init Initializes the ant population.
     */
    virtual void init();
    /**
     * @brief step Perform a step of the method.
     */
    virtual void step();
    /**
     * @brief terminated
     * @return true if the method should be terminated.
     */
    virtual bool terminated();
    /**
     * @brief done Called when the method terminates.
     */
    virtual void done();
    /**
     * @brief showDebug Shows debug information.
     */
    virtual void showDebug();
    virtual ~ACOP();
};

#endif // ACOP_H
