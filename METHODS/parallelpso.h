
#ifndef PARALLELPSO_H
#define PARALLELPSO_H
# include <OPTIMUS/collection.h>
# include <OPTIMUS/optimizer.h>

/**
 * @brief The ParallelPso class implements the parallel version of PSO.
 */
class ParallelPso :public Optimizer
{

private:
    int generation, parallelPsoParticles, similarityMaxCount, subCluster, parallelPsoGenerations,
        n, dimension, localSearchCount, subClusterEnable, propagateRate, bestIndex, pNumber, pRate;

    double besty, parallelPsoC1, parallelPsoC2;

    QString parallelPropagateMethod,parallelPso_termination;
    vector<int> similarityCurrentCount, bestF2xInClusterIndex;
    vector<double> bestx, fitness_array, best_fitness_array, lmargin, rmargin, similarityBestValue, sum, newSum, MO, newMO, bestF2xInCluster, bestF2xInClusterOLD;
    vector<Data> particles, bestParticle, velocitys, bestParticleInCluster;
    std::chrono::time_point<std::chrono::system_clock> before, after;

    /**
     * @brief fitness
     * @param x
     * @return the fitness of particle x.
     */
    double  fitness(Data &x);
    /**
     * @brief subClusterStartPos
     * @param subClusterIndex
     * @return  the start position of particles for thread subClusterIndex.
     */
    int     subClusterStartPos(int subClusterIndex);
    /**
     * @brief subClusterEndPos
     * @param subClusterIndex
     * @return  the end position of particles for thread subClusterIndex.
     */
    int     subClusterEndPos(int subClusterIndex);
    /**
     * @brief propagate Sends the best particles from thread to thread.
     */
    void    propagate();
    /**
     * @brief getBestValue Returns  the fitness value in index thread.
     * @param index
     * @param value
     */
    void    getBestValue(int &index, double &value);
    /**
     * @brief getBestValue  Returns  the fitness value in index thread.
     * @param islandName
     * @param index
     * @param value
     */
    void    getBestValue(int islandName, int &index, double &value);
    /**
     * @brief replace Replaces a series of particles in the subClusterIndex thread.
     * @param subClusterIndex
     * @param proParticles
     */
    void    replace(int subClusterIndex, vector<pair<double,Data>> proParticles);
    /**
     * @brief checkSubCluster
     * @param subClusterName
     * @return true if the thread subClusterName should be terminated.
     */
    bool    checkSubCluster(int subClusterName);
    vector<Similarity> threadSimilarity;
    vector<DoubleBox> threadDoublebox;
public:
    /**
     * @brief ParallelPso The constructor of parallel PSO.
     */
    ParallelPso();
    /**
     * @brief terminated
     * @return true when the parallel PSO should be terminated.
     */
    virtual bool terminated();
    /**
     * @brief step Executes a step of the method.
     */
    virtual void step();
    /**
     * @brief init Initializes the particles and the corresponding velocities.
     */
    virtual void init();
    /**
     * @brief done This method is executed when the optimizer finishes.
     */
    virtual void done();
    virtual ~ParallelPso();
};

#endif // PARALLELPSO_H
