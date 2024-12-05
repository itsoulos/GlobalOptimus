#include "Sphere.h"

Sphere::Sphere()
    : Problem(1)
{

}

double Sphere::funmin( Data &x)
{
    int dimension=x.size();
    double sum=0.0;
    for(int i=0;i<dimension;i++)
    {
        sum+=x[i]*x[i];;
    }
    return sum;
}

Data Sphere::gradient(Data &x)
{       Data g;
        g.resize(dimension);
    for(int i=0;i<dimension;i++){
    g[i] += 2.0 * x[i];

    }
    return g;
}

void  Sphere::init(QJsonObject &params) {
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


