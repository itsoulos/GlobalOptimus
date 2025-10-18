#include "kmeanssampler.h"

KmeansSampler::KmeansSampler(Problem *p)
    :ProblemSampler("kmeans",p)
{

}
double  KmeansSampler::eval(Data &xpoint)
{
    return 0.0;
}

void    KmeansSampler::addSampleFromProblem(Data &x,double y)
{
    //nothing
}

static double getDistance(Data &x1,Data &x2)
{
    double sum=0.0;
    for(int i=0;i<x1.size();i++)
        sum+=(x1[i]-x2[i])*(x1[i]-x2[i]);
    return sqrt(sum);
}
vector<Data> KmeansSampler::runKmeans(int pop, int K)
{
    vector<Data> point;
    point.resize(pop);
    vector<Data> center;
    center.resize(K);
    vector<Data> copyCenter;
    copyCenter.resize(K);

    vector<int> belong;
    belong.resize(pop);
    vector<int> teamElements;
    teamElements.resize(K);
    for(int i=0;i<K;i++)
    {

        teamElements[i]=0;
    }

    for(int i=0;i<pop;i++)
    {
        point[i]=myProblem->getSample();
        belong[i]=rand() % K;
        teamElements[belong[i]]++;
    }

    for(int i=0;i<K;i++)
    {

        center[i].resize(myProblem->getDimension());
        for(int j=0;j<myProblem->getDimension();j++)
            center[i][j]=0.0;
    }
    for(int j=0;j<point.size();j++)
    {
        for(int k=0;k<myProblem->getDimension();k++)
        {
            center[belong[j]][k]+=point[j][k];
        }
    }
    for(int i=0;i<K;i++)
    {
        for(int j=0;j<myProblem->getDimension();j++)
            center[i][j]/=teamElements[i]>1?teamElements[i]:1;
    }


    int iteration = 1;
    double oldDist = 1e+100;
    while(true)
    {
        copyCenter = center;
        for(int i=0;i<K;i++) teamElements[i]=0;

        for(int i=0;i<point.size();i++)
        {
            int minCenterIndex = -1;
            double minCenterDist = 1e+100;
            for(int j=0;j<K;j++)
            {
                double d = getDistance(point[i],center[j]);
                if(d<minCenterDist)
                {
                    minCenterDist = d;
                    minCenterIndex = j;
                }
            }
            belong[i]=minCenterIndex;
            teamElements[minCenterIndex]++;

        }

        for(int i=0;i<K;i++)
        {
            for(int j=0;j<myProblem->getDimension();j++)
                center[i][j]=0.0;
        }
        for(int j=0;j<point.size();j++)
        {
            for(int k=0;k<myProblem->getDimension();k++)
            {
                center[belong[j]][k]+=point[j][k];
            }
        }

        for(int i=0;i<K;i++)
            for(int k=0;k<myProblem->getDimension();k++)
                center[i][k]/=teamElements[i]>1?teamElements[i]:1;

        double totalDistance = 0.0;
        for(int i=0;i<K;i++)
        {
            totalDistance+=getDistance(center[i],copyCenter[i]);
        }
        if(totalDistance<1e-6) break;
        if(iteration>1 && fabs(totalDistance-oldDist)<1e-8) break;
        iteration++;
        oldDist = totalDistance;
    }


    return center;
}

void    KmeansSampler::sampleFromProblem(int N,Matrix &xsample,Data &ysample)
{
    xpoint.resize(N);
    ypoint.resize(N);
    xsample = xpoint;
    ysample = ypoint;
    N=100;
    sampleFromModel(N,xsample,ysample);
}

void    KmeansSampler::sampleFromModel(int &N,Matrix &xsample,Data &ysample)
{
    xsample.clear();
    ysample.clear();
    vector<Data> center = runKmeans(xpoint.size(),N);
    //reduce near centers
    const double eps=1e-5;
    for(int i=0;i<center.size();i++)
    {
        bool found =false;
        for(int j=0;j<i;j++)
        {
            double d=getDistance(center[i],center[j]);
            if(d<eps)
            {
                found = true;
                break;
            }
        }
        if(!found) xsample.push_back(center[i]);
    }
    ysample.resize(xsample.size());
    for(int i=0;i<(int)xsample.size();i++)
    {
        ysample[i]=myProblem->statFunmin(xsample[i]);
    }
    N = xsample.size();
}

KmeansSampler::~KmeansSampler()
{
    //nothing here
}
