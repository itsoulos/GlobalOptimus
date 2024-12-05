#include "f3.h"

F3::F3()
    : Problem(1)
{

}

double F3::funmin(Data &x) {
    double result = 0.0;
    double y = 0.854;

    for (int i = 0; i < dimension; i++) {
        double x_i = x[i];
        double part1 = exp(-2.0 * log(2.0) * pow((x_i - 0.08) / y, 2.0));
        double part2 = pow(sin(5.0 * M_PI * (pow(x_i, 3.0 / 4.0) - 0.05)), 6.0);
        result += part1 * part2;
    }

    return result;
}

Data F3::gradient(Data &x) {
    Data g;
    g.resize(dimension);
    double y = 0.854;

    for (int i = 0; i < dimension; i++) {
        double x_i = x[i];

        double u = exp(-2.0 * log(2.0) * pow((x_i - 0.08) / y, 2.0));
        double u1 = u * (-4.0 * log(2.0) * (x_i - 0.08) / (y * y));

        double v = pow(sin(5.0 * M_PI * (pow(x_i, 3.0 / 4.0) - 0.05)), 6.0);
        double sin_x = sin(5.0 * M_PI * (pow(x_i, 3.0 / 4.0) - 0.05));
        double cos_x = cos(5.0 * M_PI * (pow(x_i, 3.0 / 4.0) - 0.05));
        double v1 = 6.0 * pow(sin_x, 5.0) * cos_x * 5.0 * M_PI * (3.0 / 4.0) * pow(x_i, -1.0 / 4.0);

        g[i] = u1 * v + u * v1;
    }

    return g;
}
void F3::init(QJsonObject &params) {
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
