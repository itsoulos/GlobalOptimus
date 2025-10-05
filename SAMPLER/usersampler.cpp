#include "usersampler.h"

UserSampler::UserSampler(Problem *p)
    :ProblemSampler("UserSampler",p)
{
    //code here
}

void    UserSampler::sampleFromProblem(int N,Matrix &xsample,Data &ysample)
{

    //code here
    int i;
    xsample.resize(N);
    ysample.resize(N);
    for(i=0;i<N;i++)
    {
        xsample[i].resize(myProblem->getDimension());
        xsample[i]=myProblem->getSample();
        ysample[i]=myProblem->statFunmin(xsample[i]);
    }
}

double UserSampler::eval(Data &xpoint)
{
    //code here
    return 0.0;
}

void    UserSampler::addSampleFromProblem(Data &x,double y)
{
    //code here
}

UserSampler::~UserSampler()
{
    //code here
}

