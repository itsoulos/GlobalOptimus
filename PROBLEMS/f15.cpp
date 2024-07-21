#include "f15.h"

static const double a[] = {0.1957, 0.1947, 0.1735, 0.16, 0.0844, 0.0627, 0.0456, 0.0342, 0.0323, 0.0235, 0.0246};
static const double b[] = {0.25, 0.5, 1.0, 2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0};
static double b1;

f15::f15()
    : Problem(2)
{
    Data l, r;
    l.resize(4);
    r.resize(4);
    for (int i = 0; i < 4; i++)
    {
        l[i] = -5.0;
        r[i] =  5.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double f15::funmin( Data &x){
    double sum = 0.0;
      int dimension=x.size();
    for (int i = 1; i <= 11; ++i) {
        double n = x[0] * (b1 * b1 + b[i-1] * x[1]);
        double y = b1 * b1 + b[i-1] * x[2] + x[3];
        double t = a[i-1] - n / y;
        sum += t * t;
    }
    return sum;
}

Data f15::gradient(Data &x)

{
    Data g;
    g.resize(dimension);
      int dimension=x.size();

    for (int i = 1; i <= dimension; ++i) {
        double n = x[0] * (b1 * b1 + b[i-1] * x[1]);
        double y = b1 * b1 + b[i-1] * x[2] + x[3];
        double t= a[i-1] - n / y;
        double ds = y * y;

        g[0] += -2 * t * (b1 * b1 + b[i-1] * x[1]) / y;
        g[1] += 2 * t * n / ds;
    }
    return g;
}

