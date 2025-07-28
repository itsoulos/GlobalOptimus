#include "levy.h"

Levy::Levy()
    : Problem(1)
{
}

double Levy::funmin(Data &x)
{
    int n = getDimension();
    double sum = 0.0;
    std::vector<double> w(n);
    
    // Compute w_i = 1 + (x_i - 1)/4
    for (int i = 0; i < n; i++) {
        w[i] = 1.0 + (x[i] - 1.0) / 4.0;
    }
    
    // First term: sin^2(π w1)
    sum += pow(sin(M_PI * w[0]), 2);
    
    // Middle terms: sum_{i=1}^{n-1} (w_i - 1)^2 [1 + 10 sin^2(π w_i + 1)]
    for (int i = 0; i < n - 1; i++) {
        double term = pow(w[i] - 1.0, 2) * 
                     (1.0 + 10.0 * pow(sin(M_PI * w[i] + 1.0), 2));
        sum += term;
    }
    
    // Last term: (w_n - 1)^2 [1 + sin^2(2 π w_n)]
    sum += pow(w[n-1] - 1.0, 2) * 
           (1.0 + pow(sin(2.0 * M_PI * w[n-1]), 2));
    
    return sum;
}

Data Levy::gradient(Data &x)
{
    int n = getDimension();
    Data g(n, 0.0);
    std::vector<double> w(n);
    std::vector<double> dw(n); // Derivatives of w_i w.r.t. x_i
    
    // Precompute w_i and dw_i/dx_i = 1/4
    for (int i = 0; i < n; i++) {
        w[i] = 1.0 + (x[i] - 1.0) / 4.0;
        dw[i] = 0.25;
    }
    
    // Gradient for first term (i=0)
    g[0] = 2.0 * sin(M_PI * w[0]) * cos(M_PI * w[0]) * M_PI * dw[0];
    
    // Gradient for middle terms (i=0 to n-2)
    for (int i = 0; i < n - 1; i++) {
        double base = w[i] - 1.0;
        double sin_term = sin(M_PI * w[i] + 1.0);
        
        // Derivative of (w_i - 1)^2
        double d_base = 2.0 * base * dw[i];
        
        // Derivative of [1 + 10 sin^2(π w_i + 1)]
        double d_sin = 10.0 * 2.0 * sin_term * cos(M_PI * w[i] + 1.0) * M_PI * dw[i];
        
        // Product rule: d/dx [A*B] = A'B + AB'
        g[i] += d_base * (1.0 + 10.0 * pow(sin_term, 2)) + 
                pow(base, 2) * d_sin;
    }
    
    // Gradient for last term (i=n-1)
    double last_base = w[n-1] - 1.0;
    double last_sin = sin(2.0 * M_PI * w[n-1]);
    
    // Derivative of (w_n - 1)^2
    double d_last_base = 2.0 * last_base * dw[n-1];
    
    // Derivative of [1 + sin^2(2 π w_n)]
    double d_last_sin = 2.0 * last_sin * cos(2.0 * M_PI * w[n-1]) * 2.0 * M_PI * dw[n-1];
    
    // Product rule
    g[n-1] += d_last_base * (1.0 + pow(last_sin, 2)) + 
              pow(last_base, 2) * d_last_sin;
    
    return g;
}

void Levy::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    
    Data l, r;
    l.resize(n);
    r.resize(n);
    
    // Standard range for Levy function
    for (int i = 0; i < n; i++) {
        l[i] = -10.0;
        r[i] = 10.0;
    }
    
    setLeftMargin(l);
    setRightMargin(r);
}
