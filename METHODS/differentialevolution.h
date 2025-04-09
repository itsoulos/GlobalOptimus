#ifndef DIFFERENTIALEVOLUTION_H
#define DIFFERENTIALEVOLUTION_H
# include <OPTIMUS/collection.h>
# include <OPTIMUS/optimizer.h>

/**
 * @brief The DifferentialEvolution class implements the Differential Evolution global optimizer.
 */
class DifferentialEvolution : public Optimizer
{
private:
    vector<Data> agentx;
    Data agenty;
    int NP;
    double F;
    double CR;
    int maxiters;
    int iter;
    Data bestx;
    double besty;
    int tournament();
    double sumFitness=0.0;
    int    sumItersCount = 0;
     vector<double> migrantWeights;
public:
    /**
     * @brief DifferentialEvolution The constructor of the class.
     */
    DifferentialEvolution();
    /**
     * @brief init Initializes the agents.
     */
    virtual void init();
    /**
     * @brief step Performs a step of the method.
     */
    virtual void step();
    /**
     * @brief terminated
     * @return true if DE should be terminated.
     */
    virtual bool terminated();
    /**
     * @brief showDebug Displays debug  information.
     */
    virtual void showDebug();
    /**
     * @brief done Called when DE terminates.
     */
    virtual void done();
    /**
     * @brief calculateMigrantWeights Calculate migration weights
     * for the migration de scheme.
     */
    void    calculateMigrantWeights();
    virtual ~DifferentialEvolution();

};

#endif // DIFFERENTIALEVOLUTION_H
