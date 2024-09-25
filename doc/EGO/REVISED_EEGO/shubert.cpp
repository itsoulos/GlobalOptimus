#include "shubert.h"

shubert::shubert()
    : Problem(3)
{
    Data l, r;
    l.resize(dimension);
    r.resize(dimension);
    for (int i = 0; i < dimension; i++)
    {
        l[i] = -10.0;
        r[i] =  10.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double shubert::funmin( Data &x){
    double x1=x[0];
        double x2=x[1];
    double sum1 = 0, sum2 = 0;

    for (int j = 1; j <= 5; j++) {
        sum1 += j * cos((j + 1) * x1 + j);
        sum2 += j * cos((j + 1) * x2 + j);
    }

    return sum1 * sum2;
}





static double dmax(double a,double b)
{
    return a>b?a:b;
}
Data shubert::gradient(Data &x)

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
