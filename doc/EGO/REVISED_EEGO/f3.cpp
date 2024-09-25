#include "f3.h"

F3::F3()
    : Problem(1)
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

double F3::funmin( Data &x){
    double x1=x[0];
    double part1 = exp(-2.0 * log(2.0) * pow((x1 - 0.08) / 0.854, 2.0));
    double part2 = pow(sin(5.0 * M_PI * (pow(x1, 3.0/4.0) - 0.05)), 6.0);


    return part1 * part2;
}



static double dmax(double a,double b)
{
    return a>b?a:b;
}
Data F3::gradient(Data &x)

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
