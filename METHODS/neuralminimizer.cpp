#include "neuralminimizer.h"

NeuralMinimizer::NeuralMinimizer()
{
    addParam(Parameter("neural_model","neural","Model used. Available values: neural,rbf,nnc"));
    addParam(Parameter("neural_weights","10","The weights used in the neural sampler"));
    addParam(Parameter("neural_samples","100","Number of samples for Neural Minimizer"));
    addParam(Parameter("neural_iterations","200","Number of maximum iterations for Neural Minimizer"));
    addParam(Parameter("neural_start_samples","50","Number of start samples for Neural Minimizer"));
    addParam(Parameter("neural_termination","doublebox","The used stopping rule. Available values: similarity, doublebox"));
    addParam(Parameter("neural_similarityMaxValue","20","The maximum number of iterations for the similarity stopping rule"));
    addParam(Parameter("neural_trainmethod","bfgs","The method used to train the neural network model. Default: bfgs"));
    sampler= NULL;
}

double doublebox_xx1=0.0,doublebox_xx2=0.0,doublebox_variance=0.0,
       doublebox_stopat=0.0,doublebox_oldBesty=1e+10;
bool    NeuralMinimizer::terminated()
{
    ++iter;
    int neural_iterations =params["neural_iterations"].toString().toInt();
    QString neural_termination = params["neural_termination"].toString();
    double fmin=fabs(1.0+fabs(besty));
    doublebox_xx1+=fmin;
    doublebox_xx2+=fmin * fmin;

    iter++;
    doublebox_variance = doublebox_xx2/(iter+1) -(doublebox_xx1/(iter+1))*(doublebox_xx1/(iter+1));

    if(fabs(besty-doublebox_oldBesty)>1e-6)
    {
        doublebox_oldBesty=besty;
        doublebox_stopat=doublebox_variance/2.0;
    }
    return iter>=neural_iterations|| (doublebox_variance<=doublebox_stopat && iter>=20);


    if(neural_termination=="maxiters")
        return iter>=neural_iterations;
    else
        if(neural_termination=="doublebox")
        return doubleBox.terminate(besty);
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

}

void    NeuralMinimizer::done()
{
    besty = localSearch(bestx);
    printf("Neural Minimizer: Terminating with %20.10lg\n",besty);
}

NeuralMinimizer::~NeuralMinimizer()
{
    if(sampler!=NULL) delete sampler;

}
