#include "neuralminimizer.h"

NeuralMinimizer::NeuralMinimizer()
{
    QStringList neural_model;
    neural_model<<"neural"<<"rbf"<<"nnc";
    addParam(Parameter("neural_model",neural_model[0],
                       neural_model,
                       "Model used. Available values: neural,rbf,nnc"));
    addParam(Parameter("neural_weights",10,1,20,"The weights used in the neural sampler"));
    addParam(Parameter("neural_samples",100,10,10000,"Number of samples for Neural Minimizer"));
    addParam(Parameter("neural_iterations",200,10,2000,"Number of maximum iterations for Neural Minimizer"));
    addParam(Parameter("neural_start_samples",50,10,5000,"Number of start samples for Neural Minimizer"));
    QStringList neural_termination;
    neural_termination<<"doublebox"<<"similarity";
    addParam(Parameter("neural_termination",neural_termination[0],
                       neural_termination,
                       "The used stopping rule. Available values: similarity, doublebox"));
    addParam(Parameter("neural_similarityMaxValue",20,1,100,"The maximum number of iterations for the similarity stopping rule"));
    addParam(Parameter("neural_trainmethod","bfgs","The method used to train the neural network model. Default: bfgs"));
    sampler= NULL;
}

bool    NeuralMinimizer::terminated()
{
    ++iter;
    int neural_iterations =params["neural_iterations"].toString().toInt();
    QString neural_termination = params["neural_termination"].toString();

    if(neural_termination=="maxiters")
        return iter>=neural_iterations;
    else
        if(neural_termination=="doublebox")
        return doubleBox.terminate(besty,iter);
    else
        if(neural_termination=="similarity")
        return similarity.terminate(besty);
    return false;
}

void    NeuralMinimizer::step()
{
    int neural_samples=params["neural_samples"].toString().toInt();
    QString neural_model = params["neural_model"].toString();
    if(neural_model == "neural")
    {
        QString method=params["neural_trainmethod"].toString();
        ((NeuralSampler *)sampler)->trainModel(method);
    }
    else
        sampler->trainModel();
    xsample.clear();
    ysample.clear();
    sampler->sampleFromModel(neural_samples,xsample,ysample);
    for(int i=0;i<(int)xsample.size();i++)
    {

        double y = localSearch(xsample[i]);
        sampler->addSampleFromProblem(xsample[i],y);
        minima.push_back(y);
        if(y<besty)
        {
            besty = y;
            bestx = xsample[i];
        }
        if(terminated()) break;

    }
}


void    NeuralMinimizer::init()
{
    similarity_best_value = 1e+100;
    similarity_current_count = 0;
    int neural_similarityMaxValue = params["neural_similarityMaxValue"].toString().toInt();
    similarity_max_count= neural_similarityMaxValue;
    besty = 1e+100;
    doublebox_oldBesty = 1e+100;
    iter = 0;
    doublebox_xx1 = 0;
    doublebox_xx2 = 0;
    if(sampler!=NULL)
        delete sampler;
    int neural_weights  = params["neural_weights"].toString().toInt();
    QString neural_model = params["neural_model"].toString();
    if(neural_model=="rbf")
        sampler = new RbfSampler(myProblem,neural_weights);
    else
        sampler = new NeuralSampler(myProblem,neural_weights);

    minima.clear();
    int neural_start_samples  = params["neural_start_samples"].toString().toInt();
    sampler->sampleFromProblem(neural_start_samples,xsample,ysample);
    doubleBox.setMinIters(20);
}

void    NeuralMinimizer::showDebug()
{
    methodLogger->printMessage(QString::asprintf(
        "NeuralMinimizer. Iter=%4d BESTY=%10.5lg",iter,besty));
}

void    NeuralMinimizer::done()
{
    besty = localSearch(bestx);
    methodLogger->printMessage(
        QString::asprintf("NeuralMinimizer: Terminating with %20.10lg",besty));
}

NeuralMinimizer::~NeuralMinimizer()
{
    if(sampler!=NULL) delete sampler;

}
