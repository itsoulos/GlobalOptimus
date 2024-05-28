#ifndef GENETIC_H
#define GENETIC_H
# include <OPTIMUS/collection.h>
# include <OPTIMUS/optimizer.h>

# define LOCAL_NONE "none"
# define LOCAL_CROSS "crossover"
# define LOCAL_MUTATE "mutate"
# define LOCAL_SIMAN "siman"

class Genetic : public Optimizer
{
private:

    vector<Data> population;
    vector<Data> childrenArray;
    Data         fitnessArray;

    Data bestx;
    double besty;
    bool hasInitialized;
    int lsearchItems,lsearchGens;

    Collection chromosomes;
    Collection children;
    QString selectionMethod;
    QString crossoverMethod;
    QString mutationMethod;
    QString terminationMethod;
    QString localSearchMethod;
    double selectionRate;
    double mutationRate;
    double localsearchRate;
    int    chromosomeCount;
    int    maxGenerations;
    int    generation;
    int    tournamentSize;
    void    localCrossover(int pos);
    void    localMutate(int pos);
    void    localSiman(int pos);

public:
    Genetic();
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void showDebug();
    virtual void done();
    Collection  makeChromosomesForRoulette();
    int     selectWithTournament(int size=8);
    int     selectWithRoulette(Collection &copy);
    void    makeChildrenUniform(Data &parent1,Data &parent2,Data &x1,Data &x2);
    void    makeChildrenOnePoint(Data &parent1,Data &parent2,Data &x1,Data &x2);
    void    makeChildrenDouble(Data &parent1,Data &parent2,Data &x1,Data &x2);

    void    CalcFitnessArray();
    void    Selection();
    void    Crossover();
    void    Mutate();
    void    LocalSearch(int pos);
    void    setBest(Data &xx,double y);
    double deltaIter(int iter,double y);
    ~Genetic();
};

#endif // GENETIC_H
