#include "sharp_ridge.h"
#include <cmath>

SharpRidge::SharpRidge()
    : Problem(1)
{
}

double SharpRidge::funmin(Data &x)
{
    double sum_squares = 0.0;
    for (int i = 1; i < getDimension(); i++)
    {
        sum_squares += x[i] * x[i];
    }
    return x[0] * x[0] + 100.0 * sqrt(sum_squares);
}

Data SharpRidge::gradient(Data &x)
{
    Data g;
    g.resize(getDimension());
    
    double sum_squares = 0.0;
    for (int i = 1; i < getDimension(); i++)
    {
        sum_squares += x[i] * x[i];
    }
    double norm = sqrt(sum_squares);
    
    // Gradient για την πρώτη διάσταση
    g[0] = 2.0 * x[0];
    
    // Gradient για τις υπόλοιπες διαστάσεις
    for (int i = 1; i < getDimension(); i++)
    {
        if (norm > 1e-8)  // Αποφυγή διαίρεσης με 0
        {
            g[i] = 100.0 * x[i] / norm;
        }
        else
        {
            g[i] = 0.0;
        }
    }
    
    return g;
}

void SharpRidge::init(QJsonObject &params)
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
