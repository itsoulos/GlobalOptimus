#include "f19.h"
#include <cmath>

static double a[4][6];

static double p[4][6];

static double c1= 0.83;

f19::f19()
    : Problem(6)
{
    Data l, r;
    l.resize(6);
    r.resize(6);
    for (int i = 0; i < 6; i++)
    {
        l[i] =  0.0;
        r[i] =  1.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
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
    g.resize(6);

    for (int j = 0; j < 6; ++j) {
        g[j] = 0.0;

        for (int i = 0; i < 4; ++i) {
            double sum1 = 0.0;

            for (int k = 0; k < 6; ++k) {
                sum1 += a[i][k] * pow(x[k] - p[i][k], 2.0);
            }

            double exp_term = exp(-sum1);
            g[j] += 2.0 * a[i][j] * (x[j] - p[i][j]) * exp_term;
        }

        g[j] = -c1 * g[j];
    }

    return g;
}
