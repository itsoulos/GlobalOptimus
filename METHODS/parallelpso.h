#ifndef PARALLELPSO_H
#define PARALLELPSO_H
# include <OPTIMUS/collection.h>
# include <OPTIMUS/optimizer.h>

class ParallelPso :public Optimizer
{

private:
    int generation, parallelPsoParticles, similarityMaxCount, subCluster, parallelPsoGenerations,
        n, dimension, localSearchCount, subClusterEnable, propagateRate, bestIndex, pNumber, pRate;

    double besty, parallelPsoC1, parallelPsoC2;

    QString parallelPropagateMethod;

    vector<int> similarityCurrentCount, bestF2xInClusterIndex;

    vector<double> bestx, fitness_array, best_fitness_array, lmargin, rmargin, similarityBestValue, sum, newSum, MO, newMO, bestF2xInCluster, bestF2xInClusterOLD;

    vector<Data> particles, bestParticle, velocitys, bestParticleInCluster;

    std::chrono::time_point<std::chrono::system_clock> before, after;

    double fitness(Data &x);
    int subClusterStartPos(int subClusterIndex);
    int subClusterEndPos(int subClusterIndex);
    void propagate();
    void getBestValue(int &index, double &value);
    void getBestValue(int islandName, int &index, double &value);
    void replace(int subClusterIndex, vector<pair<double,Data>> proParticles);
    bool checkSubCluster(int subClusterName);
public:
    ParallelPso();
    virtual bool terminated();
    virtual void step();
    virtual void init();
    virtual void done();
    virtual ~ParallelPso();
};

#endif // PARALLELPSO_H
