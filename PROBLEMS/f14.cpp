#include "f14.h"
static   double a[2];
f14::f14()
    : Problem(2)
{
    Data l, r;
    l.resize(dimension);
    r.resize(dimension);
    for (int i = 0; i < dimension; i++)
    {
        l[i] = -65;
        r[i] =  65;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double f14::funmin( Data &x){
    double sum_j = 0.0;
    for (int j = 1; j <= 25; ++j) {
        sum_j += 1.0 / j;
    }

    double sum_i = 0.0;
    for (int i = 2; i <= 2; ++i) {
        sum_i += pow(x[i-1] - a[i-2], 6.0);
    }

    double y = 1.0 / (1.0 / 500.0 + sum_j + sum_i);
    return y;
}

Data f14::gradient(Data &x)

{
    Data g;
    g.resize(dimension);

    double sum_j = 0.0;
    for (int j = 1; j <= 25; ++j) {
        sum_j += 1.0 / j;
    }

    for (int i = 2; i <= 2; ++i) {
        double t = pow(x[i-1] - a[i-2], 6.0);
        g[i-1] = -6.0 * pow(x[i-1] - a[i-2], 5.0) / (1.0 / 500.0 + sum_j + t) / (1.0 / 500.0 + sum_j + t);
    }

    return g;
}


