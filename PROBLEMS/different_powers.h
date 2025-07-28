#ifndef DIFFERENT_POWERS_H
#define DIFFERENT_POWERS_H
#include <OPTIMUS/problem.h>

class DifferentPowers : public Problem
{
public:
    DifferentPowers();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
};

#endif // DIFFERENT_POWERS_H