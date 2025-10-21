#ifndef DISTRIBUTEDSAMPLER_H
#define DISTRIBUTEDSAMPLER_H

#include "SAMPLER/problemsampler.h"

class DistributedSampler :public ProblemSampler
{
private:
    int partitions;
public:
    DistributedSampler(Problem *p, int npartitions);
    void            sampleFromProblem(int N,Matrix &xsample,Data &ysample);
    ~DistributedSampler();
    Data getSampleInPartition(int p);
};

#endif // DISTRIBUTEDSAMPLER_H
