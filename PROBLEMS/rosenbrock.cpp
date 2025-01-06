#include "rosenbrock.h"

Rosenbrock::Rosenbrock()
    :Problem(1)
{

}

double  Rosenbrock::funmin(Data &x)
{
    double sum=0.0;
    for(int i=0;i<dimension-1;i++)
    {
        sum=sum+100.0*pow(x[i]*x[i]-x[i+1],2.0)+pow(x[i],2.0);
    }
    return sum;
}

Data    Rosenbrock::gradient(Data &x)
{
    Data g;
    g.resize(getDimension());
    for(int i=0;i<getdimension();i++)
    {
        g[i]=0.0;
    }
    for(int i=0;i<getdimension()-1;i++)
    {
        g[i]+=100.0*4.0*x[i]*(x[i]*x[i]-x[i+1])+2.0*x[i];
        g[i+1]+=-100.0*2.0*(x[i]*x[i]-x[i+1]);
    }
    return g;
}

void    Rosenbrock::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    Data l, r;
    l.resize(n);
    r.resize(n);
    for (int i = 0; i < n; i++)
    {
        l[i] = -30.0;
        r[i] =  30.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}
