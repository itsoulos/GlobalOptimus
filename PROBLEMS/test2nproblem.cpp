#include "test2nproblem.h"

Test2nProblem::Test2nProblem()
    :Problem(1)
{
}

void Test2nProblem::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    Data l, r;
    l.resize(n);
    r.resize(n);
    for (int i = 0; i < n; i++)
    {
        l[i] = -5.0;
        r[i] = 5.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double Test2nProblem::funmin(Data &x)
{
    double sum=0.0;
    for(int i=0;i<dimension;i++)
      sum+=0.5 * (pow(x[i],4.0)-16.0 * pow(x[i],2.0)+5.0*x[i]);
   return sum;
}

Data Test2nProblem::gradient(Data &x)
{
    Data g;
    g.resize(dimension);
    for(int i=0;i<dimension;i++)
        {
            g[i]=0.5 * (4.0*pow(x[i],3.0)-32*x[i]+5.0);
        }
    return g;
}
