#include "f16.h"

f16::f16()
    : Problem(2)
{
    Data l, r;
    l.resize(2);
    r.resize(2);
    for (int i = 0; i < 2; i++)
    {
        l[i] = -5.0;
        r[i] = 5.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double f16::funmin( Data &x)
{
    double x1=x[0],x2=x[1];
    double t1 = 4 * pow(x1, 2);
    double t2 = -2.1 * pow(x1, 4);
    double t3 = (1.0 / 3) * pow(x1, 6);
    double t4 = x1 * x2;
    double t5 = -4 * pow(x2, 2);
    double t6 = 4 * pow(x2, 4);

    return t1 + t2 + t3 + t4 + t5 + t6;

}

Data f16::gradient(Data &x)

{
    Data g;
    g.resize(2);

    double x1=x[0],x2=x[1];
    g[0]=8*x1 - 8.4 * pow(x1, 3) + 2 * pow(x1, 5) + x2;
    g[1]=x1 - 8 * x2 + 16 * pow(x2, 3);
    return g;
}
