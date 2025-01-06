#include "f9.h"

f9::f9()
    : Problem(2)
{

}

double f9::funmin( Data &x){
    int dimension=x.size();
    double sum = 0.0;
    for (int k = 1; k <= dimension; k++) {
        sum += (10.0 + 9.0 * cos(2 * M_PI * k * x[k]));
    }

    return -sum;
}



Data f9::gradient(Data &x)

{

    std::vector<double> g(dimension);
    for (int i = 1; i <= dimension; i++) {
        g[i-1] = 18.0 * M_PI * i * sin(2 * M_PI * i * x[i-1]);
    }
    return g;
}

void f9::init(QJsonObject &params) {
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
