 #include "schwefel221.h"

schwefel221::schwefel221()
    : Problem(2)
{
    Data l, r;
    l.resize(2);
    r.resize(2);
    for (int i = 0; i < 2; i++)
    {
        l[i] = -500;
        r[i] = 500;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double schwefel221::funmin( Data &x)
{
    double y ;
    double sum = 0.0;

    for (int i = 0; i < 2; ++i) {
        sum += -x[i] * sin(sqrt(abs(x[i])));
    }

   y = 418.9829 * 2 + sum;

    return y;

}



Data schwefel221::gradient(Data &x)

{
    Data g;
    g.resize(2);


    for (int i = 0; i < 2; ++i) {
        double t1 = abs(x[i]);
        double t2 = (x[i] >= 0) ? 1.0 : -1.0;
   g[i] = -sin(sqrt(t1)) - t2 *  0.5 * x[i]*cos(sqrt(t1))/ sqrt(t1);    }
    return g;
}
