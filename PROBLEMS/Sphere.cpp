#include "Sphere.h"

Sphere::Sphere()
    : Problem(2)
{
    Data l, r;
    l.resize(2);
    r.resize(2);
    for (int i = 0; i < 2; i++)
    {
        l[i] = -100;
        r[i] = 100;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double Sphere::funmin( Data &x)
{
    int n=2;
    double sum=0.0;
    for(int i=0;i<n;i++)
    {
        sum+=x[i]*x[i];;
    }
    return sum;
}

Data Sphere::gradient(Data &x)
{   int n=2;
    Data g;
    g.resize(2);
    for(int i=0;i<n;i++){
    g[i] = 2.0 * x[i];

    }
    return g;
}
