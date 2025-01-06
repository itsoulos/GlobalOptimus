#include "similarity.h"
# include <math.h>
Similarity::Similarity()
{
    minimumIters = 5;
    similarityIterations=5;
}


void    Similarity::init()
{
    oldBest = 1e+100;
    similarityCount=0;
    iteration=0;
}

void    Similarity::setMinIters(int m)
{
    minimumIters = m;
}

void    Similarity::setSimilarityIterations(int m)
{
    similarityIterations=m;
}

bool    Similarity::terminate(double value)
{
    iteration++;
    if(iteration==1)
    {
        oldBest = value;
        return false;
    }
    if(fabs(oldBest-value)<1e-6)
    {
        similarityCount++;
    }
    else similarityCount=0;
    oldBest =value;
    if(iteration<minimumIters) return false;
    if(similarityCount>=similarityIterations)
        return true;
    return false;
}

Similarity::~Similarity()
{

}
