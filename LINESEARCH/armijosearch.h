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
    ArmijoSearch(Problem *p);
    /**
     * @brief setLambda
     * @param l
     */
    void setLambda(double l);
    /**
     * @brief getDirection
     * @param x
     * @return
     */
    virtual double getDirection(Data &x);
};

#endif // ARMIJOSEARCH_H
