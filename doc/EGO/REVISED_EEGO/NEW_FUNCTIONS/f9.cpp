#include "f9.h"

f9::f9()
    : Problem(2)
{
    Data l, r;
    l.resize(dimension);
    r.resize(dimension);
    for (int i = 0; i < dimension; i++)
    {
        l[i] = 0.0;
        r[i] =  1.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double f9::funmin( Data &x){
    int n=x.size();
    double sum = 0.0;
    for (int k = 1; k <= n; k++) {
        sum += (10.0 + 9.0 * cos(2 * M_PI * k * x[k]));
    }

    return -sum;
}



Data f9::gradient(Data &x)

{ int n = x.size();
    std::vector<double> g(n);
    for (int i = 1; i <= n; i++) {
        g[i-1] = 18.0 * M_PI * i * sin(2 * M_PI * i * x[i-1]);
    }
    return g;
}
