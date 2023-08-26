#ifndef CM_H
#define CM_H
# include <OPTIMUS/problem.h>

class Cm : public Problem
{
public:
    Cm();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
};

#endif // CM_H
