#include "f5.h"

F5::F5()
    : Problem(2)
{
    Data l, r;
    l.resize(dimension);
    r.resize(dimension);
    for (int i = 0; i < dimension; i++)
    {
        l[i] = -5.0;
        r[i] =  5.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double F5::funmin( Data &x){
    double x1=x[0];
    double x2=x[1];
    double part1 = (4.0 - 2.1 * x1 * x1 + (x1 * x1 * x1 * x1) / 3.0) * x1 * x1;
    double part2 = x1 * x2;
    double part3 = (4.0 * x2 * x2 - 4.0) * x2 * x2;

    return -4.0 * (part1 + part2 + part3);
}




Data F5::gradient(Data &x)

{
    Data g;
    g.resize(2);
    double x1 = x[0];
    double x2 = x[1];

    double dpart1_x1 = (-4.2 * x1 + (4.0 * x1 * x1 * x1) / 3.0) * x1 * x1 +
                        (4.0 - 2.1 * x1 * x1 + (x1 * x1 * x1 * x1) / 3.0) * 2.0 * x1;

    double dpart2_x1 = x2;
    double dpart2_x2 = x1;

    double dpart3_x2 = 8.0 * x2 * x2 * x2 + (4.0 * x2 * x2 - 4.0) * 2.0 * x2;

    g[0] = -4.0 * (dpart1_x1 + dpart2_x1);

    g[1] = -4.0 * (dpart2_x2 + dpart3_x2);
    return g;
}
