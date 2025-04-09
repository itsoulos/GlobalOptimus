#ifndef GWOOPTIMIZER_H
#define GWOOPTIMIZER_H
# include <OPTIMUS/collection.h>
# include <OPTIMUS/optimizer.h>
/**
 * @brief The GWOoptimizer class implements the Grey Wolf Optimizer.
 */
class GWOoptimizer : public Optimizer
{
private:
    Data Alpha_pos,Beta_pos,Delta_pos;
    vector<Data> Positions;
    Data fitnessArray;
    double Alpha_score,Beta_score,Delta_score;
    int SearchAgents_no, Max_iter;
    int iter;
    double sumMean;
public:
    /**
     * @brief GWOoptimizer  The constructor of the method.
     */
    GWOoptimizer();
    /**
     * @brief init  Initializes the optimizer.
     */
    virtual void init();
    /**
     * @brief step Performs a step of GWO.
     */
    virtual void step();
    /**
     * @brief terminated
     * @return true when the GWO should be terminated.
     */
    virtual bool terminated();
    /**
     * @brief showDebug Displays debug information.
     */
    virtual void showDebug();
    /**
     * @brief done It is executed when GWO finishes.
     */
    virtual void done();
    virtual ~GWOoptimizer();
};

#endif // GWOOPTIMIZER_H
