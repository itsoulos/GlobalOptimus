#include "f3.h"

F3::F3()
    : Problem(1)
{
    Data l, r;
    l.resize(dimension);
    r.resize(dimension);
    for (int i = 0; i < dimension; i++)
    {
        l[i] = 0.0;
        r[i] =  1.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double F3::funmin( Data &x){
    double x1=x[0];
    double part1 = exp(-2.0 * log(2.0) * pow((x1 - 0.08) / 0.854, 2.0));
    double part2 = pow(sin(5.0 * M_PI * (pow(x1, 3.0/4.0) - 0.05)), 6.0);


    return part1 * part2;
}




Data F3::gradient(Data &x)

{
    Data g;
    g.resize(2);
        double x1=x[0];

    double y = 0.854;


    double u = exp(-2.0 * log(2.0) * pow((x1 - 0.08) / y, 2.0));


    double u1 = u * (-4.0 * log(2.0) * (x1 - 0.08) / (y * y));


    double v = pow(sin(5.0 * M_PI * (pow(x1, 3.0 / 4.0) - 0.05)), 6.0);

    double sin_x = sin(5.0 * M_PI * (pow(x1, 3.0 / 4.0) - 0.05));
    double cos_x = cos(5.0 * M_PI * (pow(x1, 3.0 / 4.0) - 0.05));
    double v1 = 6.0 * pow(sin_x, 5.0) * cos_x * 5.0 * M_PI * (3.0 / 4.0) * pow(x1, -1.0 / 4.0);

   g[0] = u1 * v + u * v1;

    return g;
}
