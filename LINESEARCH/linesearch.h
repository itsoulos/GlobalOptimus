#ifndef LINESEARCH_H
#define LINESEARCH_H
# include <OPTIMUS/problem.h>

class LineSearch
{
protected:
    Problem *myProblem;
public:
    LineSearch(Problem *p);
    virtual double fl(Data &x,double h);
    virtual double getDirection(Data &x);
    ~LineSearch();
};

#endif // LINESEARCH_H
