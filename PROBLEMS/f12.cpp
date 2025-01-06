#include "f12.h"
#include <vector>
#include <cmath>

f12::f12()
    : Problem(1)
{

}

double f12::u(double xi, double a, double k, double m) {
    if (xi > a) {
        return k * pow(xi - a, m);
    } else if (xi < -a) {
        return k * pow(-xi - a, m);
    } else {
        return 0.0;
    }
}

double Ufun(double *u, double a, double k, double m, int dimension) {
    double sum = 0.0;
    for(int i = 0; i < dimension; i++) {
        double xi = u[i];
        if (xi > a) {
            sum += k * pow(xi - a, m);
        } else if (xi < -a) {
            sum += k * pow(-xi - a, m);
        } else {
            sum += 0.0;
        }
    }
    return sum;
}

double calculate_z(double *u, int dimension) {
    double sum1 = 0.0;
    double sum2 = 0.0;
    double pi = 3.141592653589793;

    sum1 = 10 * pow(sin(pi * (1 + (u[0] + 1) / 4)), 2);

    for (int i = 0; i < dimension - 1; i++) {
        sum2 += pow((u[i] + 1) / 4, 2) * (1 + 10 * pow(sin(pi * (1 + (u[i + 1] + 1) / 4)), 2));
    }

    sum1 += pow((u[dimension - 1] + 1) / 4, 2);

    double z = (pi / dimension) * (sum1 + sum2 + Ufun(u, 10, 100, 4, dimension));

    return z;
}

double f12::funmin(Data &x) {
   int dimension = x.size();
    double y[dimension];
    for (int i = 0; i < dimension; ++i) {
        y[i] = 1 + (x[i] + 1) / 4.0;
    }
    return calculate_z(y, dimension);
}

static double dmax(double a, double b) {
    return a > b ? a : b;
}

Data f12::gradient(Data &x) {
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

void f12::init(QJsonObject &params) {
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
