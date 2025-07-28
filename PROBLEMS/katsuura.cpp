#include "katsuura.h"
#include <cmath>
#include <algorithm>

Katsuura::Katsuura()
    : Problem(1)
{
}

double Katsuura::funmin(Data &x)
{
    double product = 1.0;
    int n = getDimension();
    
    for (int i = 0; i < n; i++) {
        double sum_term = 0.0;
        for (int k = 1; k <= 32; k++) {
            sum_term += fabs(pow(2.0, k) * x[i] - floor(pow(2.0, k) * x[i])) / pow(2.0, k);
        }
        product *= pow(1.0 + (i + 1) * sum_term, 10.0 / pow(n, 1.2));
    }
    
    double sum_squares = 0.0;
    for (int i = 0; i < n; i++) {
        sum_squares += x[i] * x[i];
    }
    
    return (10.0 / (n * n)) * product - 10.0 / (n * n) + sum_squares / (D * n);
}

Data Katsuura::gradient(Data &x)
{
    Data g(getDimension());
    int n = getDimension();
    
    // Numerical gradient computation (more practical for this complex function)
    double epsilon = 1e-8;
    Data x_plus = x;
    Data x_minus = x;
    
    for (int i = 0; i < n; i++) {
        x_plus[i] += epsilon;
        x_minus[i] -= epsilon;
        
        double f_plus = funmin(x_plus);
        double f_minus = funmin(x_minus);
        
        g[i] = (f_plus - f_minus) / (2.0 * epsilon);
        
        // Reset values
        x_plus[i] = x[i];
        x_minus[i] = x[i];
    }
    
    return g;
}

void Katsuura::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    
    Data l, r;
    l.resize(n);
    r.resize(n);
    
    // Standard range for Katsuura function
    for (int i = 0; i < n; i++) {
        l[i] = 0.0;
        r[i] = 1.0;  // Note: Some implementations use [-5,5]
    }
    
    setLeftMargin(l);
    setRightMargin(r);
}
