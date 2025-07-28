#include "schwefel.h"
#include <cmath>
#include <numeric>

Schwefel::Schwefel()
    : Problem(1)
{
}

double Schwefel::calculateSchwefel(double xi) const
{
    return xi * sin(sqrt(fabs(xi)));
}

double Schwefel::funmin(Data &x)
{
    int n = getDimension();
    double sum = 0.0;
    
    // Calculate Schwefel function value
    for (int i = 0; i < n; i++) {
        sum += calculateSchwefel(x[i]);
    }
    
    // Transform to make minimum at 0 (original has minimum at -n*418.9829)
    return 418.9829 * n - sum;
}

Data Schwefel::gradient(Data &x)
{
    Data g(getDimension());
    int n = getDimension();
    
    for (int i = 0; i < n; i++) {
        double xi = x[i];
        double root = sqrt(fabs(xi));
        if (fabs(xi) < 1e-10) {
            g[i] = 0.0;  // Avoid division by zero
        } else {
            g[i] = -sin(root) - (xi * cos(root)) / (2 * root * (xi > 0 ? 1 : -1));
        }
    }
    
    return g;
}

void Schwefel::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    
    Data l, r;
    l.resize(n);
    r.resize(n);
    
    // Standard range for Schwefel function
    for (int i = 0; i < n; i++) {
        l[i] = -500.0;
        r[i] = 500.0;
    }
    
    setLeftMargin(l);
    setRightMargin(r);
}
