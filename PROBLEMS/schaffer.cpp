#include "schaffer.h"


schaffer::schaffer()
    : Problem(2)
{
    Data l, r;
    l.resize(2);
    r.resize(2);
    for (int i = 0; i < 2; i++)
    {
        l[i] = -100;
        r[i] = 100;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double schaffer::funmin(Data &x)
{
    double x1 = x[0];
    double x2 = x[1];
    double n = pow(sin(x1 * x1 - x2 * x2), 2) - 0.5;
    double d = pow(1 + 0.001 * (x1 * x1 + x2 * x2), 2);
    return 0.5 + n / d;
}


Data schaffer::gradient(Data &x)
{  
    Data g;
    g.resize(2);
    double x1 = x[0];
    double x2 = x[1];

    double t1 = 1 + 0.001 * (x1 * x1 + x2 * x2);
    double sin_t = sin(x1 * x1 - x2 * x2);
    double cos_t = cos(x1 * x1 - x2 * x2);

    double gx1 = (2 * x1 * cos_t * sin_t * t1 -
                     0.002 * x1 * (sin_t * sin_t - 0.5)) / (t1 * t1 * t1);

    double gx2 = (-2 * x2 * cos_t * sin_t * t1 - 0.002 * x2 * (sin_t * sin_t - 0.5)) / (t1 * t1 * t1);

    g[0] = gx1;
    g[1] = gx2;

    return g;
}
