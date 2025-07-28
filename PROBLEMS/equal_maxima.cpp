#include "equal_maxima.h"

EqualMaxima::EqualMaxima()
 :Problem(1)
{

}


double EqualMaxima::funmin(Data &x) {
    double result = 0.0;
    for (int i = 0; i < dimension; i++) {
        double sin_value = sin(5.0 * M_PI * x[i]);
        result += pow(sin_value, 6.0);
    }

    return result;
}

Data EqualMaxima::gradient(Data &x) {
    Data g(dimension);
    for (int i = 0; i < dimension; i++) {
        double sin_value = sin(5.0 * M_PI * x[i]);
        double cos_value = cos(5.0 * M_PI * x[i]);
        g[i] = 30.0 * M_PI * pow(sin_value, 5.0) * cos_value;
    }

    return g;
}

void EqualMaxima::init(QJsonObject &params) {
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


