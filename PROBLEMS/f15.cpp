#include "f15.h"
static double a[11];
static double b[11];
static double b1;
f15::f15()
    : Problem(2)
{
    Data l, r;
    l.resize(dimension);
    r.resize(dimension);
    for (int i = 0; i < dimension; i++)
    {
        l[i] = -5.0;
        r[i] =  5.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double f15::funmin( Data &x){
    double sum = 0.0;
    for (int i = 1; i <= 11; ++i) {
        double n = x[0] * (b1 * b1 + b[i-1] * x[1]);
        double d = b1 * b1 + b[i-1] * x[2] + x[3];
        double t = a[i-1] - n / d;
        sum += t * t;
    }
    return sum;
}

Data f15::gradient(Data &x)

{
    Data g;
    g.resize(dimension);

    for (int i = 1; i <= dimension; ++i) {
        double n = x[0] * (b1 * b1 + b[i-1] * x[1]);
        double d = b1 * b1 + b[i-1] * x[2] + x[3];
        double t= a[i-1] - n / d;
        double ds = d * d;

        g[0] += -2 * t * (b1 * b1 + b[i-1] * x[1]) / d;
        g[1] += 2 * t * n / ds;
    }
    return g;
}

