#include "f13.h"

f13::f13()
    : Problem(2)
{
    Data l, r;
    l.resize(dimension);
    r.resize(dimension);
    for (int i = 0; i < dimension; i++)
    {
        l[i] = -100;
        r[i] =  100;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double f13::funmin( Data &x){
    double sum = 0.0;
double p = 1.0;

for (int i = 0; i < dimension; ++i) {
    sum += x[i] * x[i];
    p *= cos(x[i] / sqrt(i));
}

return (1.0 / 4000.0) * sum - p + 1.0;
}


Data f13::gradient(Data &x)

{
    Data g;
     g.resize(dimension);

    for (int j = 0; j < dimension; ++j) {
        g[j] += x[j] / 2000.0;
    }

    for (int j = 0; j < dimension; ++j) {
        double p_cos = 1.0;
        for (int i = 0; i < dimension; ++i) {
            p_cos *= cos(x[i] / sqrt(i));
        }
        double sin_t = sin(x[j] / sqrt(j));
        g[j] -= p_cos * sin_t / sqrt(j);
    }

    return g;
}
