#include "vincent.h"

Vincent::Vincent()
    :Problem(3)
{

    Data l, r;
    l.resize(dimension);
    r.resize(dimension);
    for (int i = 0; i < dimension; i++)
    {
        l[i] = 0.25;
        r[i] =  10.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double  Vincent::funmin(Data &x)
{

    int D = x.size();
    double sum = 0.0;

    for (int i = 0; i < D; ++i) {
        sum += sin(10 * log(x[i]));
    }

    return sum / D;

}

Data    Vincent::gradient(Data &x)
{
    int D = x.size();
    std::vector<double> g(D);

    for (int i = 0; i < D; ++i) {
        g[i] = (10 / D) * cos(10 * log(x[i])) / x[i];
    }

    return g;

}
