#include "rbfproblem.h"
#include <iostream>
#include <random>
using namespace std;

RbfProblem::RbfProblem()
    :Problem(1)
{
    addParam(Parameter("rbf_nodes","1","Number of rbf nodes"));
    addParam(Parameter("rbf_factor","3.0","Rbf Scale factor"));
}
double  RbfProblem::getDerivative(Data &x,int pos)
{

    double sum = 0.0;
    for(int i=0;i<weight.size();i++)
    {
        double px = gaussianDerivative(x,centers[i],variances[i],pos);
        sum = sum + weight[i]*px;
    }
    return sum;
}
double      RbfProblem::gaussianDerivative(Data &x,Data &m,double v,int pos)
{
    double hx = gaussian(x,m,v);
    return hx * (-2.0/v)*(x[pos]-m[pos]);
}

double      RbfProblem::gaussianSecondDerivative(Data &x,Data &m,double v,int pos)
{


    double hx = gaussian(x,m,v);
    double phixx = (-2.0/v)*(x[pos]-m[pos]);
    return hx *(phixx  * phixx +(-2.0/v));
}
double  RbfProblem::getSecondDerivative(Data &x,int pos)
{

    double sum = 0.0;
    for(int i=0;i<weight.size();i++)
    {
        double px = gaussianSecondDerivative(x,centers[i],variances[i],pos);
        sum = sum + weight[i]*px;
    }
    return sum;
}

double  RbfProblem::gaussian(Data &x,Data &center,double variance)
{
    double arg = getDistance(x,center);
    arg = (arg * arg)/(variance * variance);
  //  if(isnan(arg) || isinf(arg)) return 0.0;
 //   if(arg>10) return 0.0;
    return exp(-arg);
}

void    RbfProblem::setParameters(Data &x)
{
    int icount =0;
    int nodes        = getParam("rbf_nodes").getValue().toInt();
    int d = trainDataset==NULL?1:trainDataset->dimension();

    weight.resize(nodes);
    centers.resize(nodes);
    for(int i=0;i<centers.size();i++)
        centers[i].resize(d);
    variances.resize(nodes);
    lastGaussianValues.resize(nodes);
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
    double val =lastGaussianValues[index];// gaussian(x,centers[index],variances[index]);
    g=val;
}

void    RbfProblem::getVarianceDerivative(int index,Data &x,double &g)
{
    double val = lastGaussianValues[index];// gaussian(x,centers[index],variances[index]);
    double v3 = variances[index]*variances[index]*variances[index];
    double d2 = getDistance(x,centers[index]);
    d2 = d2 * d2;
    g =weight[index]* val * (-2.0) * (-d2/v3);
}

void    RbfProblem::getCenterDerivative(int index,Data &x,Data &g)
{
    double val = lastGaussianValues[index];// gaussian(x,centers[index],variances[index]);
    int d = trainDataset->dimension();
    for(int j=0;j<d;j++)
    {
        g[j]=(-1.0)*val * 2.0 * (x[j]-centers[index][j])*(1.0/(variances[index]*variances[index]));
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
    int tcount = trainDataset->count();
    for(int i=0;i<tcount;i++)
    {
        Data xx = trainDataset->getXPoint(i);
        double per=getOutput(xx)-trainDataset->getYPoint(i);
        int icount = 0;
        for(int j=0;j<nodes;j++)
        {
            getCenterDerivative(j,xx,g1);
            for(int l=0;l<d;l++)
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
        for(int j=0;j<dimension;j++)	g[j]+=gtemp[j]*per;
    }
    for(int j=0;j<x.size();j++) g[j]*=2.0;

    return g;

    Data g2;
    g2.resize(dimension);
    for(int i=0;i<dimension;i++)
    {
        double eps=pow(1e-18,1.0/3.0)*dmax(1.0,fabs(x[i]));
        x[i]+=eps;
        double v1=funmin(x);
        x[i]-=2.0 *eps;
        double v2=funmin(x);
        g2[i]=(v1-v2)/(2.0 * eps);
        if(fabs(g[i]-g2[i])>1e-3)
        {
            g[i]=g2[i];/*
            printf("DIFFERENCE[%4d]=%.8lf\n",i,fab(g[i]-g2[i]));
            if(i<nodes*d)
                printf("Center difference \n");
            else
                if(i>=nodes *d && i<nodes*d+nodes)
                printf("Varince difference \n");
            else
                printf("Weight difference\n");*/
        }
        x[i]+=eps;
    }
    return g;
}

double  RbfProblem::getOutput(Data &x)
{
    int nodes = weight.size();
    double sum = 0.0;
    if(error_flag) return 1e+100;
    for(int i=0;i<nodes;i++)
    {
        double val = gaussian(x,centers[i],variances[i]);
        lastGaussianValues[i]=val;
        sum+=weight[i]*val;
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

    using param_t = std::uniform_int_distribution<>::param_type;
    intDistrib.param(param_t(0, K-1));

 for(int i=0;i<K;i++)
 {

     teamElements[i]=0;
 }


    for(int i=0;i<pop;i++)
    {
        belong[i]=intDistrib(generator);
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
    const int maxiters=100;
    double oldDist = 1e+100;
    while(iteration<=maxiters)
    {
        copyCenter = centers;
        for(int i=0;i<K;i++) teamElements[i]=0;

        for(int i=0;i<point.size();i++)
        {
            int minCenterIndex = 0;
            double minCenterDist = getDistance(point[i],centers[0]);
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
            if(variances[i]<1e-6 || isnan(variances[i]) || isinf(variances[i]))
                variances[i]=0.0001;
    }

    double var_diag = 0.0;
    for(int i=0;i<variances.size();i++)
    {
        var_diag+=variances[i];
    }
    if(var_diag<0.00000001) var_diag=0.001;
    for(int i=0;i<variances.size();i++)
        variances[i]=var_diag;


}

void    RbfProblem::initModel()
{
    int nodes        = getParam("rbf_nodes").getValue().toInt();

    weight.resize(nodes);
    int d = trainDataset->dimension();
    int k = (d*nodes)+
            nodes+nodes;
    setDimension(k);
    left.resize(k);
    right.resize(k);
    lastGaussianValues.resize(nodes);

    //kmeans to estimate the range of margins
    vector<Data> xpoint = trainDataset->getAllXpoint();
    runKmeans(xpoint,nodes,centers,variances);
    double scale_factor = 3.0;
    if(contains("rbf_factor"))
    {
        scale_factor = getParam("rbf_factor").getValue().toDouble();
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
        left[icount]= 0.01;//-scale_factor *dmax;
        right[icount]= scale_factor * dmax;
        if(right[icount]<left[icount])
        {
            double t = right[icount];
            right[icount]=left[icount];
            left[icount]=t;
        }
        if(right[icount]<0.001) right[icount]=0.001;
        icount++;
    }
    for(int i=0;i<nodes;i++)
    {
        left[icount]=  -scale_factor * 10.0;
        right[icount]= scale_factor * 10.0;
        icount++;
    }

}
void    RbfProblem::init(QJsonObject &px)
{
    QString trainName = px["model_trainfile"].toString();
    QString testName =  px["model_testfile"].toString();
    setParam("model_trainfile",trainName);
    setParam("model_testfile",testName);

    loadTrainSet();
    loadTestSet();
    initModel();
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


void    RbfProblem::trainModel()
{
    if(method!=NULL)
    {

        Model::trainModel();

    }
    else
    {
        originalTrain();
    }
}



Matrix  RbfProblem::matrix_mult(Matrix &x,Matrix &y)
{
    int m=x.size();
    int p=x[0].size();
    int n=y[0].size();
    if(p!=y.size())
    {
        printf("Impossible to multiple \n");

    }
    else
    {
        Matrix z;
        z.resize(m);
        int i,j,k;
        for(i=0;i<m;i++) z[i].resize(n);
        for(i=0;i<m;i++)
        {
            for(j=0;j<n;j++)
            {
                z[i][j]=0.0;
                for(k=0;k<p;k++)
                {
                    z[i][j]=z[i][j]+x[i][k]*y[k][j];
                }
            }
        }
        return z;
    }
}

Matrix  RbfProblem::matrix_inverse(Matrix x,bool &error_flag)
{
    Matrix c=x;
    error_flag =false;
    int npivot;
    double det;
    int pass, row, col, maxrow, i, j;
    double temp, pivot, mult;
    int n=x.size();
     for(i=0; i<n; ++i) {
        for(j=0; j<n; ++j) {
            if(i==j) {
            c[i][j] = 1.0;
            } else {
            c[i][j] = 0.0;
              }
        }
       }

       det=1.0;
       npivot=0;


       for(pass=0; pass<n; ++pass) {
        maxrow=pass;
        for(row=pass; row<n; ++row)
            if(fabs(x[row][pass]) > fabs(x[maxrow][pass]))
            maxrow=row;

        if(maxrow != pass)
            ++npivot;

        for(col=0; col<n; ++col) {
            temp=x[pass][col];
            x[pass][col] = x[maxrow][col];
            x[maxrow][col] = temp;
            temp = c[pass][col];
            c[pass][col] = c[maxrow][col];
            c[maxrow][col] = temp;
        }


        pivot = x[pass][pass];
        det *= pivot;

        if(fabs(det) < 1.0e-40) {
            error_flag =true;
            return c;
        }


        for(col=0; col<n; ++col) {
            x[pass][col] = x[pass][col]/pivot;
            c[pass][col] = c[pass][col]/pivot;
        }

        for(row=0; row<n; ++row) {
            if(row != pass) {
                mult = x[row][pass];
                for(col=0; col<n; ++col) {
                    x[row][col] = x[row][col] - x[pass][col] * mult;
                    c[row][col] = c[row][col] - c[pass][col] * mult;
                }
            }
        }

       }

       if(npivot % 2 != 0)
        det = det * (-1.0);
      return c;
}

Matrix  RbfProblem::matrix_pseudoinverse(Matrix &a,bool &error_flag)
{
    error_flag=false;
    Matrix b=matrix_transpose(a);
    Matrix e=matrix_mult(b,a);
    Matrix d=matrix_inverse(e,error_flag);
    Matrix c=matrix_mult(d,b);
    return c;
}

Matrix  RbfProblem::matrix_transpose(Matrix &x)
{
    Matrix xx;
    xx.resize(x[0].size());
    int i,j;
    for(i=0;i<xx.size();i++)
    {
        xx[i].resize(x.size());
        for(j=0;j<x.size();j++)
        {
            xx[i][j]=x[j][i];
        }
    }
    return xx;
}

void    RbfProblem::originalTrain()
{
    //phase1
    int nodes        = getParam("rbf_nodes").getValue().toInt();
    vector<Data> xpoint = trainDataset->getAllXpoint();
    runKmeans(xpoint,nodes,centers,variances);


    //phase2
    Matrix A,RealOutput;
    A.resize(trainDataset->count());
    RealOutput.resize(trainDataset->count());
    int i,j;
    for(i=0;i<A.size();i++)
    {
        /**/
        RealOutput[i].push_back(trainDataset->getYPoint(i));
        Data x=trainDataset->getXPoint(i);
        A[i].resize(centers.size());

        for(j=0;j<centers.size();j++)
        {
          A[i][j]=gaussian(x,centers[j],variances[j]);

        }

    }
    Matrix pA=matrix_pseudoinverse(A,error_flag);
    if(error_flag) return;
    Matrix pW=matrix_mult(pA,RealOutput);
    for(i=0;i<pW.size();i++)
        weight[i]=pW[i][0];
}

RbfProblem::~RbfProblem()
{

}
