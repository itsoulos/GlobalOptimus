#include "discus.h"
# include <stdio.h>
Discus::Discus()
    :Problem(1)
{

}

double  Discus::funmin(Data &x)
{
	double sum=pow(10.0,6.0)*x[0]*x[0];
	for(int i=1;i<mydimension;i++)
		sum+=x[i]*x[i];
	return sum;
}

Data Discus::gradient(Data &x)
{
    Data g;
    g.resize(x.size());
    g[0]=2.0 * x[0]*pow(10.0,6.0);
	for(int i=1;i<mydimension;i++)
		g[i]=2.0 * x[i];
    return g;
}

void    Discus::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    mydimension = n;
    setDimension(n);
    Data l, r;
    l.resize(n);
    r.resize(n);
    for (int i = 0; i < n; i++)
    {
        l[i] = -1e+10;
        r[i] =  1e+10;
    }
    setLeftMargin(l);
    setRightMargin(r);

}

QJsonObject Discus::done(Data &x)
{
    QJsonObject t;
    return t;
}

Discus::~Discus()
{

}
