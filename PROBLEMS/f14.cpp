#include "f14.h"
#include <cmath>


static double a[2][25] = {{-32,-16,0,16,32,-32,-16,0,16,32,-32,-16,0,16,32,-32,-16,0,16,32,-32,-16,0,16,32}, {-32,-32,-32,-32,-32,-16,-16,-16,-16,-16,0,0,0,0,0,16,16,16,16,16,32,32,32,32,32}
};

f14::f14()
    : Problem(2)
{

}

double f14::funmin(Data &x) {
    int dimension=x.size();
	double sumj=0;
	for(int j=0;j<25;j++)
	{
		double sum2=0.0;
        for(int i=0;i<dimension;i++)
			sum2+=pow(x[i]-a[i][j],6.0);
		sumj+=1.0/(j+1.0 +sum2);
	}
	return pow(1.0/500.0 + sumj,-1.0);
}

static double dmax(double a,double b)
{
	return a>b?a:b;
}
Data f14::gradient(Data &x)

{
    Data g;
    g.resize(dimension);
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

void f14::init(QJsonObject &params) {
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    Data l, r;
    l.resize(n);
    r.resize(n);

    for (int i = 0; i < n; i++) {
        l[i] = -dimension;
        r[i] = dimension;
    }

    setLeftMargin(l);
    setRightMargin(r);
}
