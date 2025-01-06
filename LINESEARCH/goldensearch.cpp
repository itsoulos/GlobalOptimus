#include "goldensearch.h"

GoldenSearch::GoldenSearch(Problem *p)
    :LineSearch(p)
{

}

void    GoldenSearch::setParams(double a,double b)
{
    goldena  = a;
    goldenb  = b;
}


double  GoldenSearch::getDirection(Data &x)
{
    double phi = (sqrt(5.0)-1.0)/2.0;
    int iteration = 1;
    while(true)
    {
        double x1 = goldena+(1-phi)*(goldenb-goldena);
        double x2 = goldena+phi * (goldenb-goldena);
        double diff =fabs(x1-x2);
        if(diff<1e-6) break;

        if(fl(x,x1)<fl(x,x2))
        {
            goldenb = x2;
        }
        else
        {
            goldena=x1;
        }
        iteration++;

    }
    return goldena;

}
