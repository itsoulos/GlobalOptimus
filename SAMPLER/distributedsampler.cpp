#include "distributedsampler.h"

DistributedSampler::DistributedSampler(Problem *p, int npartitions)
    :ProblemSampler("distsampler",p)
{
    partitions = npartitions;
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
        double delta = (i+1)*1.0/partitions;
        for(int k=i*N/partitions;k<(i+1)*N/partitions;k++)
        {
        xsample[k].resize(myProblem->getDimension());
        total_count++;
        for(int j=0;j<myProblem->getDimension();j++)
        {
            double width = (right[i]-left[j]);
            double mid = left[j]+width/2.0;
            double a = mid-delta * width/2;
            double b = mid+delta * width/2;

            xsample[k][j]=a+(b-a)*myProblem->randomDouble();
        }
        ysample[k]=myProblem->statFunmin(xsample[i]);
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

