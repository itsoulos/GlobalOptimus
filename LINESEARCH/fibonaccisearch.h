#ifndef FIBONACCISEARCH_H
#define FIBONACCISEARCH_H
# include <LINESEARCH/linesearch.h>

/**
 * @brief The FibonacciSearch class implements
 * the Fibonacci line search algorithm.
 */
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
