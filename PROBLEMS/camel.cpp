#include "camel.h"

Camel::Camel()
    :Problem(2)
{
    Data l, r;
    l.resize(2);
    r.resize(2);
    for (int i = 0; i < 2; i++)
    {
        l[i] = -5.0;
        r[i] =  5.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double  Camel::funmin(Data &x)
{
    double x1=x[0],x2=x[1];
    return 4*x1*x1-2.1*x1*x1*x1*x1+x1*x1*x1*x1*x1*x1/3.0+x1*x2-4*x2*x2+4*x2*x2*x2*x2;
}

Data Camel::gradient(Data &x)
{
    Data g;
    g.resize(2);
    double x1=x[0],x2=x[1];
    g[0]=8*x1-8.4*x1*x1*x1+2*x1*x1*x1*x1*x1+x2;
    g[1]=x1-8*x2+16*x2*x2*x2;
    return g;
}
