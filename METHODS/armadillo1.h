#ifndef ARMADILLO1_H
#define ARMADILLO1_H
# include <OPTIMUS/problem.h>
# include <OPTIMUS/optimizer.h>

/**
 * @brief The Armadillo1 class implements the Giant Armadillo Optimizer.
 */
class Armadillo1: public Optimizer
{
private:
    double localsearchRate;
    int maxGenerations;
    vector<vector<double>> population;
    double bestCF;
    vector<double> fitness;
    double speed;
    double r;
    double besty;
    int x;
    long M;
    double bestSolutionValue;
    int bestSolutionIndex;
    vector<double> bestValues;
    int generation;
    vector<double> solutionCopy;
    vector<double> bestSolution;
    vector<double> worstSolution;
    double fitness_k;
    double worstSpeed;
    double worstSpeedNew;
    double fitness1;
    int Size;
    double max_speed;
    double worstSpeedBest;
    double worstValue;
    double worstSpeedWorst;
    double finalBestValue;
    int GaoCount;
    vector<double> lower;
    vector<double> upper;
    int similaritySize;
    double similarityValue;
    int similarityCount;
    int iters;
public:
    /**
     * @brief Armadillo1 The constructor of the class.
     */
    Armadillo1();
    /**
     * @brief evaluate
     * @param solution
     * @param f
     * @param worstSpeed
     * @return The fitness value for an element.
     */
    double  evaluate(const vector<double>& solution,double f, double& worstSpeed) ;
    /**
     * @brief initialize Initialize the population.
     * @param population
     * @param GaoCount
     */
    void    initialize(vector<vector<double>>& population, int GaoCount);
    /**
     * @brief GAO Perform the main step.
     * @param GaoCount
     * @param bestValues
     */
    void    GAO(int GaoCount, vector<double>& bestValues);
    /**
     * @brief init Initializes the optimizer.
     */
    virtual void init();
    /**
     * @brief CalcFitnessArray Calculate fitness values.
     */
    void    CalcFitnessArray();
    /**
     * @brief step Perform a step of the method.
     */
    virtual void step();
    /**
     * @brief done Called when the optimizer finishes.
     */
    virtual void   done();
    /**
     * @brief terminated
     * @return true if the optimizer should be terminated.
     */
    virtual bool terminated();
    /**
     * @brief Selection Sorts the elements according to the fitness values.
     */
    virtual void Selection();
    /**
     * @brief showDebug Displays debug information.
     */
    virtual void showDebug();

    virtual ~Armadillo1();
};

#endif // ARMADILLO1_H




