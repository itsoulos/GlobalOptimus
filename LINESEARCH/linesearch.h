#ifndef LINESEARCH_H
#define LINESEARCH_H
# include <OPTIMUS/problem.h>

/**
 * @brief The LineSearch class defines an abstract class
 * for LineSearch
 */
class LineSearch
{
protected:
    /**
     * @brief myProblem, the objective problem
     */
    Problem *myProblem;
public:
    /**
     * @brief LineSearch, the constructor of the base class
     * @param p
     */
    LineSearch(Problem *p);
    /**
     * @brief fl the function to be minimized in the direction of search
     * @param x
     * @param h
     * @return
     */
    virtual double fl(Data &x,double h);
    /**
     * @brief getDirection computes the new search direction
     * @param x
     * @return
     */
    virtual double getDirection(Data &x);
    ~LineSearch();
};

#endif // LINESEARCH_H
