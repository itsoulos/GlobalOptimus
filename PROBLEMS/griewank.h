#ifndef GRIEWANK_H
#define GRIEWANK_H

#include <OPTIMUS/problem.h>

class Griewank : public Problem
{
public:
    Griewank();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
};

#endif // GRIEWANK_H
