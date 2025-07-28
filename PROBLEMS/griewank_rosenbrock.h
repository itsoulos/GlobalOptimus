#ifndef GRIEWANK_ROSENBROCK_H
#define GRIEWANK_ROSENBROCK_H

#include <OPTIMUS/problem.h>

class GriewankRosenbrock : public Problem
{
public:
    GriewankRosenbrock();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
    
private:
    double rosenbrockPart(double x1, double x2);
    double griewankPart(const Data &x);
};

#endif // GRIEWANK_ROSENBROCK_H