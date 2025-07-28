#ifndef RASTRIGIN_H
#define RASTRIGIN_H
#include <OPTIMUS/problem.h>

class Rastrigin : public Problem
{
public:
    Rastrigin();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
};

#endif // RASTRIGIN_H 
