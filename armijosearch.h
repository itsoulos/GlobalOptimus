#ifndef ARMIJOSEARCH_H
#define ARMIJOSEARCH_H
# include <linesearch.h>

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
