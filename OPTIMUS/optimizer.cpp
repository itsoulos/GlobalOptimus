#include "optimizer.h"
# include <METHODS/gradientdescent.h>
# include <METHODS/bfgs.h>
# include <METHODS/lbfgs.h>
Optimizer::Optimizer()
{
    problemSampler=NULL;
    myProblem = NULL;
    addParam(Parameter("opt_debug","yes","Set it to yes to show messages"));
    addParam(Parameter("opt_localsearch","bfgs","The method used in localsearch"));
    addParam(Parameter("opt_doubleboxminiters","5","Minimum iters to terminate the doublebox"));
    addParam(Parameter("opt_similarityminiters","5","Minimum number of iters for similarity stopping rule"));
    addParam(Parameter("opt_similaritycriterion","5","Number of iterations for same value in the similarity stopping rule"));
    addParam(Parameter("opt_sampler","uniform","Sampling method. Available values: uniform, triangular, maxwell, mlp, rbf, kmeans"));
    addParam(Parameter("opt_mlpsamples","10","Number of MLP samples"));
    addParam(Parameter("opt_mlpweights","10","Number of MLP weights"));
    addParam(Parameter("opt_rbfsamples","10","Number of RBF samples"));
    addParam(Parameter("opt_rbfweights","10","Number of RBF weights"));
}

void        Optimizer::setProblem(Problem *p)
{
    myProblem = p;
}

void        Optimizer::addParam(Parameter p)
{
   params[p.getName()]=p.getValue();
   paramsHelp[p.getName()]=p.getHelp();
}

void        Optimizer::setParam(QString name,QString value,QString help)
{
    params[name]=value;
    if(help.size()!=0)
    paramsHelp[name]=help;
}

Parameter   Optimizer::getParam(QString name)
{
    if(!params.contains(name))
    {
        Parameter pt(name,"","");
        return pt;
    }
    Parameter pt;
    pt.setName(name);
    pt.setValue(params[name].toString());
    pt.setHelp(paramsHelp[name].toString());
    return pt;

}

void    Optimizer::init()
{

}

void    Optimizer::step()
{

}

bool    Optimizer::terminated()
{
    return true;
}


void    Optimizer::showDebug()
{

}
void    Optimizer::solve()
{
    bool debug = getParam("opt_debug").getValue()=="yes"?true:false;
    doubleBox.setMinIters(getParam("opt_doubleboxminiters").getValue().toInt());
    similarity.setMinIters(getParam("opt_similarityminiters").getValue().toInt());
    similarity.setSimilarityIterations(getParam("opt_similaritycriterion").getValue().toInt());
    doubleBox.init();
    similarity.init();
    if(problemSampler!=NULL)
        delete problemSampler;
    QString sampling = getParam("opt_sampler").getValue();
    if(sampling=="uniform")
        problemSampler=new UniformSampler(myProblem);
    else
    if(sampling=="maxwell")
        problemSampler=new MaxwellSampler(myProblem);
    else
    if(sampling=="triangular")
        problemSampler=new TriangularSampler(myProblem);
    else
    if(sampling=="mlp")
    {
        int w = getParam("opt_mlpweights").getValue().toInt();
        problemSampler=new NeuralSampler(myProblem,w);
    }

    init();
    do
    {
        step();
        if(debug) showDebug();
    }while(!terminated());
    done();
}

void    Optimizer::sampleFromProblem(int &N,Matrix &xsample,Data &ysample)
{
    QString sampling = getParam("opt_sampler").getValue();
    if(sampling=="rbf")
    {
        int M = getParam("opt_rbfsamples").getValue().toInt();
        problemSampler->sampleFromProblem(M,xsample,ysample);
    }
    else
    if(sampling=="mlp")
    {
        int M = getParam("opt_mlpsamples").getValue().toInt();
        problemSampler->sampleFromProblem(M,xsample,ysample);
    }
    else
    if(sampling=="nnc")
    {
        int M=500;
        problemSampler->sampleFromProblem(M,xsample,ysample);
    }
    else
        problemSampler->sampleFromProblem(N,xsample,ysample);
    problemSampler->trainModel();
    problemSampler->sampleFromModel(N,xsample,ysample);
}

double  Optimizer::localSearch(Data &x)
{

    QString localMethod = getParam("opt_localsearch").getValue();
    double y=1e+10;
    Optimizer *local=NULL;
    if(localMethod == "gradient")
    {
        local = new GradientDescent();
        local->setProblem(myProblem);
        local->setParam("opt_debug","no");
        ((GradientDescent *)local)->setParam("gd_linesearch","armijo");
         y = myProblem->statFunmin(x);
        ((GradientDescent *)local)->setPoint(x,y);
        local->solve();
        ((GradientDescent *)local)->getPoint(x,y);

    }
    else
    if(localMethod == "bfgs")
    {
        local = new Bfgs();
        local->setProblem(myProblem);
        ((Bfgs *)local)->setPoint(x,y);
        local->solve();
        ((Bfgs *)local)->getPoint(x,y);
    }
    else
    if(localMethod == "lbfgs")
    {
        local = new Lbfgs();
        local->setProblem(myProblem);
        ((Lbfgs *)local)->setPoint(x,y);
        local->solve();
        ((Lbfgs *)local)->getPoint(x,y);
    }
    if(local!=NULL)
    {
        y = myProblem->statFunmin(x);
        delete local;
    }
    return y;
}

void    Optimizer::done()
{

}

Problem     *Optimizer::getProblem()
{
    return myProblem;
}

QStringList Optimizer::getParameterNames() const
{
    return params.keys();
}

Optimizer::~Optimizer()
{
    if(problemSampler!=NULL)
        delete problemSampler;
}
