#include "f16.h"

f16::f16()
    : Problem(2)
{

}

double f16::funmin(Data &x)
{
    {

        double result = 0.0;
        for (int i = 0; i < dimension; i++)
        {

            result += x[i] * x[i] - 2.1 * pow(x[i], 4) + (1.0 / 3.0) * pow(x[i], 6);


        }
        return result;
    }

}

Data f16::gradient(Data &x)

{
    Data g;
    g.resize(dimension);

      for (int i = 0; i < dimension; i++)
        {
            g[i] = 2 * x[i] - 8.4 * pow(x[i], 3) + 2.0 * pow(x[i], 5);


        return g;
    }
}
void f16::init(QJsonObject &params) {
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    Data l, r;
    l.resize(n);
    r.resize(n);

    for (int i = 0; i < n; i++) {
        l[i] = -dimension;
        r[i] = dimension;
    }

    setLeftMargin(l);
    setRightMargin(r);
}


