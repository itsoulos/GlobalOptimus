#include "griewank10.h"

Griewank10::Griewank10()
    :Problem(10)
{

}

double  Griewank10::funmin(Data &x)
{
 int dimension=x.size();
    double sum=0.0;
    double product=1.0;
    for(int i=0;i<dimension;i++)
    {
       sum+=x[i]*x[i]/4000.0;
       product*=cos(x[i]/sqrt(i+1.0));
    }
    return sum+1.0-product;
}

Data    Griewank10::gradient(Data &x)
{

    Data g;
    g.resize(dimension);
   for(int i=0;i<dimension;i++)
   {
       double product=1.0;
       for(int j=0;j<dimension;j++)
       {
          if(i==j) continue;
          product*=cos(x[j]/sqrt(j+1.0));
       }
       g[i]=x[i]/2000.0+sin(x[i]/sqrt(i+1.0))*product/sqrt(i+1.0);
   }
   return g;

}
void Griewank10::init(QJsonObject &params) {
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    Data l, r;
    l.resize(n);
    r.resize(n);

    for (int i = 0; i < n; i++) {
        l[i] = -dimension;
        r[i] = dimension;
    }

    setLeftMargin(l);
    setRightMargin(r);
}
