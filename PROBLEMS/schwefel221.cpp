 #include "schwefel221.h"

schwefel221::schwefel221()
    : Problem(1)
{
    Data l, r;
    l.resize(dimension);
    r.resize(dimension);
    for (int i = 0; i < dimension; i++)
    {
        l[i] = -500;
        r[i] = 500;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double schwefel221::funmin( Data &x)
{ int n=x.size();
    double y ;
    double sum = 0.0;

    for (int i = 0; i < n; ++i) {
        sum += -x[i] * sin(sqrt(abs(x[i])));
    }

   y = 418.9829 * n + sum;

    return y;

}



Data schwefel221::gradient(Data &x)

{ int n=x.size();
    Data g;
    g.resize(n);


    for (int i = 0; i < n; ++i) {
        double t1 = abs(x[i]);
        double t2 = (x[i] >= 0) ? 1.0 : -1.0;
   g[i] = -sin(sqrt(t1)) - t2 *  0.5 * x[i]*cos(sqrt(t1))/ sqrt(t1);    }
    return g;
}
