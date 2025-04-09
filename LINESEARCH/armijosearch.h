#ifndef ARMIJOSEARCH_H
#define ARMIJOSEARCH_H
# include <LINESEARCH/linesearch.h>

/**
 * @brief The ArmijoSearch class implements the Armijo
 * line search algorithm.
 */
class ArmijoSearch : public LineSearch
{
private:
    double ArmijoLambda;
public:
    /**
     * @brief ArmijoSearch The constructor of the line search for problem p.
     * @param p
     */
    ArmijoSearch(Problem *p);
    /**
     * @brief setLambda Sets the lambda value.
     * @param l
     */
    void setLambda(double l);
    /**
     * @brief getDirection
     * @param x
     * @return a new direction using line search.
     */
    virtual double getDirection(Data &x);
};

#endif // ARMIJOSEARCH_H
