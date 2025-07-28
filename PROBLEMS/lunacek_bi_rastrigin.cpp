#include "lunacek_bi_rastrigin.h"

LunacekBiRastrigin::LunacekBiRastrigin()
    : Problem(1), mu0(2.5), d(1.0), s(1.0 - 1.0/(2.0*sqrt(20.0 + 1.0)))
{
    calculateParameters();
}

void LunacekBiRastrigin::calculateParameters()
{
    int n = getDimension();
    mu1 = -sqrt((mu0*mu0 - d*d)/s);
    omega = 10.0 / (n * n);
}

double LunacekBiRastrigin::funmin(Data &x)
{
    int n = getDimension();
    double sum1 = 0.0, sum2 = 0.0, sum3 = 0.0;
    
    // Calculate the three main components
    for (int i = 0; i < n; i++) {
        sum1 += pow(x[i] - mu0, 2);
        sum2 += pow(x[i] - mu1, 2);
        sum3 += 1.0 - cos(2.0 * M_PI * (x[i] - mu0));
    }
    
    // Combine the components
    double term1 = std::min(sum1, d*d*n + s*sum2);
    double term2 = 10.0 * sum3;
    
    return term1 + term2 + omega * std::accumulate(x.begin(), x.end(), 0.0);
}

Data LunacekBiRastrigin::gradient(Data &x)
{
    Data g(getDimension());
    int n = getDimension();
    
    // Numerical gradient (analytical would be too complex)
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

void LunacekBiRastrigin::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    calculateParameters();
    
    Data l, r;
    l.resize(n);
    r.resize(n);
    
    // Standard range for this function
    for (int i = 0; i < n; i++) {
        l[i] = -5.0;
        r[i] = 5.0;
    }
    
    setLeftMargin(l);
    setRightMargin(r);
}
