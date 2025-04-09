#ifndef GENETIC_H
#define GENETIC_H
# include <OPTIMUS/collection.h>
# include <OPTIMUS/optimizer.h>

# define LOCAL_NONE "none"
# define LOCAL_CROSS "crossover"
# define LOCAL_MUTATE "mutate"
# define LOCAL_SIMAN "siman"

/**
 * @brief The Genetic class implements a genetic algorithm.
 */
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
    /**
     * @brief Genetic The constructor of the method.
     */
    Genetic();
    /**
     * @brief init Initializes the genetic population.
     */
    virtual void init();
    /**
     * @brief step Performs a step of the genetic algorithm.
     */
    virtual void step();
    /**
     * @brief terminated
     * @return true if the method should be terminated.
     */
    virtual bool terminated();
    /**
     * @brief showDebug Displays debug information.
     */
    virtual void showDebug();
    /**
     * @brief done It is called when the optimizer should be finished.
     */
    virtual void done();
    /**
     * @brief makeChromosomesForRoulette
     * @return a collection with chromosomes produced for roulette selection.
     */
    Collection  makeChromosomesForRoulette();
    /**
     * @brief selectWithTournament
     * @param size
     * @return the position of a chromosome selected with tournament selection.
     */
    int         selectWithTournament(int size=8);
    /**
     * @brief selectWithRoulette
     * @param copy
     * @return a selected chromosome with roulette wheel selection.
     */
    int         selectWithRoulette(Collection &copy);
    /**
     * @brief makeChildrenUniform Produces offsprings using uniform crossover.
     * @param parent1
     * @param parent2
     * @param x1
     * @param x2
     */
    void        makeChildrenUniform(Data &parent1,Data &parent2,Data &x1,Data &x2);
    /**
     * @brief makeChildrenOnePoint Produces offsprings using one point crossover.
     * @param parent1
     * @param parent2
     * @param x1
     * @param x2
     */
    void        makeChildrenOnePoint(Data &parent1,Data &parent2,Data &x1,Data &x2);
    /**
     * @brief makeChildrenDouble Produces childrens using double crossover.
     * @param parent1
     * @param parent2
     * @param x1
     * @param x2
     */
    void        makeChildrenDouble(Data &parent1,Data &parent2,Data &x1,Data &x2);
    /**
     * @brief CalcFitnessArray Calculates the fitness values for all chromosomes.
     */
    void        CalcFitnessArray();
    /**
     * @brief Selection Sorts the chromosomes according to the fitness values.
     */
    void        Selection();
    /**
     * @brief Crossover Performs crossover.
     */
    void        Crossover();
    /**
     * @brief Mutate Performs mutation.
     */
    void        Mutate();
    /**
     * @brief LocalSearch Enhances the chromosome at position pos.
     * @param pos
     */
    void        LocalSearch(int pos);
    /**
     * @brief setBest Sets the best chromosome to (x,y).
     * @param xx
     * @param y
     */
    void        setBest(Data &xx,double y);
    /**
     * @brief deltaIter
     * @param iter
     * @param y
     * @return
     */
    double      deltaIter(int iter,double y);
    virtual ~Genetic();
};

#endif // GENETIC_H
