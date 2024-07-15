#include "f19.h"

static double a[4][6] = {
    {1.0, 2.0, 3.0, 4.0, 5.0, 6.0},
    {0.5, 1.5, 2.5, 3.5, 4.5, 5.5},
    {2.0, 3.0, 4.0, 5.0, 6.0, 7.0},
    {1.5, 2.5, 3.5, 4.5, 5.5, 6.5}
};

static double p[4][6] = {
    {0.1, 0.2, 0.3, 0.4, 0.5, 0.6},
    {0.2, 0.3, 0.4, 0.5, 0.6, 0.7},
    {0.3, 0.4, 0.5, 0.6, 0.7, 0.8},
    {0.4, 0.5, 0.6, 0.7, 0.8, 0.9}
};

static double c1 = 1.0;

f19::f19()
    : Problem(6)
{
    Data l, r;
    l.resize(dimension);
    r.resize(dimension);
    for (int i = 0; i < dimension; i++)
    {
        l[i] =  -2.0;
        r[i] =  2.0;
    setLeftMargin(l);
    setRightMargin(r);
}

}
double f19::funmin(Data &x)
{
    double sum = 0.0;

    for (int i = 0; i < 4; ++i) {
        double sum1 = 0.0;

        for (int j = 0; j < 6; ++j) {
            sum1 += a[i][j] * pow(x[j] - p[i][j], 2.0);
        }

        sum += c1 * exp(-sum1);
    }

    return -sum;
}

Data f19::gradient(Data &x)
{
    Data g;
    g.resize(dimension);

    for (int j = 0; j < dimension; ++j) {
        double sum = 0.0;

        for (int i = 0; i < 4; ++i) {
            double sum1 = 0.0;

            for (int k = 0; k < 6; ++k) {
                sum1 += a[i][k] * pow(x[k] - p[i][k], 2.0);
            }

            sum += c1 * 2.0 * a[i][j] * (x[j] - p[i][j]) * exp(-sum1);
        }

        g[j] = -sum;
    }

    return g;
}
