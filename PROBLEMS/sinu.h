#ifndef SINU_H
#define SINU_H
# include <OPTIMUS/problem.h>

class Sinu : public Problem
{
public:
    Sinu();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
};

#endif // SINU_H
