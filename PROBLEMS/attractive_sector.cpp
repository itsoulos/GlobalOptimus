#include "attractive_sector.h"
#include <cmath>

AttractiveSector::AttractiveSector()
    : Problem(1)
{
}

double AttractiveSector::funmin(Data &x)
{
    double sum = 0.0;
    for (int i = 0; i < getDimension(); i++)
    {
        if (x[i] * x[i] > 0.0)
        {
            sum += pow(100.0, 2.0) * x[i] * x[i];
        }
        else
        {
            sum += x[i] * x[i];
        }
    }
    return sum;
}

Data AttractiveSector::gradient(Data &x)
{
    Data g;
    g.resize(getDimension());
    for (int i = 0; i < getDimension(); i++)
    {
        if (x[i] * x[i] > 0.0)
        {
            g[i] = 2.0 * pow(100.0, 2.0) * x[i];
        }
        else
        {
            g[i] = 2.0 * x[i];
        }
    }
    return g;
}

void AttractiveSector::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    Data l, r;
    l.resize(n);
    r.resize(n);
    for (int i = 0; i < n; i++)
    {
        l[i] = -5.0;
        r[i] = 5.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}
