#include "cm.h"

Cm::Cm()
    :Problem(1)
{

}


void    Cm::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    Data l, r;
    l.resize(n);
    r.resize(n);
    for (int i = 0; i < n; i++)
    {
        l[i] = -1.0;
        r[i] = 1.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double  Cm::funmin(Data &x)
{
    double s1=0.0,s2=0.0;
    for(int i=0;i<dimension;i++)
    {
        s1=s1+cos(5.0 * M_PI * x[i]);
        s2=s2+x[i]*x[i];
    }
    return s2-0.1 * s1;
}

Data    Cm::gradient(Data &x)
{
    Data g;
    g.resize(dimension);
    for(int i=0;i<dimension;i++)
    {
        g[i]=2.0 * x[i]+0.1 * 5.0 * M_PI * sin(5.0 *M_PI *x[i]);
    }
    return g;
}
