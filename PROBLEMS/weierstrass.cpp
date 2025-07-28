#include "weierstrass.h"
#include <cmath>
#include <stdexcept>

Weierstrass::Weierstrass()
    : Problem(1), k_max(20), a(0.5), b(3)
{
    // Validate parameters
    if (a <= 0 || a >= 1) throw std::invalid_argument("a must be in (0,1)");
    if (b <= 0 || b != floor(b)) throw std::invalid_argument("b must be a positive integer");
}

double Weierstrass::funmin(Data &x)
{
    double sum = 0.0;
    for (int i = 0; i < getDimension(); i++)
    {
        double term = 0.0;
        for (int k = 0; k <= k_max; k++)
        {
            term += pow(a, k) * cos(2 * M_PI * pow(b, k) * (x[i] + 0.5));
        }
        sum += term;
    }
    // Subtract the constant term to make minimum at zero
    double constant_term = 0.0;
    for (int k = 0; k <= k_max; k++)
    {
        constant_term += pow(a, k) * cos(M_PI * pow(b, k));
    }
    return sum - getDimension() * constant_term;
}

Data Weierstrass::gradient(Data &x)
{
    Data g;
    g.resize(getDimension());
    
    // Numerical gradient approximation since analytical gradient doesn't exist
    double epsilon = 1e-5;
    Data x_plus = x;
    Data x_minus = x;
    
    for (int i = 0; i < getDimension(); i++)
    {
        x_plus[i] += epsilon;
        x_minus[i] -= epsilon;
        double f_plus = funmin(x_plus);
        double f_minus = funmin(x_minus);
        g[i] = (f_plus - f_minus) / (2 * epsilon);
        x_plus[i] = x[i];  // Reset
        x_minus[i] = x[i];  // Reset
    }
    
    return g;
}

void Weierstrass::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    
    // Optional parameters from JSON
    if (params.contains("weierstrass_a")) a = params["weierstrass_a"].toDouble();
    if (params.contains("weierstrass_b")) b = params["weierstrass_b"].toDouble();
    if (params.contains("weierstrass_kmax")) k_max = params["weierstrass_kmax"].toInt();
    
    Data l, r;
    l.resize(n);
    r.resize(n);
    for (int i = 0; i < n; i++)
    {
        l[i] = -0.5;  // Typical range for Weierstrass function
        r[i] = 0.5;
    }
    setLeftMargin(l);
    setRightMargin(r);
}
