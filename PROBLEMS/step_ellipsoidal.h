#ifndef STEP_ELLIPSOIDAL_H
#define STEP_ELLIPSOIDAL_H
#include <OPTIMUS/problem.h>

class StepEllipsoidal : public Problem
{
public:
    StepEllipsoidal();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
};

#endif // STEP_ELLIPSOIDAL_H