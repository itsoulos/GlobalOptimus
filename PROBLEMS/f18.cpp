#include "f18.h"

static double a[4][3] = {{1.0, 2.0, 3.0}, {0.5, 1.5, 2.5}, {2.0, 3.0, 4.0}, {1.5, 2.5, 3.5}};
static double p[4][3] = {{0.1, 0.2, 0.3}, {0.2, 0.3, 0.4}, {0.3, 0.4, 0.5}, {0.4, 0.5, 0.6}};
static double c1 = 1.0;

f18::f18()
    : Problem(3) // Ορίζουμε τη διάσταση του προβλήματος σε 3 μεταβλητές
{
    Data l, r;
    l.resize(dimension);
    r.resize(dimension);
    for (int i = 0; i < dimension; i++)
    {
        l[i] =  1.0; // Κάτω όριο των μεταβλητών x_j
        r[i] =  3.0; // Πάνω όριο των μεταβλητών x_j
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double f18::funmin(Data &x)
{
    double sum = 0.0;

    for (int i = 0; i < 4; ++i) {
        double sum1 = 0.0;

        for (int j = 0; j < 3; ++j) {
            sum1 += a[i][j] * pow(x[j] - p[i][j], 2.0);
        }

        sum += c1 * exp(-sum1);
    }

    return -sum;
}

Data f18::gradient(Data &x)
{
    Data g;
    g.resize(dimension);

    for (int j = 0; j < dimension; ++j) {
        double sum = 0.0;

        for (int i = 0; i < 4; ++i) {
            double sum1 = 0.0;

            for (int k = 0; k < 3; ++k) {
                sum1 += a[i][k] * pow(x[k] - p[i][k], 2.0);
            }

            sum += c1 * 2.0 * (x[j] - p[i][j]) * a[i][j] * exp(-sum1);
        }

        g[j] = -sum;
    }

    return g;
}
