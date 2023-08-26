#include "diffpower.h"

DiffPower::DiffPower()
    :Problem(1)
{

}

double  DiffPower::funmin(Data &x)
{
    double sum=0.0;
    for(int i=0;i<dimension;i++)
        sum+=pow(fabs(x[i]),(i+2.0));
    return sum;
}

static double dmax(double a,double b)
{
    return a>b?a:b;
}
Data    DiffPower::gradient(Data &x)
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

void    DiffPower::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    Data l, r;
    l.resize(n);
    r.resize(n);
    for (int i = 0; i < n; i++)
    {
        l[i] = -1.0;
        r[i] = 1.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}
