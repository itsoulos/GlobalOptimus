#include "f1.h"

F1::F1()
    : Problem(1)
{
}

double F1::funmin(Data &x) {
    double result = 0.0;

    for (int i = 0; i < dimension; i++) {
        double xi = x[i];
        if (xi >= 0 && xi < 2.5) {
            result += 80 * (2.5 - xi);
        } else if (xi >= 2.5 && xi < 5.0) {
            result += 64 * (xi - 2.5);
        } else if (xi >= 5.0 && xi < 7.5) {
            result += 64 * (7.5 - xi);
        } else if (xi >= 7.5 && xi < 12.5) {
            result += 28 * (xi - 7.5);
        } else if (xi >= 12.5 && xi < 17.5) {
            result += 28 * (17.5 - xi);
        } else if (xi >= 17.5 && xi < 22.5) {
            result += 32 * (xi - 17.5);
        } else if (xi >= 22.5 && xi < 27.5) {
            result += 32 * (27.5 - xi);
        } else if (xi >= 27.5 && xi <= 30) {
            result += 80 * (xi - 27.5);
        } else {
            result += -1e6;
        }
    }
    return result;
}

Data F1::gradient(Data &x) {

    Data g;
    g.resize(dimension);

    for (int i = 0; i < dimension; i++) {
        double xi = x[i];

        if (xi >= 0 && xi < 2.5) {
            g[i] = -80.0;
        } else if (xi >= 2.5 && xi < 5.0) {
            g[i] = 64.0;
        } else if (xi >= 5.0 && xi < 7.5) {
            g[i] = -64.0;
        } else if (xi >= 7.5 && xi < 12.5) {
            g[i] = 28.0;
        } else if (xi >= 12.5 && xi < 17.5) {
            g[i] = -28.0;
        } else if (xi >= 17.5 && xi < 22.5) {
            g[i] = 32.0;
        } else if (xi >= 22.5 && xi < 27.5) {
            g[i] = -32.0;
        } else if (xi >= 27.5 && xi <= 30) {
            g[i] = 80.0;
        } else {
            g[i] = 0.0;
        }
    }
    return g;
}

void F1::init(QJsonObject &params) {
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
