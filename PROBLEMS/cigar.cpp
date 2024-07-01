#include "cigar.h"
# include <stdio.h>
Cigar::Cigar()
    :Problem(1)
{

}

double  Cigar::funmin(Data &x)
{
	double sum=x[0]*x[0];
	for(int i=1;i<mydimension;i++)
		sum+=pow(10.0,6.0) * x[i]*x[i];
	return sum;
}

Data Cigar::gradient(Data &x)
{
    Data g;
    g.resize(x.size());
    g[0]=2.0 * x[0];
	for(int i=1;i<mydimension;i++)
		g[i]=pow(10.0,6.0)* 2.0 * x[i];
    return g;
}

void    Cigar::init(QJsonObject &params)
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

QJsonObject Cigar::done(Data &x)
{
    QJsonObject t;
    return t;
}

Cigar::~Cigar()
{

}
