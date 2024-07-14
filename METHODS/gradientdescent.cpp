#include "gradientdescent.h"
GradientDescent::GradientDescent()
{
    hasInitialized  = false;
    addParam(Parameter("gd_maxiters",200,1,10000,"Maximum iterations for gradient descent"));
    QStringList gd_linesearch;
    gd_linesearch<<"none"<<"golden"<<"fibonacci"<<"armijo";
    addParam(Parameter("gd_linesearch",gd_linesearch[0],
                       gd_linesearch,
                       "Line search. Values: none, golden, fibonacci, armijo"));
    addParam(Parameter("gd_epsilon",0.000001,0.0,1.0,"Eps value for termination"));
    addParam(Parameter("gd_rate",0.001,0.0,1.0,"Step for gradient descent"));
}

 void    GradientDescent::setPoint(Data &x,double &y)
 {
     xpoint = x;
     ypoint = y;
     hasInitialized = true;
 }
void    GradientDescent::init()
{
    iteration = 0;
    maxiters = getParam("gd_maxiters").getValue().toInt();
    rate  = getParam("gd_rate").getValue().toDouble();
    eps  = getParam("gd_rate").getValue().toDouble();
    lineSearchMethod = getParam("gd_linesearch").getValue();
    if(!hasInitialized)
    {
        xpoint = myProblem->getSample();
        ypoint = myProblem->statFunmin(xpoint);
    }
    lt = NULL;
}

void    GradientDescent::step()
{
    iteration++;
    updaterate();
    updatepoint();
}

void    GradientDescent::showDebug()
{
    if(getParam("opt_debug").getValue()=="yes")
        methodLogger->printMessage(QString::asprintf("GD. Iteration=%4d Optimum = %20.10lg",iteration,ypoint));
}
bool    GradientDescent::terminated()
{
    if(iteration>=maxiters) return true;
    if(myProblem->grms(xpoint)<eps) return true;
    return false;
}

void    GradientDescent::updaterate()
{
    if(lineSearchMethod == "golden")
    {
        double lambda = rate;
        double a = lambda * 0.9;
        double b = lambda * 1.1;
        if(lt==NULL)
            lt = new GoldenSearch(myProblem);
        ((GoldenSearch *)lt)->setParams(a,b);

        rate = lt->getDirection(xpoint);
    }
    else
    if(lineSearchMethod == "fibonacci")
    {
        double lambda = rate;
        double a = lambda * 0.9;
        double b = lambda * 1.1;
        if(lt==NULL)
            lt = new FibonacciSearch(myProblem);
        ((FibonacciSearch *)lt)->setParams(a,b);
        rate = lt->getDirection(xpoint);
    }
    else
    if(lineSearchMethod == "armijo")
    {
        double lambda  = 2;
        if(lt == NULL)
            lt = new ArmijoSearch(myProblem);
        ((ArmijoSearch*)lt)->setLambda(lambda);

        rate = lt->getDirection(xpoint);

    }
}

void    GradientDescent::updatepoint()
{
    Data g = myProblem->gradient(xpoint);
    unsigned int i;
    for(i=0;i<g.size();i++)
        xpoint[i]=xpoint[i]-rate * g[i];
    ypoint=myProblem->statFunmin(xpoint);
}

void    GradientDescent::getPoint(Data &x,double &y)
{
    x = xpoint;
    y = ypoint;
}

GradientDescent::~GradientDescent()
{
    if(lt!=NULL) delete lt;
}
