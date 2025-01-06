#include "f3.h"


f3::f3()
    : Problem(1)
{
}

double f3::funmin(Data &x) {
    double result = 0.0;
    double y = 0.854;

    for (int i = 0; i < dimension; i++) {
        double xi = x[i];


        double part1 = exp(-2.0 * log(2.0) * pow((xi - 0.08) / y, 2.0));
        double part2 = pow(sin(5.0 * M_PI * (pow(xi, 3.0 / 4.0) - 0.05)), 6.0);

        result += part1 * part2;
    }

    return result;
}

static double dmax(double a, double b) {
    return a > b ? a : b;
}

Data f3::gradient(Data &x) {
 Data g;
    g.resize(dimension);
    for(int i = 0; i < dimension; i++) {
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

void f3::init(QJsonObject &params) {
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    Data l, r;
    l.resize(n);
    r.resize(n);

    for (int i = 0; i < n; i++) {
        l[i] = -n;
        r[i] = n;
    }

    setLeftMargin(l);
    setRightMargin(r);
}
