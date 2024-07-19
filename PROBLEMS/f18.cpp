#include "f18.h"

static double a[4][3];
static double p[4][3];
static double c1 = 0.965;

f18::f18()
    : Problem(3)
{
    Data l, r;
    l.resize(3);
    r.resize(3);
    for (int i = 0; i < 3; i++)
    {
        l[i] =  1.0;
        r[i] =  3.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double f18::funmin(Data &x)
{
    double sum = 0.0;

    for (int i = 0; i < 4; ++i) {
        double sum1 = 0.0;

        for (int j = 0; j < 3; ++j) {
            sum1 += a[i][j] * pow(x[j] - p[i][j], 2.0);
        }

        sum += c1 * exp(-sum1);
    }

    return -sum;
}

Data f18::gradient(Data &x)
{
    Data g;
    g.resize(3);

    for (int j = 0; j < 3; ++j) {
        double sum = 0.0;

        for (int i = 0; i < 4; ++i) {
            double sum1 = 0.0;

            for (int k = 0; k < 3; ++k) {
                sum1 += a[i][k] * pow(x[k] - p[i][k], 2.0);
            }

            sum += c1 * 2.0 * (x[j] - p[i][j]) * a[i][j] * exp(-sum1);
        }

        g[j] = -sum;
    }

    return g;
}
