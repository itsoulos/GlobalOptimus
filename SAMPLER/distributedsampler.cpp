#include "distributedsampler.h"
# include <MLMODELS/mlpproblem.h>
#include <iostream>
#include <typeinfo>
using namespace std;

template <typename Base, typename T>
bool instanceof(const T* ptr) {
    return dynamic_cast<const Base*>(ptr) != nullptr;
}
DistributedSampler::DistributedSampler(Problem *p, int npartitions)
    :ProblemSampler("distsampler",p)
{
    partitions = npartitions;
}

Data DistributedSampler::getSampleInPartition(int p)
{
    Data x;
    x.resize(myProblem->getDimension());
    Data left  = myProblem->getLeftMargin();
    Data right = myProblem->getRightMargin();
    double delta = (p+1)*1.0/partitions;

    for(int j=0;j<myProblem->getDimension();j++)
    {
        double width = (right[j]-left[j]);
        double mid = left[j]+width/2.0;
        double a = mid-delta * width/2;
        double b = mid+delta * width/2;
        x[j]=a+(b-a)*myProblem->randomDouble();
    }
    return x;
}
void            DistributedSampler::sampleFromProblem(int N,Matrix &xsample,Data &ysample)
{
    xsample.resize(N);
    ysample.resize(N);
    Data left  = myProblem->getLeftMargin();
    Data right = myProblem->getRightMargin();
    int total_count =0;
    for(int i=0;i<partitions;i++)
    {
        for(int k=i*N/partitions;k<(i+1)*N/partitions;k++)
        {
            xsample[k]=getSampleInPartition(i);
            ysample[k]=myProblem->statFunmin(xsample[k]);
            total_count++;
        }
    }
    if(total_count<N)
    {
        for(int i=total_count;i<N;i++)
        {
            xsample[i].resize(myProblem->getDimension());
            for(int j=0;j<myProblem->getDimension();j++)
            {
                double a = left[j];
                double b = right[j];
                xsample[i][j]=a+(b-a)*myProblem->randomDouble();
            }
            ysample[i]=myProblem->statFunmin(xsample[i]);
        }
    }
}
DistributedSampler::~DistributedSampler()
{
    //nothing here
}

