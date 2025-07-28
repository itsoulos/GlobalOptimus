#include "griewank_rosenbrock.h"
#include <cmath>

GriewankRosenbrock::GriewankRosenbrock()
    : Problem(1)
{
}

double GriewankRosenbrock::rosenbrockPart(double x1, double x2)
{
    return 100.0 * pow(x2 - pow(x1, 2), 2) + pow(1.0 - x1, 2);
}

double GriewankRosenbrock::griewankPart(const Data &x)
{
    double sum = 0.0;
    double product = 1.0;
    
    for (int i = 0; i < getDimension(); i++) {
        sum += pow(x[i], 2) / 4000.0;
        product *= cos(x[i] / sqrt(i + 1.0));
    }
    
    return sum - product + 1.0;
}

double GriewankRosenbrock::funmin(Data &x)
{
    // First compute Rosenbrock components between consecutive variables
    double rosenbrock_sum = 0.0;
    for (int i = 0; i < getDimension() - 1; i++) {
        rosenbrock_sum += rosenbrockPart(x[i], x[i + 1]);
    }
    
    // Then compute the Griewank envelope
    double griewank_val = griewankPart(x);
    
    // Combine them
    return griewank_val * (rosenbrock_sum / 10.0);
}

Data GriewankRosenbrock::gradient(Data &x)
{
    Data g(getDimension(), 0.0);
    
    // Numerical gradient computation (more stable for this complex function)
    double epsilon = 1e-8;
    Data x_plus = x;
    Data x_minus = x;
    
    for (int i = 0; i < getDimension(); i++) {
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

void GriewankRosenbrock::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    
    Data l, r;
    l.resize(n);
    r.resize(n);
    
    // Recommended range for this function
    for (int i = 0; i < n; i++) {
        l[i] = -5.0;
        r[i] = 5.0;
    }
    
    setLeftMargin(l);
    setRightMargin(r);
}
