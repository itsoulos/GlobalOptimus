#include "test30n.h"

Test30n::Test30n()
    :Problem(1)
{

}

double  Test30n::funmin(Data &x)
{
    double sum=0.0;
    for(int i=1;i<dimension-1;i++)
        sum+=pow(x[i]-1.0,2.0)*(1.0+pow(sin(3*M_PI*x[i+1]),2.0));
    return 0.1 *(pow(sin(3*M_PI*x[0]),2.0))*sum+pow(x[dimension-1]-1.0,2.0)*(1.0+pow(sin(2.0*M_PI*x[dimension-1]),2.0));

}

Data    Test30n::gradient(Data &x)
{
    Data g;
    g.resize(dimension);
    for(int i=0;i<dimension;i++) g[i]=0.0;
    double sum=0.0;
    for(int i=1;i<dimension-1;i++)
        sum+=pow(x[i]-1.0,2.0)*(1.0+pow(sin(3*M_PI*x[i+1]),2.0));
    g[0]=0.1 * sum * 2 * sin(3.0 * M_PI *x[0])* 3 * M_PI *cos(3.0 * M_PI *x[0]);
    for(int i=1;i<dimension-1;i++)
    {
        double m=0.1 * pow(sin(3*M_PI*x[0]),2.0);
        g[i]+=2.0*(x[i]-1.0)*(1.0+pow(sin(3*M_PI*x[i+1]),2.0))*m;
        g[i+1]+=pow(x[i]-1.0,2.0)*2.0*3*M_PI*sin(3*M_PI*x[i+1])*cos(3*M_PI*x[i+1])*m;
    }
    g[dimension-1]+=2.0*(x[dimension-1]-1.0)*(1.0+pow(sin(2.0*M_PI*x[dimension-1]),2.0))+
                        pow(x[dimension-1]-1.0,2.0)*2.0*2.0*M_PI*sin(2.0*M_PI*x[dimension-1])*
                            cos(2.0*M_PI*x[dimension-1]);
    return g;
}

void    Test30n::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    Data l, r;
    l.resize(n);
    r.resize(n);
    for (int i = 0; i < n; i++)
    {
        l[i] = -10.0;
        r[i] =  10.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}
