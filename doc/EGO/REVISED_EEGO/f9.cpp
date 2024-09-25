#include "f9.h"

f9::f9()
    : Problem(2)
{
    Data l, r;
    l.resize(dimension);
    r.resize(dimension);
    for (int i = 0; i < dimension; i++)
    {
        l[i] = 0.0;
        r[i] =  1.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double f9::funmin( Data &x){
    int n=x.size();
    double sum = 0.0;
    for (int k = 1; k <= n; k++) {
        sum += (10.0 + 9.0 * cos(2 * M_PI * k * x[k]));
    }

    return -sum;
}


static double dmax(double a,double b)
{
    return a>b?a:b;
}
Data f9::gradient(Data &x)

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
