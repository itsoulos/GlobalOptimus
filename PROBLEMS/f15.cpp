#include "f15.h"

static const double a[] = {0.1957, 0.1947, 0.1735, 0.16, 0.0844, 0.0627, 0.0456, 0.0342, 0.0323, 0.0235, 0.0246};
static const double b[] = {0.25, 0.5, 1.0, 2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0};

f15::f15()
    : Problem(4)
{

}

double f15::funmin(Data &x) {
    int dimension=x.size();
    double sum = 0.0;
    for (int i = 1; i <= dimension; ++i) {
        double n = x[0] * (b[i - 1] * b[i - 1] + b[i - 1] * x[1]);
        double y = b[i - 1] * b[i - 1] + b[i - 1] * x[2] + x[3];
        double t = a[i - 1] - n / y;
        sum += t * t;
    }
    return sum;
}

static double dmax(double a, double b) {
    return a > b ? a : b;
}

Data f15::gradient(Data &x) {
    Data g;
    g.resize(dimension);

    for (int i = 0; i < dimension; i++) {
        double eps = pow(1e-18, 1.0 / 3.0) * dmax(1.0, fabs(x[i]));


        x[i] += eps;
        double v1 = funmin(x);
        x[i] -= 2.0 * eps;
        double v2 = funmin(x);

        g[i] = (v1 - v2) / (2.0 * eps);
        x[i] += eps;
    }

    return g;
}
void f15::init(QJsonObject &params) {
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
