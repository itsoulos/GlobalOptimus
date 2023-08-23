#include "gradientdescent.h"
GradientDescent::GradientDescent()
{
    addParam(Parameter("gd_maxiters","200","Maximum iterations for gradient descent"));
    addParam(Parameter("gd_linesearch","none","Line search. Values: none, golden, fibonacci, armijo"));
    addParam(Parameter("gd_epsilon","1e-6","Eps value for termination"));
    addParam(Parameter("gd_rate","0.001","Step for gradient descent"));
}

void    GradientDescent::init()
{
    iteration = 0;
    maxiters = getParam("gd_maxiters").getValue().toInt();
    rate  = getParam("gd_rate").getValue().toDouble();
    eps  = getParam("gd_rate").getValue().toDouble();
    lineSearchMethod = getParam("gd_linesearch").getValue();
    xpoint = myProblem->getSample();
    ypoint = myProblem->statFunmin(xpoint);
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
    printf("GD. Iteration=%4d Optimum = %20.10lg\n",iteration,ypoint);
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

    }
    else
    if(lineSearchMethod == "fibonacci")
    {

    }
    else
    if(lineSearchMethod == "armijo")
    {

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

GradientDescent::~GradientDescent()
{
    if(lt!=NULL) delete lt;
}
