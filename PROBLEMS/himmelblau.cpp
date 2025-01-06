#include "himmelblau.h"

Himmelblau::Himmelblau()
    :Problem(1)
{
    Data l, r;
    l.resize(dimension);
    r.resize(dimension);
    for (int i = 0; i < dimension; i++)
    {
        l[i] = -6.0;
        r[i] =  6.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double Himmelblau::funmin(Data &x)
{
 double x1=x[0],x2=x[1];
    return 200 - pow((pow(x1, 2.0) + x2 - 11.0), 2.0)
           - pow((x1 + pow(x2, 2.0) - 7.0), 2.0);
}

static double dmax(double a,double b)
{
    return a>b?a:b;
}
Data Himmelblau::gradient(Data &x)

{ int n=x.size();
    Data g;
    g.resize(n);
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




