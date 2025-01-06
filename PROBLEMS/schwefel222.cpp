 #include "schwefel222.h"

schwefel222::schwefel222()
    : Problem(2)
{

}
void  schwefel222::init(QJsonObject &params) {
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
double schwefel222::funmin( Data &x)

{
    int dimension=x.size();
    double y ;
    double p =1.0;
    double sum = 0.0;
    for (int i = 0; i < dimension; ++i) {
        sum = sum + abs(x[i]);
         p= p*abs(x[i]);
    }

    y = sum+p;

    return y;

}



Data schwefel222::gradient(Data &x)

{
    Data g;
          g.resize(dimension);
    double p =1.0;

    for (int i = 0; i < dimension; ++i) {
        double s_xi = (x[i] > 0) ? 1.0 : -1.0;
        double p_without_xi = (x[i] == 0) ? 0 : p/ abs(x[i]);
        g[i] = s_xi + p_without_xi * s_xi;  }
    return g;
}


