#include "griewank10.h"

Griewank10::Griewank10()
    :Problem(10)
{

    Data l, r;
    l.resize(dimension);
    r.resize(dimension);
    for (int i = 0; i < dimension; i++)
    {
        l[i] = -600.0;
        r[i] =  600.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double  Griewank10::funmin(Data &x)
{

    int n=10;
    double sum=0.0;
    double product=1.0;
    for(int i=0;i<n;i++)
    {
       sum+=x[i]*x[i]/4000.0;
       product*=cos(x[i]/sqrt(i+1.0));
    }
    return sum+1.0-product;
}

Data    Griewank10::gradient(Data &x)
{
    int n=10;
    Data g;
    g.resize(n);
   for(int i=0;i<n;i++)
   {
       double product=1.0;
       for(int j=0;j<n;j++)
       {
          if(i==j) continue;
          product*=cos(x[j]/sqrt(j+1.0));
       }
       g[i]=x[i]/2000.0+sin(x[i]/sqrt(i+1.0))*product/sqrt(i+1.0);
   }
   return g;

}
