 #include "schwefel221.h"

schwefel221::schwefel221()
    : Problem(2)
{
    Data l, r;
    l.resize(dimension);
    r.resize(dimension);
    for (int i = 0; i < dimension; i++)
    {
        l[i] = -500;
        r[i] = 500;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double schwefel221::funmin( Data &x)
{ int n=x.size();
    double y ;
    double sum = 0.0;

    for (int i = 0; i < n; ++i) {
        sum += -x[i] * sin(sqrt(abs(x[i])));
    }

   y = 418.9829 * n + sum;

    return y;

}



static double dmax(double a,double b)
{
	return a>b?a:b;
}
Data schwefel221::gradient(Data &x)

{ int n=x.size();
    Data g;
    g.resize(n);
	for(int i=0;i<dimension;i++)
	{
		double eps=pow(1e-18,1.0/3.0)*dmax(1.0,fabs(x[i]));
		x[i]+=eps;
		double v1=funmin(x);
		x[i]-=2.0 *eps;
		double v2=funmin(x);
		g[i]=(v1-v2)/(2.0 * eps);
		x[i]+=eps;
	}
	return g;

}

