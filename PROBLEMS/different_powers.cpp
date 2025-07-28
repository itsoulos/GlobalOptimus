#include "different_powers.h"
#include <cmath>

DifferentPowers::DifferentPowers()
    : Problem(1)
{
}

double DifferentPowers::funmin(Data &x)
{
    double sum = 0.0;
    for (int i = 0; i < getDimension(); i++)
    {
        double exponent = 2.0 + (4.0 * i) / (getDimension() - 1.0); // Εκθέτης από 2 έως 6
        sum += pow(fabs(x[i]), exponent);
    }
    return sqrt(sum);
}

Data DifferentPowers::gradient(Data &x)
{
    Data g;
    g.resize(getDimension());
    
    double sum = 0.0;
    for (int i = 0; i < getDimension(); i++)
    {
        double exponent = 2.0 + (4.0 * i) / (getDimension() - 1.0);
        sum += pow(fabs(x[i]), exponent);
    }
    double sqrt_sum = sqrt(sum);
    
    for (int i = 0; i < getDimension(); i++)
    {
        double exponent = 2.0 + (4.0 * i) / (getDimension() - 1.0);
        if (fabs(x[i]) > 1e-8)  // Αποφυγή απροσδιόριστων μορφών
        {
            g[i] = (exponent * pow(fabs(x[i]), exponent - 1.0) * (x[i] > 0 ? 1 : -1)) / (2.0 * sqrt_sum);
        }
        else
        {
            g[i] = 0.0;
        }
    }
    
    return g;
}

void DifferentPowers::init(QJsonObject &params)
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
