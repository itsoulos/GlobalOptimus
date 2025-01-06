 #include "schwefel.h"

schwefel::schwefel()
    : Problem(2)
{
    Data l, r;
    l.resize(2);
    r.resize(2);
    for (int i = 0; i < 2; i++)
    {
        l[i] = -100;
        r[i] = 100;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double schwefel::funmin( Data &x)
{
    double y = 0.0;

    for (int i = 0; i < 2; ++i) {
        double sum = 0.0;
        for (int j = 0; j <= i; ++j) {
            sum += x[i];
        }
        y += sum * sum;
    }

    return y;
}

Data schwefel::gradient(Data &x)

{
    Data g;
    g.resize(2);
    for (int i = 0; i < 2; ++i) {
        for (int k = i; k < 2; ++k) {
            double sum = 0.0;
            for (int j = 0; j <= k; ++j) {
                sum += x[j];
            }
            g[i] += 2 * sum;
        }
    }

    return g;
}
