#include "exp.h"

Exp::Exp()
{

}

double  Exp::funmin(Data &x)
{
    double s1=0.0;
    for(int i=0;i<dimension;i++) s1+=x[i]*x[i];
    return -exp(-0.5 * s1);
}

Data    Exp::gradient(Data &x)
{
    double s1=0.0;
    Data g;
    g.resize(dimension);
    for(int i=0;i<dimension;i++) s1+=x[i]*x[i];
    for(int i=0;i<dimension;i++)
    {
        g[i]=0.5 * 2.0 *x[i] *  exp(-0.5 * s1);
    }
    return g;
}

void    Exp::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    Data l, r;
    l.resize(n);
    r.resize(n);
    for (int i = 0; i < n; i++)
    {
        l[i] = -1.0;
        r[i] =  1.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}
