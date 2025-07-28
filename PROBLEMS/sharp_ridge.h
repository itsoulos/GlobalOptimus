#ifndef SHARP_RIDGE_H
#define SHARP_RIDGE_H
#include <OPTIMUS/problem.h>

class SharpRidge : public Problem
{
public:
    SharpRidge();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
};

#endif // SHARP_RIDGE_H