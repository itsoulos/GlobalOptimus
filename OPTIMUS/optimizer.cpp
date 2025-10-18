#include "optimizer.h"
# include <METHODS/gradientdescent.h>
# include <METHODS/bfgs.h>
# include <METHODS/lbfgs.h>
# include <METHODS/neldermead.h>
# include <METHODS/adam.h>
Optimizer::Optimizer()
{
    problemSampler=NULL;
    myProblem = NULL;
    QStringList opt_debug;opt_debug<<"yes"<<"no";
    QStringList opt_localsearch;
    opt_localsearch<<"bfgs"<<"lbfgs"<<"nelderMead"<<"gradient"<<"adam"<<"none";
    QStringList opt_sampler;
    opt_sampler<<"uniform"<<"triangular"<<"maxwell"<<"mlp"
                <<"rbf"<<"kmeans"<<"user";

    addParam(Parameter("opt_debug",opt_debug[0],
                       opt_debug,
                       "Set it to yes to show messages"));

    addParam(Parameter("opt_localsearch",opt_localsearch[0],
                       opt_localsearch,
                       "The method used in localsearch"));


    addParam(Parameter("opt_doubleboxminiters",5,1,20,"Minimum iters to terminate the doublebox"));
    addParam(Parameter("opt_similarityminiters",5,1,20,"Minimum number of iters for similarity stopping rule"));
    addParam(Parameter("opt_similaritycriterion",5,1,20,"Number of iterations for same value in the similarity stopping rule"));
    addParam(Parameter("opt_meanminiters",5,1,20,"Minimum number of iters for mean stopping rule"));
    addParam(Parameter("opt_meancriterion",5,1,20,"Number of iterations for same value in the mean stopping rule"));
    addParam(Parameter("opt_sampler",opt_sampler[0],opt_sampler,
                       "Sampling method. Available values: uniform, triangular, maxwell, mlp, rbf, kmeans"));
    addParam(Parameter("opt_mlpsamples",10,10,100,"Number of MLP samples"));
    addParam(Parameter("opt_mlpweights",10,10,100,"Number of MLP weights"));
    addParam(Parameter("opt_rbfsamples",10,10,100,"Number of RBF samples"));
    addParam(Parameter("opt_rbfweights",10,1,20,"Number of RBF weights"));
    addParam(Parameter("opt_kmeansamples",1000,100,10000,"Number of samples for k-means sampling"));

    QStringList termMethod;
    termMethod<<"doublebox"<<"similarity"<<"maxiters"<<"sumfitness";
    addParam(Parameter("opt_termination",termMethod[0],termMethod,
            "Termination methods: doublebox,similarity,maxiters"));
    methodLogger = new Logger();
}

void    Optimizer::setMethodLogger(Logger *p)
{
    if(methodLogger!=NULL)
        delete methodLogger;
    methodLogger = p;
}

QJsonObject Optimizer::getParams()
{
    return paramList.getParams();
}

void        Optimizer::setParams(QJsonObject &x)
{
    paramList.setParamValuesFromJson(x);
}

void        Optimizer::setProblem(Problem *p)
{
    myProblem = p;

}

void        Optimizer::addParam(Parameter p)
{
    paramList.addParam(p);
}

void        Optimizer::setParam(QString name,QString value,QString help)
{
    paramList.setParam(name,value,help);
}

Parameter   Optimizer::getParam(QString name)
{
    return paramList.getParam(name);

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

QString     Optimizer::getTerminationMethod() const
{
    return terminationMethod;
}

Logger  *Optimizer::getMethodLogger()
{
    return methodLogger;
}

void    Optimizer::showDebug()
{

}
void    Optimizer::solve()
{

    terminationMethod = getParam("opt_termination").getValue();
    doubleBox.setMinIters(getParam("opt_doubleboxminiters").getValue().toInt());
    similarity.setMinIters(getParam("opt_similarityminiters").getValue().toInt());
    similarity.setSimilarityIterations(getParam("opt_similaritycriterion").getValue().toInt());
    mean.setMinIters(getParam("opt_meanminiters").getValue().toInt());
    mean.setMeanIterations(getParam("opt_meancriterion").getValue().toInt());
    doubleBox.init();
    similarity.init();
    mean.init();
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
    else
    if(sampling == "rbf")
    {
        int w = getParam("opt_rbfweights").getValue().toInt();
        problemSampler=new RbfSampler(myProblem,w);
    }
    else
    if(sampling=="kmeans")
    {
        problemSampler=new KmeansSampler(myProblem);
    }
    else
    if(sampling=="user")
    {
        problemSampler = new UserSampler(myProblem);
    }
    else
    {
        problemSampler=new UniformSampler(myProblem);

    }
    init();
    do
    {
        step();
        if(getParam("opt_debug").getValue()=="yes")
            showDebug();
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
    if(sampling=="kmeans")
    {
        int M = getParam("opt_kmeansamples").getValue().toInt();
	printf("KMEANS M= %d \n",M);
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
    if(localMethod=="none" || localMethod=="")
    {
        return myProblem->funmin(x);
    }
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
    else
    if(localMethod == "nelderMead")
    {
        local = new NelderMead();
        local->setProblem(myProblem);
        ((NelderMead *)local)->setPoint(x,y);
        local->solve();
        ((NelderMead *)local)->getPoint(x,y);
    }
    else
    if(localMethod=="adam")
    {
        local = new Adam();
        local->setProblem(myProblem);
        ((Adam *)local)->setPoint(x,y);
        local->solve();
        ((Adam *)local)->getPoint(x,y);

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
    //nothing here is a virtual function
}

Problem     *Optimizer::getProblem()
{
    return myProblem;
}

QStringList Optimizer::getParameterNames() const
{
    return paramList.getParameterNames();
}

ParameterList Optimizer::getParameterList()
{
    return paramList;
}

Optimizer::~Optimizer()
{
    if(problemSampler!=NULL)
        delete problemSampler;
    if(methodLogger!=NULL)
        delete methodLogger;
}
