#include "zakharov.h"
#include <cmath>
#include <numeric>

Zakharov::Zakharov()
    : Problem(1)
{
}

double Zakharov::funmin(Data &x)
{
    int n = getDimension();
    double sum1 = 0.0;
    double sum2 = 0.0;
    double sum3 = 0.0;
    
    // Calculate the three sums
    for (int i = 0; i < n; i++) {
        sum1 += x[i] * x[i];
        sum2 += 0.5 * (i + 1) * x[i];
        sum3 += 0.5 * (i + 1) * x[i];
    }
    
    return sum1 + pow(sum2, 2) + pow(sum3, 4);
}

Data Zakharov::gradient(Data &x)
{
    int n = getDimension();
    Data g(n, 0.0);
    
    // Precompute the sums needed for gradient
    double sum2 = 0.0;
    double sum3 = 0.0;
    for (int i = 0; i < n; i++) {
        sum2 += 0.5 * (i + 1) * x[i];
        sum3 += 0.5 * (i + 1) * x[i];
    }
    
    // Compute gradient for each dimension
    for (int j = 0; j < n; j++) {
        double coeff = 0.5 * (j + 1);
        g[j] = 2 * x[j]                  // From sum1
               + 2 * sum2 * coeff        // From sum2^2
               + 4 * pow(sum3, 3) * coeff; // From sum3^4
    }
    
    return g;
}

void Zakharov::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    
    Data l, r;
    l.resize(n);
    r.resize(n);
    
    // Standard range for Zakharov function
    for (int i = 0; i < n; i++) {
        l[i] = -5.0;
        r[i] = 10.0;
    }
    
    setLeftMargin(l);
    setRightMargin(r);
}
