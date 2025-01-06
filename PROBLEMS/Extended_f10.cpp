#include "Extended_f10.h"

Extended_f10::Extended_f10()
    : Problem(2)
{
    Data l, r;
    l.resize(2);
    r.resize(2);
    for (int i = 0; i < 2; i++) {
        l[i] = -5.12;
        r[i] = 5.12;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double  Extended_f10::funmin(Data &x)
{
    double sum = 0.0;
    int n=x.size();
    for (int i = 0; i < n; ++i) {
        sum += x[i] * x[i] - 10 * cos(2 * M_PI * x[i]);
    }
    return 10 * n + sum;
}


Data    Extended_f10::gradient(Data &x)
{
    int n=x.size();
    Data g;
    g.resize(n);
    for (int i = 0; i < n; ++i) {
        g[i] = 2 * x[i] + 20 * M_PI * sin(2 * M_PI * x[i]);
    }
    return g;
}


