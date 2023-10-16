#ifndef NEURALMINIMIZER_H
#define NEURALMINIMIZER_H

# include <OPTIMUS/collection.h>
# include <OPTIMUS/optimizer.h>
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
    NeuralMinimizer();
    virtual bool terminated();
    virtual void step();
    virtual void init();
    virtual void done();
    virtual         ~NeuralMinimizer();
};

#endif // NEURALMINIMIZER_H
