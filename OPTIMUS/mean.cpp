#include "mean.h"
# include <math.h>
Mean::Mean()
{
    minimumIters = 5;
    meanIterations=5;
}


void    Mean::init()
{
    //oldSum = 1e+100;
    meanCount=0;
    iteration=0;
}

void    Mean::setMinIters(int m)
{
    minimumIters = m;
}

void    Mean::setMeanIterations(int m)
{
    meanIterations=m;
}

bool    Mean::terminate(double value)
{
    iteration++;
    if(iteration==1)
    {
        oldSum = value;
        return false;
    }
    if(fabs(oldSum-value)<1e-6)
    {
        meanCount++;
    }
    else meanCount=0;
    oldSum =value;
    if(iteration<minimumIters) return false;
    if(meanCount>=meanIterations)
        return true;
    return false;
}

Mean::~Mean()
{

}
