#ifndef ELLIPSOIDAL_H
#define ELLIPSOIDAL_H
#include <OPTIMUS/problem.h>

class Ellipsoidal : public Problem
{
public:
    Ellipsoidal();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
};

#endif // ELLIPSOIDAL_H 
