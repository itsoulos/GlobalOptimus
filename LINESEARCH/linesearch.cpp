#include "linesearch.h"
LineSearch::LineSearch(Problem *p)
{
    myProblem = p;
}

double  LineSearch::fl(Data &x,double h)
{
    Data trialx = x;
    unsigned int i;
    Data g = myProblem->gradient(x);
    for(i=0;i<x.size();i++)
    {
        trialx[i]=trialx[i]-h * g[i];
    }
    return myProblem->statFunmin(trialx);
}

double  LineSearch::getDirection(Data &x)
{
    return 0.0;
}

LineSearch::~LineSearch()
{

}
