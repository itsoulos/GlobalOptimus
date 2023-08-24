#include "armijosearch.h"

ArmijoSearch::ArmijoSearch(Problem *p)
    :LineSearch(p)
{

}

void    ArmijoSearch::setLambda(double l)
{
    ArmijoLambda = l;
}

double  ArmijoSearch::getDirection(Data &x)
{
    double f0 = myProblem->statFunmin(x);
    double beta =0.001;
    double t = 0.5;
    int iteration = 0;
    do
    {
            ArmijoLambda = ArmijoLambda * t;
            iteration++;
            if(iteration>=20) break;
    }while(fl(x,ArmijoLambda)>f0-ArmijoLambda*beta*myProblem->grms(x));
    return ArmijoLambda;
}
