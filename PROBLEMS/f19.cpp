#include "f19.h"
#include <cmath>

static double a[4][6];

static double p[4][6];

static double c1= 0.83;

f19::f19()
    : Problem(6)
{


}

double f19::funmin(Data &x)
{
     int dimension=x.size();
    double sum = 0.0;

    for (int i = 0; i < 4; ++i) {
        double sum1 = 0.0;

        for (int j = 0; j < dimension; ++j) {
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
        g[j] = 0.0;

        for (int i = 0; i < 4; ++i) {
            double sum1 = 0.0;

            for (int k = 0; k < dimension; ++k) {
                sum1 += a[i][k] * pow(x[k] - p[i][k], 2.0);
            }

            double exp_term = exp(-sum1);
            g[j] += 2.0 * a[i][j] * (x[j] - p[i][j]) * exp_term;
        }

        g[j] = -c1 * g[j];
    }

    return g;
}
void f19::init(QJsonObject &params) {
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    Data l, r;
    l.resize(n);
    r.resize(n);

    for (int i = 0; i < n; i++) {
        l[i] = -dimension;
        r[i] = dimension;
    }

    setLeftMargin(l);
    setRightMargin(r);
}
