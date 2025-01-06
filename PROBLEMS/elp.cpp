#include "elp.h"

Elp::Elp()
    :Problem(1)
{

}

double  Elp::funmin(Data &x)
{
    double sum1=0.0;
    for(int i=0;i<dimension;i++)
        sum1+=pow(pow(10.0,6.0),i*1.0/(dimension-1.0))*x[i]*x[i];
    return sum1;
}

Data    Elp::gradient(Data &x)
{
    Data g;
    g.resize(dimension);
    for(int i=0;i<dimension;i++)
        g[i]=pow(pow(10.0,6.0),i*1.0/(dimension-1.0))*2.0*x[i];
    return g;
}

void    Elp::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    Data l, r;
    l.resize(n);
    r.resize(n);
    for (int i = 0; i < n; i++)
    {
        l[i] = -dimension;
        r[i] =  dimension;
    }
    setLeftMargin(l);
    setRightMargin(r);
}
