#include "Sphere.h"

Sphere::Sphere()
    : Problem(1)
{
    Data l, r;
    l.resize(dimension);
    r.resize(dimension);
    for (int i = 0; i < dimension; i++)
    {
        l[i] = -500;
        r[i] = 500;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double Sphere::funmin( Data &x)
{
    int n;
    double sum=0.0;
    for(int i=0;i<n;i++)
    {
        sum+=x[i]*x[i];;
    }
    return sum;
}

Data Sphere::gradient(Data &x)
{   int n;
    Data g;
    g.resize(2);
    for(int i=0;i<n;i++){
    g[i] += 2.0 * x[i];

    }
    return g;
}
