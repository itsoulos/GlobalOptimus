#ifndef LEVY_H
#define LEVY_H

#include <OPTIMUS/problem.h>

class Levy : public Problem
{
public:
    Levy();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
};

#endif // LEVY_H