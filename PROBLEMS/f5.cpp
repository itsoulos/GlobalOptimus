#include "f5.h"

F5::F5()
    : Problem(2)
{

}

double F5::funmin(Data &x) {

    double result = 0.0;

    for (int i = 0; i < dimension; i++) {
        double xi = x[i];
        double part1 = (4.0 - 2.1 * xi * xi + (xi * xi * xi * xi) / 3.0) * xi * xi;
        double part2 = (i < dimension - 1) ? xi * x[i + 1] : 0.0;  // interaction term with next variable
        double part3 = (4.0 * xi * xi - 4.0) * xi * xi;
        result += part1 + part2 + part3;
    }

    return -4.0 * result;
}

Data F5::gradient(Data &x) {
    Data g;
    g.resize(dimension);

    for (int i = 0; i < dimension; i++) {
        double xi = x[i];

        double dpart1_xi = (-4.2 * xi + (4.0 * xi * xi * xi) / 3.0) * xi * xi +
                           (4.0 - 2.1 * xi * xi + (xi * xi * xi * xi) / 3.0) * 2.0 * xi;

        double dpart2_xi = (i < dimension - 1) ? x[i + 1] : 0.0;
        double dpart2_xi_next = (i > 0) ? x[i - 1] : 0.0;

        double dpart3_xi = 8.0 * xi * xi * xi + (4.0 * xi * xi - 4.0) * 2.0 * xi;

        g[i] = -4.0 * (dpart1_xi + dpart2_xi + dpart2_xi_next + dpart3_xi);
    }

    return g;
}

void F5::init(QJsonObject &params) {
    int n = params["opt_dimension"].toInt();
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
