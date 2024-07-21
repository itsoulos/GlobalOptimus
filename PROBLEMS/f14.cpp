#include "f14.h"
#include <cmath>


static double a[25][2] = {
    {2.1657, 2.1947}, {2.1260, 1.1294}, {2.0625, 1.0721}, {1.0090, 1.0222}, {1.0312, 1.0312},
    {2.0557, 2.0531}, {2.0658, 1.0809}, {1.1114, 1.0998}, {1.1230, 1.1267}, {1.1456, 1.1452},
    {2.1780, 2.1696}, {1.1623, 1.1807}, {1.2117, 1.1992}, {1.2348, 1.2147}, {1.2471, 1.2343},
    {2.2678, 2.2678}, {1.8801, 1.2907}, {1.2915, 1.3133}, {1.3073, 1.3450}, {1.3177, 1.3680},
    {2.3304, 2.3909}, {1.7425, 1.4161}, {1.0500, 1.4400}, {1.3602, 1.4650}, {1.3726, 1.4926}
};

f14::f14()
    : Problem(2)
{
    Data l, r;
    l.resize(2);
    r.resize(2);
    for (int i = 0; i < 2; i++)
    {
        l[i] = -65;
        r[i] =  65;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double f14::funmin(Data &x) {
    double sum_j = 0.0;
    for (int j = 1; j <= 25; ++j) {
        sum_j += 1.0 / j;
    }

    double sum_i = 0.0;
    for (int i = 1; i <= 2; ++i) {
        sum_i += pow(x[i-1] - a[i-1][i-1], 6.0);
    }

    double y = 1.0 / (1.0 / 500.0 + sum_j + sum_i);
    return y;
}

Data f14::gradient(Data &x) {

    Data g;
    g.resize(dimension);
    int dimension=x.size();

    double sum_j = 0.0;
    for (int j = 1; j <= 25; ++j) {
        sum_j += 1.0 / j;
    }

    for (int i = 1; i <= dimension; ++i) {
        double t1 = x[i-1] - a[i-1][i-1];
        double t2 = pow(t1, 6.0);
        double t3= 1.0 / 500.0 + sum_j + t2;
        g[i-1] = -6.0 * pow(t1, 5.0) / (t3 * t3);
    }

    return g;
}
