#include "rbfproblem.h"

RbfProblem::RbfProblem()
    :Problem(1)
{

}

double  RbfProblem::gaussian(Data &x,Data &center,double variance)
{
    double arg = getDistance(x,center);
    arg = pow(arg,2.0)/(variance * variance);
   //printf("Arg = %lf \n",arg);
   // if(isnan(arg) || isinf(arg)) return 0.0;
   // if(arg>10) return 0.0;
    return exp(-arg);
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

void    RbfProblem::getWeightDerivative(int index,Data &x,double &g)
{
    double val = gaussian(x,centers[index],variances[index]);
    g=val;
}

void    RbfProblem::getVarianceDerivative(int index,Data &x,double &g)
{
    double val = gaussian(x,centers[index],variances[index]);
    g =weight[index]* val * (-2.0) * (-pow(getDistance(x,centers[index]),2.0)/pow(variances[index],3.0));
}

void    RbfProblem::getCenterDerivative(int index,Data &x,Data &g)
{
    double val = gaussian(x,centers[index],variances[index]);
    for(int j=0;j<trainDataset->dimension();j++)
    {
        g[j]=val * 2.0 * (x[j]-centers[index][j])*(-1.0)*(-1.0/(variances[index]*variances[index]));
    }
}

double  RbfProblem::funmin(Data &x)
{
    setParameters(x);
    double f =  getTrainError();
    return f;
}

static double dmax(double a,double b)
{
    return a>b?a:b;
}

Data    RbfProblem::gradient(Data &x)
{
    Data g;
    setParameters(x);
    g.resize(x.size());
   for(int i=0;i<g.size();i++)
        g[i]=0.0;
    Data gtemp ;
    gtemp.resize(dimension);
    Data g1;
    int d = trainDataset->dimension();
    int nodes = weight.size();
    g1.resize(d );
    for(int i=0;i<trainDataset->count();i++)
    {
        Data xx = trainDataset->getXPoint(i);
        double per=getOutput(xx)-trainDataset->getYPoint(i);
        int icount = 0;
        for(int j=0;j<nodes;j++)
        {
            getCenterDerivative(j,xx,g1);
            for(int l=0;l<g1.size();l++)
                gtemp[icount++]=g1[l];
        }
        for(int j=0;j<nodes;j++)
        {
            double gx;
            getVarianceDerivative(j,xx,gx);
            gtemp[icount++]=gx;
        }
        for(int j=0;j<nodes;j++)
        {
            double gx;
            getWeightDerivative(j,xx,gx);
            gtemp[icount++]=gx;
        }
        for(int j=0;j<g.size();j++)	g[j]+=gtemp[j]*per;
    }
    for(int j=0;j<x.size();j++) g[j]*=2.0;
    return g;

    //Data g;
    //g.resize(dimension);
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
    {
        sum+=weight[i]*gaussian(x,centers[i],variances[i]);
    }
    return sum;
}


void  RbfProblem::runKmeans(vector<Data> &point, int K,vector<Data> &centers,
                            Data &variances)
{
   int pop = point.size();
    centers.resize(K);
    vector<Data> copyCenter;
    copyCenter.resize(K);

    vector<int> belong;
    belong.resize(pop);
    vector<int> teamElements;
    teamElements.resize(K);
    variances.resize(K);

 for(int i=0;i<K;i++)
 {

     teamElements[i]=0;
 }

    for(int i=0;i<pop;i++)
    {
        belong[i]=rand() % K;
        teamElements[belong[i]]++;
    }
    int d = trainDataset->dimension();
    for(int i=0;i<K;i++)
    {

        centers[i].resize(d);
        for(int j=0;j<d;j++)
            centers[i][j]=0.0;
    }
    for(int j=0;j<point.size();j++)
        {
         for(int k=0;k<d;k++)
         {
             centers[belong[j]][k]+=point[j][k];
         }
     }
    for(int i=0;i<K;i++)
    {
        for(int j=0;j<d;j++)
            centers[i][j]/=teamElements[i]>1?teamElements[i]:1;
    }

    int iteration = 1;
    double oldDist = 1e+100;
    while(true)
    {
        copyCenter = centers;
        for(int i=0;i<K;i++) teamElements[i]=0;

        for(int i=0;i<point.size();i++)
        {
            int minCenterIndex = -1;
            double minCenterDist = 1e+100;
            for(int j=0;j<K;j++)
            {
                double dist = getDistance(point[i],centers[j]);
                if(dist<minCenterDist)
                {
                    minCenterDist = dist;
                    minCenterIndex = j;
                }
            }
            belong[i]=minCenterIndex;
            teamElements[minCenterIndex]++;

        }

        for(int i=0;i<K;i++)
        {
           for(int j=0;j<d;j++)
               centers[i][j]=0.0;
        }
           for(int j=0;j<point.size();j++)
           {
            for(int k=0;k<d;k++)
            {
                centers[belong[j]][k]+=point[j][k];
            }
           }

           for(int i=0;i<K;i++)
            for(int k=0;k<d;k++)
               centers[i][k]/=teamElements[i]>1?teamElements[i]:1;

        double totalDistance = 0.0;
        for(int i=0;i<K;i++)
        {
            totalDistance+=getDistance(centers[i],copyCenter[i]);
        }
        if(totalDistance<1e-6) break;
        if(iteration>1 && fabs(totalDistance-oldDist)<1e-8) break;
        iteration++;
        oldDist = totalDistance;
    }
    for(int i=0;i<variances.size();i++)
    {
        variances[i]=0.0;
    }

    Data dvar;
    dvar.resize(d);
    for(int i=0;i<K;i++)
    {
        for(int j=0;j<d;j++)
            dvar[j]=0.0;
        for(int j=0;j<belong.size();j++)
        {
            if(belong[j]==i)
            {
               for(int l=0;l<d;l++)
                    dvar[l]+=pow(point[j][l]-centers[i][l],2.0);
            }
        }
        for(int j=0;j<d;j++)
            variances[i]+=dvar[j]/teamElements[i];
    }
    for(int i=0;i<variances.size();i++)
        {
            if(teamElements[i]==0)
                variances[i]=sqrt(variances[i]);
            else

        variances[i]=sqrt(variances[i]/teamElements[i]);
    }
}


void    RbfProblem::init(QJsonObject &params)
{
    QString trainName = params["rbf_trainfile"].toString();
    QString testName =  params["rbf_testfile"].toString();
    int nodes        = 1;
    if(params.contains("rbf_nodes"))
        nodes = params["rbf_nodes"].toString().toInt();
    weight.resize(nodes);
    trainDataset=new Dataset(trainName);
    int d = trainDataset->dimension();
    testDataset = new Dataset(testName);
    int k = (d*nodes)+
            nodes+nodes;
    setDimension(k);
    left.resize(k);
    right.resize(k);

    //kmeans to estimate the range of margins
    vector<Data> xpoint = trainDataset->getAllXpoint();
    runKmeans(xpoint,nodes,centers,variances);
    double scale_factor = 5.0;
    if(params.contains("rbf_factor"))
    {
        scale_factor = params["rbf_factor"].toString().toDouble();
    }
    int icount = 0;
    for(int i=0;i<nodes;i++)
    {
        for(int j=0;j<d;j++)
        {
            left[icount]= -scale_factor * fabs(centers[i][j]);
            right[icount]= scale_factor * fabs(centers[i][j]);
            icount++;
        }
    }
    double dmin=1e+100,dmax=0;
    for(int i=0;i<nodes;i++)
    {
        if(isnan(variances[i]) || isinf(variances[i]))
            variances[i]=0.001;
            dmax += variances[i];
    }

    for(int i=0;i<nodes;i++)
    {
        left[icount]= -scale_factor *dmax;
        right[icount]= scale_factor * dmax;
        if(right[icount]<0.001) right[icount]=0.001;
        icount++;
    }
    for(int i=0;i<nodes;i++)
    {
        left[icount]=-scale_factor * 10.0;
        right[icount] = scale_factor * 10.0;
        icount++;
    }
 }

QJsonObject RbfProblem::done(Data &x)
{
    setParameters(x);
    double tr=getTrainError();
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
