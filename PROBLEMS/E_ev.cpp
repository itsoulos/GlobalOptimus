#include "E_ev.h"
#include <cmath>

E_ev::E_ev()
    : Problem(1) {}

void E_ev::init(QJsonObject &params) {
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n * 3);

    Data l, r;
    l.resize(n * 3);
    r.resize(n * 3);

    for (int i = 0; i < n * 3; i++) {
        l[i] = -dimension;
        r[i] = dimension;
    }

    setLeftMargin(l);
    setRightMargin(r);
}

double E_ev::distance(const Data &x, size_t i, size_t j) {
    double dx = x[i * 3] - x[j * 3];
    double dy = x[i * 3 + 1] - x[j * 3 + 1];
    double dz = x[i * 3 + 2] - x[j * 3 + 2];
    return sqrt(dx * dx + dy * dy + dz * dz);
}

// Eq 4
double E_ev::fij(double rij) {
    const double R1 = 1.7;
    const double R2 = 2.0;

    if (rij < R1) {
        return 1.0;
    } else if (rij > R2) {
        return 0.0;
    }
    double t = (rij - R1) * M_PI / (R2 - R1);
    return 0.5 * (1 + cos(t));
}

// Eq 2
double E_ev::VR(double rij) {
    const double Re = 1.315;
    const double d = 6.325;
    const double S = 1.29;
    const double b = 1.5;
    double e = exp(-sqrt(2 * S) * b * (rij - Re));
    return fij(rij) * (d / (S - 1)) * e;
}

// Eq 3
double E_ev::VA(double rij) {
    const double Re = 1.315;
    const double d = 6.325;
    const double S = 1.29;
    const double b = 1.5;
    double e = exp(-sqrt(2 / S) * b * (rij - Re));
    return fij(rij) * (d * S / (S - 1)) * e;
}

double E_ev::calcNeighbor(const Data &x, size_t index) {
    double sum = 0.0;
    size_t n = dimension / 3;
    for (size_t k = 0; k < n; ++k) {
        double rik = distance(x, index, k);
        sum += fij(rik);
    }
    return sum;
}

// Eq 8
double E_ev::Ni(const Data &x, size_t i) {
    return calcNeighbor(x, i);
}

// Eq 9
double E_ev::Fij(const Data &x, size_t i, size_t j) {
    double sum = 0.0;
    size_t n = dimension / 3;


    double Ni_val = Ni(x, i);
    double Nj_val = Ni(x, j);


    for (size_t k = 0; k < n; ++k) {
        if (k != i && k != j) {
            double rik = distance(x, i, k);
            sum += fij(rik) * (Ni_val + Nj_val);
        }
    }
    return sum;
}

// Eq 5
double E_ev::Bij(const Data &x, size_t i, size_t j) {
    double sum = 0.0;
    size_t n = dimension / 3;

    for (size_t k = 0; k < n; ++k) {
        if (k != i && k != j) {
            double rik = distance(x, i, k);
            double angle = calculateAngle(x, i, j, k);
            sum += fij(rik) * Gc(angle);
        }
    }

    const double delta = 0.80469;
    double Val = Fij(x, i, j);
    double bij = pow(1 + sum, -delta);
    return (bij + bij) / 2.0 + Val / 2.0;
}

// Eq 6
double E_ev::Gc(double theta) {
    const double a0 = 0.011304;
    const double c0 = 19.0;
    const double d0 = 2.5;
    return a0 * (1 + pow(c0 / d0, 2) - pow(c0, 2) / (pow(d0, 2) + pow(1 + cos(theta), 2)));
}

// Eq 1
double E_ev::funmin(Data &x) {
    double energy = 0.0;
    size_t n = dimension / 3;

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            double rij = distance(x, i, j);
            double bij = Bij(x, i, j);
            energy += VR(rij) - bij * VA(rij);
        }
    }
    return energy;
}

static double dmax(double a, double b) {
    return a > b ? a : b;
}


Data E_ev::gradient(Data &x) {
    int n = x.size();
    Data g;
    g.resize(n);

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

double E_ev::calculateAngle(const Data &x, size_t i, size_t j, size_t k) {
    double xi = x[i * 3], yi = x[i * 3 + 1], zi = x[i * 3 + 2];
    double xj = x[j * 3], yj = x[j * 3 + 1], zj = x[j * 3 + 2];
    double xk = x[k * 3], yk = x[k * 3 + 1], zk = x[k * 3 + 2];

    double v1x = xi - xj, v1y = yi - yj, v1z = zi - zj;
    double v2x = xk - xj, v2y = yk - yj, v2z = zk - zj;

    double dot = v1x * v2x + v1y * v2y + v1z * v2z;
    double mag1 = sqrt(v1x * v1x + v1y * v1y + v1z * v1z);
    double mag2 = sqrt(v2x * v2x + v2y * v2y + v2z * v2z);

    double angle = acos(dot / (mag1 * mag2));

    return angle;
}
