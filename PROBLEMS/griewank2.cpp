#include "griewank2.h"

Griewank2::Griewank2()
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

double  Griewank2::funmin(Data &x)
{
    int n=2;
       double sum=0.0;
       double product=1.0;
       for(int i=0;i<n;i++)
       {
         sum+=x[i]*x[i]/200.0;
         product*=cos(x[i]/sqrt(i+1.0));
       }
       return sum+1.0-product;
}

Data    Griewank2::gradient(Data &x)
{
    Data g;
    g.resize(dimension);
    int n=2;
        double a,b;
        for(int i=0;i<n;i++)
        {
           double product=1.0;
           for(int j=0;j<n;j++)
           {
             product*=cos(x[j]/sqrt(j+1.0));
            }
            a=sin(x[i]/sqrt(i+1.0));
            b=sqrt(i+1.0)*cos(x[i]/sqrt(i+1.0));
            g[i]=x[i]/100.0+a*product/b;
        }
    return g;
}
