#include "step_ellipsoidal.h"
#include <cmath>

StepEllipsoidal::StepEllipsoidal()
    : Problem(1)
{
}

double StepEllipsoidal::funmin(Data &x)
{
    double sum = 0.0;
    for (int i = 0; i < getDimension(); i++)
    {
        double z = floor(x[i] + 0.5); // Step function
        sum += pow(1000.0, i / (getDimension() - 1.0)) * z * z; // Ellipsoidal scaling
    }
    return sum;
}

Data StepEllipsoidal::gradient(Data &x)
{
    Data g;
    g.resize(getDimension());
    for (int i = 0; i < getDimension(); i++)
    {
        // Το gradient είναι 0 παντού εκτός από τα σημεία ασυνέχειας (x_i = k + 0.5, k ∈ ℤ)
        // Στην πράξη, χρησιμοποιούμε μια προσέγγιση για αριθμητικό gradient
        double epsilon = 1e-8;
        double x_plus = x[i] + epsilon;
        double x_minus = x[i] - epsilon;
        double z_plus = floor(x_plus + 0.5);
        double z_minus = floor(x_minus + 0.5);
        g[i] = (pow(1000.0, i / (getDimension() - 1.0)) * (z_plus * z_plus - z_minus * z_minus)) / (2.0 * epsilon);
    }
    return g;
}

void StepEllipsoidal::init(QJsonObject &params)
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
