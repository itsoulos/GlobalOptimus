#include "sinu.h"
# include <math.h>
Sinu::Sinu()
    :Problem(1)
{

}

double  Sinu::funmin(Data &x)
{
    double p1=1.0,p2=1.0;
    const double z=M_PI/6;
    for(int i=0;i<dimension;i++)
    {
        p1*=sin(x[i]-z);
        p2*=sin(5*(x[i]-z));
    }
    return -2.5 * p1 -p2;
}
Data    Sinu::gradient(Data &x)
{
    Data g;
    g.resize(dimension);
    double p1=1.0,p2=1.0;
    const double z=M_PI/6;
    for(int i=0;i<dimension;i++)
    {
        p1=1.0,p2=1.0;
        for(int j=0;j<dimension;j++)
        {
            if(i==j) continue;
            p1*=sin(x[j]-z);
            p2*=sin(5*(x[j]-z));
        }
        g[i]=-2.5 * cos(x[i]-z)*p1-5.0*p2*cos(5*(x[i]-z));
    }
    return g;
}

void    Sinu::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    Data l, r;
    l.resize(n);
    r.resize(n);
    for (int i = 0; i < n; i++)
    {
        l[i] = 0.0;
        r[i] = M_PI;
    }
    setLeftMargin(l);
    setRightMargin(r);
}
