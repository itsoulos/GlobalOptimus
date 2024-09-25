#include "f5.h"

F5::F5()
    : Problem(2)
{
    Data l, r;
    l.resize(dimension);
    r.resize(dimension);
    for (int i = 0; i < dimension; i++)
    {
        l[i] = -5.0;
        r[i] =  5.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double F5::funmin( Data &x){
    double x1=x[0];
    double x2=x[1];
    double part1 = (4.0 - 2.1 * x1 * x1 + (x1 * x1 * x1 * x1) / 3.0) * x1 * x1;
    double part2 = x1 * x2;
    double part3 = (4.0 * x2 * x2 - 4.0) * x2 * x2;

    return -4.0 * (part1 + part2 + part3);
}



static double dmax(double a,double b)
{
    return a>b?a:b;
}
Data F5::gradient(Data &x)

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
