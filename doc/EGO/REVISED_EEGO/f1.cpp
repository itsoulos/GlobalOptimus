#include "f1.h"

F1::F1()
    : Problem(1)
{
    Data l, r;
    l.resize(dimension);
    r.resize(dimension);
    for (int i = 0; i < dimension; i++)
    {
        l[i] = 0.0;
        r[i] =  30.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double F1::funmin( Data &x){
    double x1=x[0];
    if (x1 >= 0 && x1 < 2.5) {
        return 80 * (2.5 - x1);
    } else if (x1 >= 2.5 && x1 < 5.0) {
        return 64 * (x1 - 2.5);
    } else if (x1 >= 5.0 && x1 < 7.5) {
        return 64 * (7.5 - x1);
    } else if (x1 >= 7.5 && x1 < 12.5) {
        return 28 * (x1 - 7.5);
    } else if (x1 >= 12.5 && x1 < 17.5) {
        return 28 * (17.5 - x1);
    } else if (x1 >= 17.5 && x1 < 22.5) {
        return 32 * (x1 - 17.5);
    } else if (x1>= 22.5 && x1 < 27.5) {
        return 32 * (27.5 - x1);
    } else if (x1 >= 27.5 && x1 <= 30) {
        return 80 * (x1 - 27.5);
    } else {
        return -1e6;
    }
}



static double dmax(double a,double b)
{
    return a>b?a:b;
}
Data F1::gradient(Data &x)

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
