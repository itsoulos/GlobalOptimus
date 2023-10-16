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
    void setLambda(double l);
    virtual double getDirection(Data &x);
};

#endif // ARMIJOSEARCH_H
