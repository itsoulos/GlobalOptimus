#ifndef KMEANSSAMPLER_H
#define KMEANSSAMPLER_H

#include "SAMPLER/problemsampler.h"

class KmeansSampler : public ProblemSampler
{
private:
    Matrix xpoint;
    Data   ypoint;
public:
    KmeansSampler(Problem *p);
    void            sampleFromProblem(int N,Matrix &xsample,Data &ysample);
    void            sampleFromModel(int &N,Matrix &xsample,Data &ysample);
    double          eval(Data &xpoint);
    void            addSampleFromProblem(Data &x,double y);
    vector<Data>    runKmeans(int pop, int K);
    ~KmeansSampler();
};

#endif // KMEANSSAMPLER_H
