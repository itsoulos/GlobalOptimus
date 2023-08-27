#include "hansen.h"

Hansen::Hansen()
    :Problem(2)
{
    Data l, r;
    l.resize(2);
    r.resize(2);
    for (int i = 0; i < 2; i++)
    {
        l[i] = -10.0;
        r[i] =  10.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double  Hansen::funmin(Data &x)
{
    double sum1=0.0;
    double sum2=0.0;
    for(int i=1;i<=5;i++)
    {
        sum1+=i*cos((i-1)*x[0]+i);
        sum2+=i*cos((i+1)*x[1]+i);
    }
    return sum1 * sum2;
}

Data    Hansen::gradient(Data &x)
{
    Data g;
    g.resize(2);
    double sum1=0.0;
    double sum2=0.0;
    for(int i=1;i<=5;i++)
    {
        sum1+=-i*(i-1)*sin((i-1)*x[0]+i);
        sum2+=i*cos((i+1)*x[1]+i);
    }
    g[0]=sum1 * sum2;
    sum1=sum2=0;
    for(int i=1;i<=5;i++)
    {
        sum1+=i*cos((i-1)*x[0]+i);
        sum2+=-i*(i+1)*sin((i+1)*x[1]+i);
    }
    g[1]=sum1*sum2;
    return g;
}
