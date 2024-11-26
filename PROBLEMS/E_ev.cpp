#include "E_ev.h"

E_ev::E_ev()
    : Problem(1) {}

void E_ev::init(QJsonObject &params) {
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

double E_ev::distance(const Data &x, size_t i, size_t j) {
    double dx = x[i * 3] - x[j * 3];
    double dy = x[i * 3 + 1] - x[j * 3 + 1];
    double dz = x[i * 3 + 2] - x[j * 3 + 2];
    return sqrt(dx * dx + dy * dy + dz * dz);
}
//Eq 4
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
//Eq 2
double E_ev::VR(double rij) {
    const double d = 6.325;
    const double S = 1.29;
    const double b = 1.5;
    double e = exp(-sqrt(2 * S) * b * (rij - 1));
    return fij(rij) * (d / (S - 1)) * e;
}
//Eq 3
double E_ev::VA(double rij) {
    const double d = 6.325;
    const double S = 1.29;
    const double b = 1.5;
    double e = exp(-sqrt(2 / S) * b * (rij - 1));
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
//Eq 8
double E_ev::Ni(const Data &x, size_t i) {
    return calcNeighbor(x, i);
}
//Eq 8
double E_ev::Nj(const Data &x, size_t j) {
    return calcNeighbor(x, j);
}
//Eq 9
double E_ev::NijConj(const Data &x, size_t i, size_t j) {
    double sum = 0.0;
    size_t n = dimension / 3;

    for (size_t k = 0; k < n; ++k) {
        if (k != i && k != j) {
            double rik = distance(x, i, k);
            sum += fij(rik) * F(x, i, k);
        }
    }
    for (size_t l = 0; l < n; ++l) {
        if (l != j) {
            double rjl = distance(x, j, l);
            sum += fij(rjl) * F(x, j, l);
        }
    }
    return sum;
}
//Eq 10
double E_ev::F(const Data &x, size_t  i, size_t k) {
    double xik = Ni(x, k);
    if (xik > 2.0 && xik < 3.0) {
        return 0.5 * (1 + cos(M_PI * (xik - 2)));
    } else if (xik <= 2.0) {
        return 1.0;
    } else {
        return 0.0;
    }
}
//Eq 5,6
double E_ev::Bij(const Data &x, size_t i, size_t j) {
    double sum = 0.0;
    size_t n = dimension / 3;

    for (size_t k = 0; k < n; ++k) {
        if (k != i && k != j) {
            double rik = distance(x, i, k);
            double angle = calculateAngle(x, i, j, k);
            sum += Gc(angle) * fij(rik);
        }
    }
    const double delta = 0.80469;
    double Val = NijConj(x, i, j);
    double bij = pow(1 + sum, -delta);
    return (bij + bij) / 2.0 + Val / 2.0;
}
//Eq 7
double E_ev::Gc(double theta) {
    const double a0 = 0.011304;
    const double c0 = 19.0;
    const double d0 = 2.5;
    return a0 * (1 + pow(c0 / d0, 2) - pow(c0, 2) / (pow(d0, 2) + pow(1 + cos(theta), 2)));
}

double E_ev::calculateAngle(const Data &x, size_t i, size_t j, size_t k) {
    double v1[3] = {x[j * 3] - x[i * 3], x[j * 3 + 1] - x[i * 3 + 1], x[j * 3 + 2] - x[i * 3 + 2]};
    double v2[3] = {x[k * 3] - x[i * 3], x[k * 3 + 1] - x[i * 3 + 1], x[k * 3 + 2] - x[i * 3 + 2]};
    double P = v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
    double m1 = sqrt(v1[0] * v1[0] + v1[1] * v1[1] + v1[2] * v1[2]);
    double m2 = sqrt(v2[0] * v2[0] + v2[1] * v2[1] + v2[2] * v2[2]);
    return acos(P / (m1 * m2));
}
//Eq 1
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

Data E_ev::gradient(Data &x) {
    Data g;
    g.resize(dimension, 0.0);
    size_t n = dimension / 3;
    const double S = 1.29;
    const double d = 6.325;
    const double b = 1.5;

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            double rij = distance(x, i, j);
            double fij_val = fij(rij);
            double dVR = -sqrt(2 * S) * b * fij_val * (d / (S - 1)) * exp(-sqrt(2 * S) * b * (rij - 1));
            double dVA = -sqrt(2 / S) * b * fij_val * (d * S / (S - 1)) * exp(-sqrt(2 / S) * b * (rij - 1));
            double bij = Bij(x, i, j);

            for (size_t c = 0; c < 3; ++c) {
                double dx = x[i * 3 + c] - x[j * 3 + c];
                double dij = dx / rij;
                g[i * 3 + c] += (dVR - bij * dVA) * dij;
                g[j * 3 + c] -= (dVR - bij * dVA) * dij;
            }
        }
    }
    return g;
}
