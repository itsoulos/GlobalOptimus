 
#include "rastrigin.h"
#include <cmath>

Rastrigin::Rastrigin()
    : Problem(1)
{
}

double Rastrigin::funmin(Data &x)
{
    double sum = 10.0 * getDimension(); // Ο σταθερός όρος 10n
    for (int i = 0; i < getDimension(); i++)
    {
        sum += x[i] * x[i] - 10.0 * cos(2.0 * M_PI * x[i]);
    }
    return sum;
}

Data Rastrigin::gradient(Data &x)
{
    Data g;
    g.resize(getDimension());
    for (int i = 0; i < getDimension(); i++)
    {
        // Παράγωγος ως προς x_i: 2x_i + 10 * 2π * sin(2πx_i)
        g[i] = 2.0 * x[i] + 20.0 * M_PI * sin(2.0 * M_PI * x[i]);
    }
    return g;
}

void Rastrigin::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    Data l, r;
    l.resize(n);
    r.resize(n);
    for (int i = 0; i < n; i++)
    {
        l[i] = -5.12;  // Τυπικό εύρος για τη Rastrigin function
        r[i] = 5.12;
    }
    setLeftMargin(l);
    setRightMargin(r);
}
