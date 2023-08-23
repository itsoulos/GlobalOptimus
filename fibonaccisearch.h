#ifndef FIBONACCISEARCH_H
#define FIBONACCISEARCH_H
# include <linesearch.h>

class FibonacciSearch : public LineSearch
{
private:
    double fiba,fibb;
public:
    FibonacciSearch(Problem *p);
    void setParams(double a,double b);
    double fib(int k);
    virtual double getDirection(Data &x);
};

#endif // FIBONACCISEARCH_H
