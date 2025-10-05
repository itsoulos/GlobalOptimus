#ifndef USERSAMPLER_H
#define USERSAMPLER_H
#include "SAMPLER/problemsampler.h"

class UserSampler :public ProblemSampler
{
public:
    UserSampler(Problem *p);
    void    sampleFromProblem(int N,Matrix &xsample,Data &ysample);
    double  eval(Data &xpoint);
    void    addSampleFromProblem(Data &x,double y);
    ~UserSampler();
};

#endif // USERSAMPLER_H
