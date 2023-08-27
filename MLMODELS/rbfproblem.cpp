#include "rbfproblem.h"

RbfProblem::RbfProblem()
    :Problem(1)
{

}

double  RbfProblem::gaussian(Data &x,Data &center,double variance)
{
    return exp(-pow(getDistance(x,center),2.0)/(variance * variance));
}

void    RbfProblem::setParameters(Data &x)
{
    int icount =0;
    int d = trainDataset->dimension();
    int nodes = weight.size();
    for(int i=0;i<nodes;i++)
    {
        for(int j=0;j<d;j++)
            centers[i][j]=x[icount++];

    }
    for(int i=0;i<nodes;i++)
        variances[i]=x[icount++];
    for(int i=0;i<nodes;i++)
        weight[i]=x[icount++];
}

void    RbfProblem::getParameters(Data &x)
{
    int icount =0;
    int d = trainDataset->dimension();
    int nodes = weight.size();
    for(int i=0;i<nodes;i++)
    {
        for(int j=0;j<d;j++)
            x[icount++]=centers[i][j];

    }
    for(int i=0;i<nodes;i++)
        x[icount++]=variances[i];
    for(int i=0;i<nodes;i++)
        x[icount++]=weight[i];
}

double  RbfProblem::funmin(Data &x)
{
    setParameters(x);
    return getTrainError();
}

static double dmax(double a,double b)
{
    return a>b?a:b;
}

Data    RbfProblem::gradient(Data &x)
{
    Data g;
    g.resize(dimension);
    for(int i=0;i<dimension;i++)
    {
        double eps=pow(1e-18,1.0/3.0)*dmax(1.0,fabs(x[i]));
        x[i]+=eps;
        double v1=funmin(x);
        x[i]-=2.0 *eps;
        double v2=funmin(x);
        g[i]=(v1-v2)/(2.0 * eps);
        x[i]+=eps;
    }
    return g;
}

double  RbfProblem::getOutput(Data &x)
{
    int nodes = weight.size();
    double sum = 0.0;
    for(int i=0;i<nodes;i++)
        sum+=weight[i]*gaussian(x,centers[i],variances[i]);
    return sum;
}

void    RbfProblem::init(QJsonObject &params)
{
    QString trainName = params["rbf_trainfile"].toString();
    QString testName =  params["rbf_testfile"].toString();
    int nodes        = 1;
    if(params.contains("rbf_nodes"))
        nodes = params["rbf_nodes"].toString().toInt();
    trainDataset=new Dataset(trainName);
    testDataset = new Dataset(testName);
    int k = (trainDataset->dimension()*nodes)+
            nodes+nodes;
    setDimension(k);
    left.resize(k);
    right.resize(k);
    //kmeans to estimate the range of margins
 }

QJsonObject RbfProblem::done(Data &x)
{
    double tr=funmin(x);
    QJsonObject xx;
    double tt=getTestError(testDataset);
    double tc=getClassTestError(testDataset);
    xx["trainError"]=tr;
    xx["testError"]=tt;
    xx["classError"]=tc;
    return xx;
}

RbfProblem::~RbfProblem()
{

}
