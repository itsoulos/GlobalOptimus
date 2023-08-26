#include "bf2.h"

Bf2::Bf2()
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

double Bf2::funmin(Data &x)
{
    return x[0]*x[0]+2.0*x[1]*x[1]-0.3*cos(3.0*M_PI*x[0])*cos(4.0*M_PI*x[1])+0.3;

}

Data    Bf2::gradient(Data &x)
{
    Data g;
    g.resize(2);
    g[0]=2.0*x[0]+0.3*3.0*M_PI*sin(3.0*M_PI*x[0])*cos(4.0*M_PI*x[1]);
    g[1]=4.0*x[1]+0.3*4.0*M_PI*sin(4.0*M_PI*x[1])*cos(3.0*M_PI*x[0]);
    return g;
}
