#include "shekel7.h"
static double a[7][4]={{4,4,4,4},{1,1,1,1},{8,8,8,8},{3,7,3,7},{2,9,2,9},{5,5,3,3}};
static double c[7]={0.1,0.2,0.2,0.4,0.4,0.6,0.3};

Shekel7::Shekel7()
    :Problem(4)
{
    Data l, r;
    l.resize(dimension);
    r.resize(dimension);
    for (int i = 0; i < dimension; i++)
    {
        l[i] =  0.0;
        r[i] =  10.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double  Shekel7::funmin(Data &x)
{
    double sum=0.0;
    for(int i=0;i<7;i++)
    {
        double s=0.0;
        for(int j=0;j<4;j++) s=s+pow(x[j]-a[i][j],2.0);
        sum=sum+1.0/(s+c[i]);
    }
    return -sum;
}

Data    Shekel7::gradient(Data &x)
{    
    Data g;
    g.resize(dimension);
    for(int j=0;j<4;j++)
    {
        double s=0.0;
        for(int  i=0;i<7;i++)
        {
            double ss=0.0;
            for(int k=0;k<4;k++) ss+=pow(x[k]-a[i][k],2.0);
            ss+=c[i];
            s=s+pow(ss,-2.0)*2.0*(x[j]-a[i][j]);
        }
        g[j]=s;
    }
    return g;
}
