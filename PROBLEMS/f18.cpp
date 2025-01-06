#include "f18.h"

static double a[4][3];
static double p[4][3];
static double c1 = 0.965;

f18::f18()
    : Problem(3)
{


}

double f18::funmin(Data &x)
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

Data f18::gradient(Data &x)
{
    Data g;
    g.resize(dimension);

    for (int j = 0; j < dimension; ++j) {
        double sum = 0.0;

        for (int i = 0; i < 4; ++i) {
            double sum1 = 0.0;

            for (int k = 0; k < dimension; ++k) {
                sum1 += a[i][k] * pow(x[k] - p[i][k], 2.0);
            }

            sum += c1 * 2.0 * (x[j] - p[i][j]) * a[i][j] * exp(-sum1);
        }

        g[j] = -sum;
    }

    return g;
}

void f18::init(QJsonObject &params) {
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

