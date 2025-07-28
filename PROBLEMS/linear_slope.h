#ifndef LINEAR_SLOPE_H
#define LINEAR_SLOPE_H
#include <OPTIMUS/problem.h>

class LinearSlope : public Problem
{
public:
    LinearSlope();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
};

#endif // LINEAR_SLOPE_H 
