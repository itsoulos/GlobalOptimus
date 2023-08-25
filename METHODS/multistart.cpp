#include "multistart.h"
Multistart::Multistart()
{
    addParam(Parameter("ms_samples","100","Number of samples"));
    addParam(Parameter("ms_maxiters","100","Maximum number of iterations"));
}

void    Multistart::init()
{
    minima.clear();
    iteration = 0;
    maxiters = getParam("ms_maxiters").getValue().toInt();
    nsamples = getParam("ms_samples").getValue().toInt();
}

void    Multistart::step()
{
    ++iteration;
    double y;
    for(int i=0;i<nsamples;i++)
    {
        Data x = myProblem->getSample();
        y = myProblem->statFunmin(x);
        y = localSearch(x);
        minima.addPoint(x,y);
    }
}

bool    Multistart::terminated()
{
    double besty,worsty;
    minima.getBestWorstValues(besty,worsty);

    if(doubleBox.terminate(besty)) return true;
    if(iteration>=maxiters) return true;
    return false;
}

void    Multistart::showDebug()
{
    double besty,worsty;
    minima.getBestWorstValues(besty,worsty);
    printf("MULTISTART. ITERATION=%4d BEST VALUE=%20.10lg\n",
           iteration,besty);
}

void    Multistart::done()
{
    minima.sort();
    minima.getPoint(0,bestx,besty);
}

void    Multistart::getBest(Data &x,double &y)
{
    x = bestx;
    y = besty;
}

Multistart::~Multistart()
{
    //nothing here
}
