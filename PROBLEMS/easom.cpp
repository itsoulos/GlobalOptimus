#include "easom.h"

Easom::Easom()
    :Problem(2)
{
    Data l, r;
    l.resize(2);
    r.resize(2);
    for (int i = 0; i < 2; i++)
    {
        l[i] = -100.0;
        r[i] =  100.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double  Easom::funmin(Data &x)
{
    return -cos(x[0])*cos(x[1]*exp(-(pow(x[0]-M_PI,2.0)+pow(x[1]-M_PI,2.0))));

}

static double dmax(double a,double b)
{
    return a>b?a:b;
}
Data    Easom::gradient(Data &x)
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
