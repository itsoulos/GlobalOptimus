#ifndef PARALLELDE_H
#define PARALLELDE_H
# include <OPTIMUS/collection.h>
# include <OPTIMUS/optimizer.h>
/**
 * @brief The ParallelDe class implements the Parallel Differential evolution method.
 * Available Method Params are:
 * 1) parde_termination: The termination rule used. Available values: maxiters,similarity,doublebox.
 * 2) parde_similarityMax: The maximum allowed number of generations for the similarity stopping rule.
 * 3) parde_agents: The number of agents for every Parallel Island.
 * 4) parde_generations: The maximum number of generations allowed.
 * 5) parde_islands: The number of parallel islands for the method.
 * 6) parde_cr: The crossover probability.
 * 7) parde_weight_method: The differential weight method. The available values are: random, ali, constant.
 * 8) parde_f: The value for the differential weight when parde_weight_method=="constant".
 * 9) parde_propagate_rate: The number of generations before the propagation starts.
 * 10) parde_selection_method: The selection method used to select atoms in crossover. Available values
 *     are: random, tournament.
 * 11) parde_propagate_method: The used propagation method between islands.
 *     Available values are: 1to1,1toN,Nto1,NtoN
 */
class ParallelDe :public Optimizer
{
private:
    Matrix population;

    double parde_F, parde_CR;
    int islands, agents,generation, parde_generations, similarity_max_count, parde_islands_enable,parde_propagate_rate;
    vector<int> bestIslandIndex,similarity_current_count;
    vector<double> similarity_best_value,lmargin, rmargin,bestIslandValues,fitness_array,sum,newSum, MO, newMO;
    vector<double> doublebox_xx1, doublebox_xx2, doublebox_best_value, doublebox_variance, doublebox_stopat;
    QString parde_termination, parde_propagate_method, parde_weight_method;
    std::chrono::time_point<std::chrono::system_clock> before, after;

    /**
     * @brief selectAtom
     * @param islandIndex
     * @return the selected agent in islandIndex thread.
     */
    int     selectAtom(int islandIndex);
    /**
     * @brief tournament
     * @param islandIndex
     * @param tsize
     * @return with tournament selection an agent in islandIndex thread.
     */
    int     tournament(int islandIndex, int tsize = 8);
    /**
     * @brief islandStartPos
     * @param islandIndex
     * @return the start position of agents for thread islandIndex.
     */
    int     islandStartPos(int islandIndex);
    /**
     * @brief islandEndPos
     * @param islandIndex
     * @return the ending position of agents for thread islandIndex.
     */
    int     islandEndPos(int islandIndex);
    /**
     * @brief propagateIslandValues Send best agents to other threads.
     */
    void    propagateIslandValues();
    /**
     * @brief getBestValue Returns the position and the value of the best agent in the selected thread.
     * @param index
     * @param value
     */
    void    getBestValue(int &index, double &value);
    /**
     * @brief getBestValue Returns the position and the value of the best agent in the selected thread.
     * @param islandName
     * @param index
     * @param value
     */
    void    getBestValue(int islandName, int &index, double &value);
    /**
     * @brief getDifferentialWeight
     * @return The Differential Weight used in Differential Evolution.
     */
    double  getDifferentialWeight();
    /**
     * @brief replaceValueInIsland Sends values in islands.
     * @param islandIndex
     * @param x
     * @param y
     */
    void    replaceValueInIsland(int islandIndex, Data &x, double &y);
    /**
     * @brief checkIsland
     * @param islandName
     * @return true if the thread islandName should be terminated.
     */
    bool    checkIsland(int islandName);

    //for similarity stopping rule
    double global_sim_value;
    int global_sim_count;

    double start,end;
    vector<Similarity> threadSimilarity;
    vector<DoubleBox> threadDoublebox;
public:
    /**
     * @brief ParallelDe The constructor of DE.
     */
    ParallelDe();
    /**
     * @brief terminated
     * @return true if DE should be terminated.
     */
    virtual bool terminated();
    /**
     * @brief step Executes a step of DE method.
     */
    virtual void step();
    /**
     * @brief init Initializes the optimizer.
     */
    virtual void init();
    /**
     * @brief done It is executed when DE finishes.
     */
    virtual void done();
    virtual ~ParallelDe();
};

#endif // PARALLELDE_H
