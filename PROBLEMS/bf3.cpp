#include "bf3.h"

bf3::bf3()
    :Problem(2)
{
    Data l, r;
    l.resize(2);
    r.resize(2);
    for (int i = 0; i < 2; i++)
    {
        l[i] = -50.0;
        r[i] =  50.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double bf3::funmin(Data &x)
{
    return x[0]*x[0]+2.0*x[1]*x[1]-0.3*cos(3.0*M_PI*x[0]+4.0*M_PI*x[1])+0.3;

}

Data    bf3::gradient(Data &x)
{
    double x1=x[0],x2=x[1];
    Data g;
    g.resize(2);
    g[0] = 2.0 * x1 + 0.9 * M_PI * sin(3.0 * M_PI * x1 + 4.0 * M_PI * x2);
    g[1] = 4.0 * x2 + 1.2 * M_PI * sin(3.0 * M_PI * x1 + 4.0 * M_PI * x2);
    return g;

}
