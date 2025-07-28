#include "Ackley.h"
Ackley::Ackley()
    : Problem(2)
{
    Data l, r;
    l.resize(2);
    r.resize(2);
    for (int i = 0; i < 2; i++)
    {
        l[i] = -15;
        r[i] = 30;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double Ackley::funmin( Data &x)
{
   double a = 20;
    double b = 0.2;
   double c = 2 * M_PI;
    int d=x.size();

    double sum1 = 0.0;
    double sum2 = 0.0;

    for (int i = 0; i < 2; ++i) {
        sum1 += x[i] * x[i];
        sum2 += cos(c * x[i]);
    }


    return (-a * (exp(-b * (sqrt(sum1 / d)))) - exp(sum2 / d) + a + exp(1));
}

Data Ackley::gradient(Data &x)

{
    Data g;
    double a = 20;
    double b = 0.2;
      int d=x.size();
    double c = 2 * M_PI;
    double sum1 = 0.0;
    double sum2 = 0.0;
    g.resize(2);
    for (int i = 0; i < 2; ++i) {
        sum1 += x[i] * x[i];
        sum2 += cos(c * x[i]);
    }

    for (int i = 0; i < 2; ++i) {
        double t1 = a * b * x[i] / sqrt(sum1 / d) * exp(-b * sqrt(sum1 / d));
        double t2 = c * sin(c * x[i]) * exp(sum2 / d) / d;
        g[i] = t1 + t2;
    }

    return g;
}
