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
    /**
     * @brief FibonacciSearch The constructor of the line search for problem p.
     * @param p
     */
    FibonacciSearch(Problem *p);
    /**
     * @brief setParams Set the bounds of the line search.
     * @param a
     * @param b
     */
    void    setParams(double a,double b);
    /**
     * @brief fib
     * @param k
     * @return  A fibonacci value at k.
     */
    double  fib(int k);
    /**
     * @brief getDirection
     * @param x
     * @return a new direction using the line search starting from point x.
     */
    virtual double getDirection(Data &x);
};

#endif // FIBONACCISEARCH_H
