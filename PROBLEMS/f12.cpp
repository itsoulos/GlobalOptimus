#include "f12.h"
#include <vector>
#include <cmath>

f12::f12()
    : Problem(2)
{
    Data l, r;
    l.resize(dimension);
    r.resize(dimension);
    for (int i = 0; i < dimension; i++) {
        l[i] = -100;
        r[i] = 100;
    }
    setLeftMargin(l);
    setRightMargin(r);
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


double f12::funmin(Data &x) {
    int n = dimension;
    std::vector<double> y(n);
    for (int i = 0; i < n; ++i) {
        y[i] = 1 + (x[i] + 1) / 4.0;
    }

    double s1 = 10 * sin(M_PI * y[0]);
    double s2 = 0.0;
    double s3 = 0.0;
    double s4 = 0.0;

    for (int i = 0; i < n - 1; ++i) {
        s2 += pow(y[i] - 1, 2);
        s3 += pow(y[i + 1] - 1, 2);
        s4 += u(x[i], 10, 100, 4);
    }
    s4 += u(x[n-1], 10, 100, 4);

    double result = (M_PI / n) * (s1 + s2 + s3 + (y[n - 1] - 1) * (y[n - 1] - 1) * (1 + 10 * pow(sin(M_PI * y[n - 1]), 2))) + s4;
    return result;
}

Data f12::gradient(Data &x) {
    int n = dimension;
    Data g;
    g.resize(dimension);

    std::vector<double> y(n);
    for (int i = 0; i < n; ++i) {
        y[i] = 1 + (x[i] + 1) / 4.0;
    }

    double pi_n = M_PI / n;

    for (int i = 0; i < n; ++i) {
        double y_i = y[i];
        double sin_t = sin(M_PI * y_i);
        double cos_t = cos(M_PI * y_i);

        double t = pi_n * (2 * (y_i - 1) + 10 * sin_t * cos_t * M_PI);
        double u_grad = u(x[i], 10, 100, 4);

        g[i] = t * 0.25 + u_grad;
    }

    return g;
}
