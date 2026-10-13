#include "rbfproblem.h"
#include <random>
using namespace std;

RbfProblem::RbfProblem()
    :Problem(1)
{
    addParam(Parameter("rbf_nodes",1,1,100,"Number of rbf nodes"));
    addParam(Parameter("rbf_factor",3.0,1.0,100.0,"Rbf Scale factor"));
    QStringList yesno;
    yesno<<"no"<<"yes";
    addParam(Parameter("rbf_originaltrain",yesno[0],yesno,"Enable or disable the                                                                                                              original RBF Training"));
    addParam(Parameter("rbf_weightbound",10.0,1.0,100000.0,"The margins for weig                                                                                                             hts"));
    trainA.resize(0);
}

double RbfProblem::apostasi(Data &x,Data &y)
{
    double s= 0.0;
    for(int i=0;i<(int)x.size();i++)
        s+=(x[i]-y[i])*(x[i]-y[i]);
    return s;
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
    // return hx * (-2.0/v)*(x[pos]-m[pos]); 
    return hx * (-(x[pos] - m[pos]) / (v*v));  // Πρώτη παράγωγος : Δφ / Δχ = φ(χ) . (-Xp - Mp / v*v)
}

double      RbfProblem::gaussianSecondDerivative(Data &x,Data &m,double v,int pos)
{
    double hx = gaussian(x,m,v);
    //double phixx = (-2.0/v)*(x[pos]-m[pos]);
    double difference = x[pos] - m[pos];
    double v2 = v*v;
    double v4 = v2*v2;
    //return hx *(phixx  * phixx +(-2.0/v));
    return hx * ((difference*difference) / v4 - (1.0 / v2)); // Δεύτερη παράγωγος : Δ2φ / Δ2χ = φ(χ). (((Xp-Mp)*(Xp-Mp) / v4) - 1.0 / v2)

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


double  RbfProblem::gaussian(double *x,Data &center,double variance)
{
    double d= 0.0;
    for(int i=0;i<center.size();i++)
            d+=pow(x[i]-center[i],2.0);
    d=sqrt(d);

      return exp(-d*d / (2.0* variance * variance));
}

double  RbfProblem::gaussian(Data &x,Data &center,double variance)
{
    double d= getDistance(x,center);
      return exp(-d*d / (2.0* variance * variance));
}

void    RbfProblem::setParameters(Data &x)
{
    int icount =0;
    int nodes        = getParam("rbf_nodes").getValue().toInt();
    int d = trainDataset==NULL?1:trainDataset->dimension();


    for(int i=0;i<nodes;i++)
    {
        for(int j=0;j<d;j++)
            centers[i][j]=x[icount++];

    }
    for(int i=0;i<nodes;i++)
        variances[i]=x[icount++];
    for(int i=0;i<nodes;i++)
    {
        weight[i]=x[icount++];
    }
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
    double val =  gaussian(x,centers[index],variances[index]);
    double v3 = variances[index];
    double d2 = getDistance(x,centers[index]);
    g = weight[index] * val * d2*d2 /( v3 * v3 * v3);
}

void    RbfProblem::getCenterDerivative(int index,Data &x,Data &g)
{
    double val =  gaussian(x,centers[index],variances[index]);
    int d = trainDataset->dimension();
    for(int j=0;j<d;j++)
    {
        g[j]=weight[index] * val * (x[j]-centers[index][j])*
                (1.0/(variances[index]*variances[index]));
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

#ifdef OPTIMUS_ARMADILLO

adept::adouble RbfProblem::aneuronOutput(vector<adept::adouble> &x,
                                          vector<double> &patt,
                                          unsigned pattDim,
                                          unsigned offset)
{
    const int nodes = (int)weight.size();
    adept::adouble dist2 = 0.0;
    for (unsigned j = 0; j < pattDim; ++j) {
        adept::adouble diff = patt[j] - x[offset * pattDim + j];
        dist2 += diff * diff;
    }
    adept::adouble sigma = x[nodes * pattDim + offset];
    return exp(-dist2 / (2.0 * sigma * sigma));
}

adept::adouble RbfProblem::afunmin(vector<adept::adouble> &x,
                                   vector<double> &x1)
{
    (void)x1; // Keep the existing header signature.
    const int nodes = (int)weight.size();
    Matrix trainx = trainDataset->getAllXpoint();
    Data trainy = trainDataset->getAllYPoints();
    adept::adouble errorSum = 0.0;
    for (unsigned p = 0; p < trainx.size(); ++p) {
        Data pattern = trainx[p];
        const unsigned d = (unsigned)pattern.size();
        adept::adouble output = 0.0;
        for (int j = 0; j < nodes; ++j) {
            adept::adouble phi = aneuronOutput(x, pattern, d, j);
            output += x[nodes * d + nodes + j] * phi;
        }
        adept::adouble error = output - trainy[p];
        errorSum += error * error;
    }
    return errorSum;
}
#endif

Data RbfProblem::gradient(Data &x)
{
    Data g(x.size(), 0.0);
    if (trainDataset == NULL)
        return g;

    setParameters(x);
    const int d = trainDataset->dimension();
    const int nodes = (int)weight.size();
    const int n = trainDataset->count();
    const int varianceOffset = nodes * d;
    const int weightOffset = varianceOffset + nodes;

    // Analytic gradient of sum_p (getOutput(x_p) - y_p)^2.
    for (int p = 0; p < n; ++p) {
        Data xx = trainDataset->getXPoint(p);
        double yy = trainDataset->getYPoint(p);
        Data phi(nodes, 0.0);
        Data dist2(nodes, 0.0);
        double output = 0.0;

        for (int i = 0; i < nodes; ++i) {
            double r2 = 0.0;
            for (int j = 0; j < d; ++j) {
                double diff = xx[j] - centers[i][j];
                r2 += diff * diff;
            }
            double v = variances[i];
            double v2 = v * v;
            dist2[i] = r2;
            phi[i] = exp(-r2 / (2.0 * v2));
            output += weight[i] * phi[i];
        }

        double factor = 2.0 * (output - yy);
        for (int i = 0; i < nodes; ++i) {
            double v = variances[i];
            double v2 = v * v;
            double common = factor * weight[i] * phi[i];
            for (int j = 0; j < d; ++j) {
                g[i * d + j] += common * (xx[j] - centers[i][j]) / v2;
            }
            g[varianceOffset + i] += common * dist2[i] / (v2 * v);
            g[weightOffset + i] += factor * phi[i];
        }
    }
    return g;
}

double  RbfProblem::getOutput(double *x)
{
    int nodes = weight.size();
    double sum = 0.0;
    for(int i=0;i<nodes;i++)
    {
        double val = gaussian(x,centers[i],variances[i]);
        sum+=weight[i]*val;
    }
    return sum;

}
double  RbfProblem::getOutput(Data &x)
{
    return getOutput(x.data());
}


void  RbfProblem::runKmeans(vector<Data> &point, int K,vector<Data> &centers,
                            Data &variances,
                            vector<int> &teamElements)
{
   int pop = point.size();
    centers.resize(K);
    vector<Data> copyCenter;
    copyCenter.resize(K);

    vector<int> belong;
    belong.resize(pop);
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
        belong[i]=   intDistrib(generator);
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
                variances[i]=0.01;//sqrt(variances[i]);
            else

        variances[i]=sqrt(variances[i]/teamElements[i]);

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
Data    RbfProblem::getSample()
{
    Data xx;
    xx.resize(dimension);
    for(int i=0;i<dimension;i++)
    {

        double a=-1;
        double b=1;

        a= left[i];
        b  = right[i];

        double d = (b-a);
        double mid = a+d/2;
        a = mid -0.2*d;
        b = mid +0.2*d;
        xx[i]=  (a+(b-a)*randomDouble());
    }
    return xx;
}

void    RbfProblem::initModel()
{
    int nodes        = getParam("rbf_nodes").getValue().toInt();

    QJsonObject xx;
    xx["model_trainfile"]=getParam("model_trainfile").getValue();
    xx["model_testfile"]=getParam("model_testfile").getValue();
    xx["rbf_nodes"]=getParam("rbf_nodes").getValue();
    init(xx);
    weight.resize(nodes);
    int d = trainDataset->dimension();
    int k = (d*nodes)+
            nodes+nodes;
    setDimension(k);
    left.resize(k);
    right.resize(k);

    centers.resize(nodes);
    for(int i=0;i<(int)centers.size();i++)
        centers[i].resize(d);
    variances.resize(nodes);

    //kmeans to estimate the range of margins
    vector<Data> xpoint = trainDataset->getAllXpoint();
    vector<int> teamElements;
    runKmeans(xpoint,nodes,centers,variances,teamElements);
    double scale_factor = 3.0;
    if(contains("rbf_factor"))
    {
        scale_factor = getParam("rbf_factor").getValue().toDouble();
    }
    int icount = 0;
    //centers bounds
    for(int i=0;i<nodes;i++)
    {
        for(int j=0;j<d;j++)
        {
            double dv = centers[i][j];
            if(fabs(dv)<0.01) dv=0.01;
            left[icount]= -scale_factor * fabs(dv);
            right[icount]= scale_factor * fabs(dv);
            icount++;
        }
    }
    double dmin=1e+100,dmax=0;

    //variances bounds
    for(int i=0;i<nodes;i++)
    {
        dmax = variances[i];
        if(dmax<0.01) dmax=0.01;
        left[icount]= 0.01;//-scale_factor *dmax;
        right[icount]= scale_factor * variances[i];
        if(right[icount]<left[icount])
        {
            double t = right[icount];
            right[icount]=left[icount];
            left[icount]=t;
        }

        icount++;
    }
    //weights bounds
    double rb = getParam("rbf_weightbound").getValue().toDouble();
    for(int i=0;i<nodes;i++)
    {
        left[icount]=  -scale_factor * rb;
        right[icount]= scale_factor *  rb;

        icount++;
    }

}
void    RbfProblem::init(QJsonObject &px)
{
    initParams(px);
    loadTrainSet();
    loadTestSet();
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
    double precision=0.0,recall=0.0,f1score=0.0;
    getPrecisionRecall(precision,recall,f1score);
    xx["precision"]=precision;
    xx["recall"]=recall;
    xx["f1score"]=f1score;
    return xx;
}


void    RbfProblem::trainModel()
{
    QString origtrain = getParam("rbf_originaltrain").getValue();
    if(origtrain=="yes")
    {
        originalTrain();
        return;
    }
    if(method!=NULL)
    {

        Model::trainModel();

    }
    else
    {
        originalTrain();
    }
}



void  RbfProblem::matrix_mult(Matrix &x,Matrix &y,Matrix &z)
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
        int i,j,k;

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
    }
}

void RbfProblem::matrix_inverse(Matrix &x,bool &error_flag,Matrix &c)
{

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
}

void  RbfProblem::matrix_pseudoinverse(Matrix &a,bool &error_flag)
{

    error_flag=false;
    matrix_transpose(a,trA);
    matrix_mult(trA,a,matrixE);

    matrix_inverse(matrixE,error_flag,matrixD);
    matrix_mult(matrixD,trA,matrixC);

}

void  RbfProblem::matrix_transpose(Matrix &x,Matrix &xx)
{


    int i,j;
    for(i=0;i<xx.size();i++)
    {

        for(j=0;j<x.size();j++)
        {
            xx[i][j]=x[j][i];
        }
    }
}

void    RbfProblem::originalTrain()
{
    //phase1
    int nodes        = getParam("rbf_nodes").getValue().toInt();
    vector<Data> xpoint = trainDataset->getAllXpoint();
    vector<int> teamElements;
    runKmeans(xpoint,nodes,centers,variances,teamElements);


    //phase2

    if(trainA.size()==0)
    {
        RealOutput.resize(trainDataset->count());
        trainA.resize(trainDataset->count());
        trA.resize(centers.size());
        for(int i=0;i<trainA.size();i++)
        {
            trainA[i].resize(centers.size());
            RealOutput[i].resize(1);
           }
        for(int i=0;i<trA.size();i++)
            trA[i].resize(trainDataset->count());
        matrixE.resize(centers.size());
        matrixC.resize(centers.size());
        matrixD.resize(centers.size());
        for(int i=0;i<matrixE.size();i++)
        {
            matrixE[i].resize(centers.size());
            matrixD[i].resize(centers.size());
            matrixC[i].resize(trainDataset->count());
        }

    }
    int i,j;
    for(i=0;i<trainA.size();i++)
    {

        RealOutput[i][0]=trainDataset->getYPoint(i);
        for(j=0;j<centers.size();j++)
        {
          trainA[i][j]=gaussian(xpoint[i],centers[j],variances[j]);
        }
    }
    matrix_pseudoinverse(trainA,error_flag);
    if(error_flag) return;

    Matrix pw;
    pw.resize(centers.size());
    for(int i=0;i<pw.size();i++)
        pw[i].resize(1);
    matrix_mult(matrixC,RealOutput,pw);
    for(i=0;i<pw.size();i++)
        weight[i]=pw[i][0];

}

RbfProblem::~RbfProblem()
{

}
