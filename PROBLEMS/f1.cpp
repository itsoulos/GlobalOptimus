#include "f1.h"

F1::F1()
    : Problem(1)
{
    Data l, r;
    l.resize(dimension);
    r.resize(dimension);
    for (int i = 0; i < dimension; i++)
    {
        l[i] = 0.0;
        r[i] =  30.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double F1::funmin( Data &x){
    double x1=x[0];
    if (x1 >= 0 && x1 < 2.5) {
        return 80 * (2.5 - x1);
    } else if (x1 >= 2.5 && x1 < 5.0) {
        return 64 * (x1 - 2.5);
    } else if (x1 >= 5.0 && x1 < 7.5) {
        return 64 * (7.5 - x1);
    } else if (x1 >= 7.5 && x1 < 12.5) {
        return 28 * (x1 - 7.5);
    } else if (x1 >= 12.5 && x1 < 17.5) {
        return 28 * (17.5 - x1);
    } else if (x1 >= 17.5 && x1 < 22.5) {
        return 32 * (x1 - 17.5);
    } else if (x1>= 22.5 && x1 < 27.5) {
        return 32 * (27.5 - x1);
    } else if (x1 >= 27.5 && x1 <= 30) {
        return 80 * (x1 - 27.5);
    } else {
        return -1e6;
    }
}




Data F1::gradient(Data &x){

    Data g(1);
double x1 = x[0];

if (x1 >= 0 && x1 < 2.5) {
    g[0] = -80.0;
} else if (x1 >= 2.5 && x1 < 5.0) {
    g[0] = 64.0;
} else if (x1 >= 5.0 && x1 < 7.5) {
    g[0] = -64.0;
} else if (x1 >= 7.5 && x1 < 12.5) {
    g[0] = 28.0;
} else if (x1 >= 12.5 && x1 < 17.5) {
    g[0] = -28.0;
} else if (x1 >= 17.5 && x1 < 22.5) {
    g[0] = 32.0;
} else if (x1 >= 22.5 && x1 < 27.5) {
    g[0] = -32.0;
} else if (x1 >= 27.5 && x1 <= 30) {
    g[0] = 80.0;
} else {
    g[0] = 0.0;
}

return g;
}

