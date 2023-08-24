#include "fibonaccisearch.h"

FibonacciSearch::FibonacciSearch(Problem *p)
    :LineSearch(p)
{

}

void    FibonacciSearch::setParams(double a,double b)
{
    fiba = a;
    fibb = b;
}

double  FibonacciSearch::fib(int k)
{
    return (pow(1+sqrt(5),k+1)-pow(1-sqrt(5),k+1))/(pow(2,k+1)*sqrt(5));
}

double FibonacciSearch::getDirection(Data &x)
{
    int iteration = 2;
    while(true)
    {
          double phi = fib(iteration)/fib(iteration+1);
        double x1 = fiba+(1-phi)*(fibb-fiba);
        double x2 = fiba+phi * (fibb-fiba);
        double diff =fabs(x1-x2);
        if(diff<1e-6) break;

        if(fl(x,x1)<fl(x,x2))
        {
            fibb = x2;
        }
        else
        {
            fiba=x1;
        }
        iteration++;

    }
    return fiba;
}
