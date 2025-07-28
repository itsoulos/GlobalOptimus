#include "michalewicz.h"
#include <algorithm>

Michalewicz::Michalewicz()
    : Problem(1)  // Default dimension will be overridden by init()
{
}

double Michalewicz::funmin(Data &x)
{
    double sum = 0.0;
    int n = getDimension();
    
    for (int i = 0; i < n; i++) {
        double xi = x[i];
        sum += sin(xi) * pow(sin((i+1) * xi * xi / M_PI), 2 * m);
    }
    
    return -sum;  // We return negative for minimization (original function is to be maximized)
}

Data Michalewicz::gradient(Data &x)
{
    int n = getDimension();
    Data g(n, 0.0);
    
    for (int j = 0; j < n; j++) {
        double xj = x[j];
        double k = j + 1;
        double term1 = cos(xj) * pow(sin(k * xj * xj / M_PI), 2 * m);
        double term2 = sin(xj) * 2 * m * pow(sin(k * xj * xj / M_PI), 2 * m - 1) 
                      * cos(k * xj * xj / M_PI) * (2 * k * xj / M_PI);
        g[j] = -(term1 + term2);  // Negative because we're minimizing
    }
    
    return g;
}

void Michalewicz::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    
    Data l, r;
    l.resize(n);
    r.resize(n);
    
    // Standard range for Michalewicz function
    for (int i = 0; i < n; i++) {
        l[i] = 0.0;
        r[i] = M_PI;
    }
    
    setLeftMargin(l);
    setRightMargin(r);
}
