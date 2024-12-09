#include "f13.h"

f13::f13()
    : Problem(2)
{

}

double f13::funmin(Data &x) {
    double sum = 0.0;
    double p = 1.0;

    for (int i = 0; i < dimension; ++i) {
        sum += x[i] * x[i];
        p *= cos(x[i] / sqrt(i + 1));
    }

    return (1.0 / 4000.0) * sum - p + 1.0;
}

Data f13::gradient(Data &x) {
    Data g;
    g.resize(dimension);

    for (int j = 0; j < dimension; ++j) {
        g[j] = x[j] / 2000.0;

        double p_cos = 1.0;
        for (int i = 0; i < dimension; ++i) {
            if (i != j) {
                p_cos *= cos(x[i] / sqrt(i + 1));
            }
        }
        g[j] -= p_cos * sin(x[j] / sqrt(j + 1)) / sqrt(j + 1);
    }

    return g;
}

void f13::init(QJsonObject &params) {
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
