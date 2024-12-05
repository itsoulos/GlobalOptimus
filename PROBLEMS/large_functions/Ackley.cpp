#include "Ackley.h"
Ackley::Ackley()
    : Problem(2)
{

}

double Ackley::funmin( Data &x)
{
   double a = 20;
    double b = 0.2;
   double c = 2 * M_PI;

    double sum1 = 0.0;
    double sum2 = 0.0;

    for (int i = 0; i < dimension; ++i) {
        sum1 += x[i] * x[i];
        sum2 += cos(c * x[i]);
    }


    return (-a * (exp(-b * (sqrt(sum1 / dimension)))) - exp(sum2 / dimension) + a + exp(1));
}

Data Ackley::gradient(Data &x)

{        Data g;
         g.resize(dimension);
    double a = 20;
    double b = 0.2;
    double c = 2 * M_PI;
    double sum1 = 0.0;
    double sum2 = 0.0;
    for (int i = 0; i < dimension; ++i) {
        sum1 += x[i] * x[i];
        sum2 += cos(c * x[i]);
    }

    for (int i = 0; i < dimension; ++i) {
        double t1 = a * b * x[i] / sqrt(sum1 / dimension) * exp(-b * sqrt(sum1 / dimension));
        double t2 = c * sin(c * x[i]) * exp(sum2 / dimension) / dimension;
        g[i] = t1 + t2;
    }

    return g;
}
void    Ackley::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    Data l, r;
    l.resize(n);
    r.resize(n);
    for (int i = 0; i < n; i++)
    {
        l[i] = -dimension;
        r[i] = dimension;
    }
    setLeftMargin(l);
    setRightMargin(r);
  }
