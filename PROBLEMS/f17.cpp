#include "f17.h"

f17::f17()
    : Problem(2)
{
    Data l, r;
    l.resize(2);
    r.resize(2);
    for (int i = 0; i < 2; i++)
    {
        l[i] = -2.0;
        r[i] = 2.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double f17::funmin( Data &x)
{
    double x1=x[0],x2=x[1];
    double t1 = 1 + pow(x1 + x2 + 1, 2) * (19 - 14*x1 + 3*pow(x1, 2) - 14*x2 + 6*x1*x2 + 3*pow(x2, 2));
    double t2 = 30 + pow(2*x1 - 3*x2, 2) * (18 - 32*x1 + 12*pow(x1, 2) + 48*x2 - 36*x1*x2 + 27*pow(x2, 2));


    return t1 * t2;

}

Data f17::gradient(Data &x)

{  double x1 = x[0], x2 = x[1];
    double y1,y2,y3,y4,t1,t2,gt1x1,gt2x1,gt1x2,gt2x2;
    Data g;
    g.resize(2);

    y1 = x1 + x2 + 1;
    y2 = 19 - 14 * x1 + 3 * pow(x1, 2) - 14 * x2 + 6 * x1 * x2 + 3 * pow(x2, 2);
    y3 = 2 * x1 - 3 * x2;
    y4 = 18 - 32 * x1 + 12 * pow(x1, 2) + 48 * x2 - 36 * x1 * x2 + 27 * pow(x2, 2);
    t1 = 1 + pow(y1, 2) * y2;
    t2 = 30 + pow(y3, 2) * y4;
    gt1x1 = 2 * y1 * y2 + pow(y1, 2) * (-14 + 6 * x1 + 6 * x2);
    gt2x1 = 2 * y3 * y4 * 2 + pow(y3, 2) * (-32 + 24 * x1 - 36 * x2);
    gt1x2 = 2 * y1 * y2 + pow(y1, 2) * (-14 + 6 * x1 + 6 * x2);
    gt2x2 = 2 * y3 * y4 * (-3) + pow(y3, 2) * (48 - 36 * x1 + 54 * x2);
    g[0]= t2 * gt1x1 + t1 * gt2x1;
    g[1]=t2 * gt1x2 + t1 * gt2x2;

    return g;
}
