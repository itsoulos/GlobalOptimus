#ifndef GENETIC_H
#define GENETIC_H
# include <OPTIMUS/collection.h>
# include <OPTIMUS/optimizer.h>


class Genetic : public Optimizer
{
private:

    vector<Data> population;
    vector<Data> childrenArray;
    Data         fitnessArray;

    Collection chromosomes;
    Collection children;
    QString selectionMethod;
    QString crossoverMethod;
    QString mutationMethod;
    QString terminationMethod;
    double selectionRate;
    double mutationRate;
    double localsearchRate;
    int    chromosomeCount;
    int    maxGenerations;
    int    generation;
    int    tournamentSize;
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
    double deltaIter(int iter,double y);
    ~Genetic();
};

#endif // GENETIC_H
