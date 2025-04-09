#ifndef NEURALMINIMIZER_H
#define NEURALMINIMIZER_H

# include <OPTIMUS/collection.h>
# include <OPTIMUS/optimizer.h>
/**
 * @brief The NeuralMinimizer class implements the recently introduced method of Neural Minimizer.
 */
class NeuralMinimizer : public Optimizer
{
private:

    int olditer =0;
    NeuralSampler *neuralSampler;
    RbfSampler    *rbfSampler;
    //NNCSampler    *nncSampler;
    ProblemSampler *sampler;
    int iter ;
    Matrix xsample;
    Data   ysample;
    Data bestx;
    double besty;

    //for the doublebox stopping rule
    double doublebox_xx1,doublebox_xx2,doublebox_stopat,
        doublebox_variance,doublebox_oldBesty;

    //for the similarity stopping rule
    double similarity_best_value;
    int similarity_current_count,similarity_max_count;

    Data minima;
public:
    /**
     * @brief NeuralMinimizer The constructor of the class.
     */
    NeuralMinimizer();
    /**
     * @brief terminated
     * @return true when the method should be terminated.
     */
    virtual bool terminated();
    /**
     * @brief step Performs a step of the method.
     */
    virtual void step();
    /**
     * @brief init Initializes the samples used in the method.
     */
    virtual void init();
    /**
     * @brief done It is called when the NeuralMinimizer terminates.
     */
    virtual void done();
    /**
     * @brief showDebug Displays debug information.
     */
    virtual void showDebug();
    virtual ~NeuralMinimizer();
};

#endif // NEURALMINIMIZER_H
