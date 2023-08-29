#include "multistart.h"
Multistart::Multistart()
{
    addParam(Parameter("ms_samples","100","Number of samples"));
    addParam(Parameter("ms_maxiters","100","Maximum number of iterations"));
    addParam(Parameter("ms_termination","doublebox","Termination rule. Values: maxiters,doublebox,similarity"));
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
    Matrix xpoint;
    Data ypoint;
    sampleFromProblem(nsamples,xpoint,ypoint);
    for(int i=0;i<nsamples;i++)
    {
        Data x = xpoint[i];
        y = ypoint[i];
        y = localSearch(x);
        minima.addPoint(x,y);
    }
}

bool    Multistart::terminated()
{
    double besty,worsty;
    minima.getBestWorstValues(besty,worsty);
    QString termination = getParam("ms_termination").getValue();
    if(termination == "doublebox" && doubleBox.terminate(besty)) return true;
    if(termination == "similarity" && similarity.terminate(besty)) return true;
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
