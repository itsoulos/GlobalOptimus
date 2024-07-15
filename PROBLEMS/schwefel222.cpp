 #include "schwefel222.h"

schwefel222::schwefel222()
    : Problem(2)
{
    Data l, r;
    l.resize(2);
    r.resize(2);
    for (int i = 0; i < 2; i++)
    {
        l[i] = -10;
        r[i] = 10;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double schwefel222::funmin( Data &x)
{    double y ;
    double p =1.0;
    double sum = 0.0;
    for (int i = 0; i < 2; ++i) {
        sum = sum + abs(x[i]);
         p= p*abs(x[i]);
    }

    y = sum+p;

    return y;

}



Data schwefel222::gradient(Data &x)

{
    Data g;
    g.resize(2);
    double p =1.0;

    for (int i = 0; i < 2; ++i) {
        double s_xi = (x[i] > 0) ? 1.0 : -1.0;
        double p_without_xi = (x[i] == 0) ? 0 : p/ abs(x[i]);
        g[i] = s_xi + p_without_xi * s_xi;  }
    return g;
}


