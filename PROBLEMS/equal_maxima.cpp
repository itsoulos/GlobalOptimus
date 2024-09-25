#include "equal_maxima.h"

Equal_Maxima::Equal_Maxima()
 :Problem(1)
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



double  Equal_Maxima::funmin(Data &x)
{

    double sin_value = sin(5.0 * M_PI * x[0]);
    return pow(sin_value, 6.0);

}

Data   Equal_Maxima::gradient(Data &x)
{
    Data g(dimension);
    double sin_value = sin(5 * M_PI * x[0]);
    double cos_value = cos(5 * M_PI * x[0]);
    g[0] = 30.0 * M_PI * pow(sin_value, 5.0) * cos_value;
    return g;



}
