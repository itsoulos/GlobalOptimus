#ifndef EGO_H
#define EGO_H
# include <OPTIMUS/problem.h>
# include <OPTIMUS/optimizer.h>

/**
 * @brief The EGO class implements the EEL optimizer.
 */
class EGO:  public Optimizer
{

private:
    double localsearchRate;
    int maxGenerations;
        vector<vector<double>> Theseis;
    vector<double> fitness;
    vector<double> grouperBestThesi;
    double grouperBestFitness;
    vector<double> eelThesi;
    vector<double> kambili;
    long D;
    double newFitness;
    double y=1e+10;
    Optimizer *local=NULL;
    QString terminationMethod;
    int SearchAgents;
    vector<double> upper;
    vector<double> lower;
    int similaritySize;
    double similarityValue;
    int similarityCount;
    double r1 ;
    double r2 ;
    double r3 ;
    double r4;
    double C1;
    double C2;
    double b;
    int t;
    double p ;
    Data bestX;
    double distance2eel;
    double X1;
    double distance2grouper;
    double X2;
    double a ;
    double starvation_rate;
    int iters;
    int generation;
    Data best;
    int mod1, mod2, mod3;

public:
    /**
     * @brief EGO The constructor of the class.
     */
    EGO();
    /**
     * @brief init Initializes the population.
     */
    virtual void init();
    /**
     * @brief evaluate
     * @param solution
     * @param grouperBestFitness
     * @return the fitness of an item.
     */
    double  evaluate( vector<double>& solution, double& grouperBestFitness);
    /**
     * @brief step Performs a step of the method.
     */
    virtual void step();
    /**
     * @brief done It is called when the optimizer finishes.
     */
    virtual void   done();
    /**
     * @brief terminated
     * @return true if the optimizer should be terminated.
     */
    virtual bool terminated();
    /**
     * @brief Selection Sorts the population according to the fitness values.
     */
    void    Selection();
    /**
     * @brief CalcFitnessArray Calculates the fitness values for all items.
     */
    void    CalcFitnessArray();
    /**
     * @brief Feasibility
     * @param solution
     * @return true if the solution is inside the bounds of the problem.
     */
    bool    Feasibility(const vector<double>& solution);
    virtual ~EGO();
};

#endif // EGO_H



