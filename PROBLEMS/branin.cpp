#include "branin.h"

Branin::Branin()
    :Problem(2)
{
    Data l, r;
    l.resize(2);
    r.resize(2);
    l[0]=-5;
    l[1]=0;
    r[0]=10;
    r[1]=15;
    setLeftMargin(l);
    setRightMargin(r);
}

double  Branin::funmin(Data &x)
{

    double x1=x[0];
    double x2=x[1];
    return pow(x2-5.1/(4*M_PI*M_PI)*x1*x1+5.0/M_PI-6.0,2.0)+10.0*(1.0-1.0/(8.0*M_PI))*cos(x1)+10.0;

}

Data    Branin::gradient(Data &x)
{
    Data g;
    g.resize(2);
    double x1=x[0];
    double x2=x[1];
    g[0]=-2.0*x1*5.1/(4*M_PI*M_PI)*2.0*(x2-5.1/(4*M_PI*M_PI)*x1*x1+5.0/M_PI-6.0)
           -10.0*(1.0-1.0/(8.0*M_PI))*sin(x1);
    g[1]=2.0*(x2-5.1/(4*M_PI*M_PI)*x1*x1+5.0/M_PI-6.0);
    return g;
}
