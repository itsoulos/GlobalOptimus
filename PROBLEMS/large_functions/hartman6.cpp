#include "hartman6.h"
static double a[4][6]={{10,3,17,3.5,1.7,8},
                         {0.05,10,17,0.1,8,14},
                         {3,3.5,1.7,10,17,8},
                         {17,8,0.05,10,0.10,14}};
static double c[4]={1,1.2,3,3.2};
static double p[4][6]={{0.1312,0.1696,0.5569,0.0124,0.8283,0.5886},
                         {0.2329,0.4135,0.8307,0.3736,0.1004,0.9991},
                         {0.2348,0.1451,0.3522,0.2883,0.3047,0.6650},
                         {0.4047,0.8828,0.8732,0.5743,0.1091,0.0381}};
Hartman6::Hartman6()
    :Problem(6)
{

}


double  Hartman6::funmin(Data &x)
{
    double sum=0.0;
    for(int i=0;i<4;i++)
    {
        double s=0.0;
        for(int j=0;j<dimension;j++) s=s+a[i][j]*pow(x[j]-p[i][j],2.0);
        sum=sum+c[i]*exp(-s);
    }
    return -sum;
}
void Hartman6::init(QJsonObject &params) {
    int n = params["opt_dimension"].toInt();
    setDimension(n);
    Data l, r;
    l.resize(n);
    r.resize(n);

    for (int i = 0; i < n; i++) {
        l[i] = -dimension;
        r[i] = dimension;
    }

    setLeftMargin(l);
    setRightMargin(r);
}


Data    Hartman6::gradient(Data &x)
{
    Data g;
    g.resize(dimension);
    for(int i=0;i<dimension;i++) g[i]=0.0;
    for(int j=0;j<dimension;j++)
    {
        double pj=0.0;
        for(int i=0;i<4;i++)
        {
            double sum1=0.0;
            for(int k=0;k<dimension;k++)
                sum1+=a[i][k]*pow(x[k]-p[i][k],2.0);
            pj+=c[i]*exp(-sum1)*(-2.0)*a[i][j]*(x[j]-p[i][j]);
        }
        g[j]=-pj;
    }
    return g;
}
