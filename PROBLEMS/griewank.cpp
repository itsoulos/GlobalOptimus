#include "griewank.h"
#include <cmath>
#include <numeric>

Griewank::Griewank()
    : Problem(1)
{
}

double Griewank::funmin(Data &x)
{
    double sum = 0.0;
    double product = 1.0;
    int n = getDimension();

    // Calculate sum of squares
    for (int i = 0; i < n; i++) {
        sum += x[i] * x[i];
    }

    // Calculate product of cosines
    for (int i = 0; i < n; i++) {
        product *= cos(x[i] / sqrt(i + 1.0));
    }

    return 1.0 + sum / 4000.0 - product;
}

Data Griewank::gradient(Data &x)
{
    int n = getDimension();
    Data g(n, 0.0);

    // Precompute the full product of cosines
    double full_product = 1.0;
    std::vector<double> cos_terms(n);
    for (int i = 0; i < n; i++) {
        double denom = sqrt(i + 1.0);
        cos_terms[i] = cos(x[i] / denom);
        full_product *= cos_terms[i];
    }

    // Compute gradient for each dimension
    for (int j = 0; j < n; j++) {
        double denom = sqrt(j + 1.0);

        // Partial derivative of the sum term
        double sum_deriv = x[j] / 2000.0;

        // Partial derivative of the product term
        double product_deriv = 0.0;
        if (fabs(cos_terms[j]) > 1e-10) {
            // Compute product without j-th term
            double product_without_j = full_product / cos_terms[j];
            // Derivative of cos(x_j/sqrt(j+1)) is -sin(x_j/sqrt(j+1))/sqrt(j+1)
            product_deriv = product_without_j * -sin(x[j] / denom) / denom;
        }

        g[j] = sum_deriv - product_deriv;
    }

    return g;
}

void Griewank::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);

    Data l, r;
    l.resize(n);
    r.resize(n);

    // Standard range for Griewank function
    for (int i = 0; i < n; i++) {
        l[i] = -100.0;
        r[i] = 100.0;
    }

    setLeftMargin(l);
    setRightMargin(r);
}
