#ifndef BUCHE_RASTRIGIN_H
#define BUCHE_RASTRIGIN_H
#include <OPTIMUS/problem.h>

class BucheRastrigin : public Problem
{
public:
    BucheRastrigin();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
};

#endif // BUCHE_RASTRIGIN_H 
