#include "sphere.h"
#include <cmath>

Sphere::Sphere()
    : Problem(1)
{
}

double Sphere::funmin(Data &x)
{
    double sum = 0.0;
    for (int i = 0; i < getDimension(); i++)
    {
        sum += x[i] * x[i];
    }
    return sum;
}

Data Sphere::gradient(Data &x)
{
    gradientCalls++;
    Data g;
    g.resize(getDimension());
    for (int i = 0; i < getDimension(); i++)
    {
        g[i] = 2.0 * x[i];
    }
    return g;
}

void Sphere::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    Data l, r;
    l.resize(n);
    r.resize(n);
    for (int i = 0; i < n; i++)
    {
        l[i] = -5.12;  // Τυπικό εύρος για τη συνάρτηση Sphere
        r[i] = 5.12;
    }
    setLeftMargin(l);
    setRightMargin(r);
}
