#ifndef DIFFPOWER_H
#define DIFFPOWER_H
# include <OPTIMUS/problem.h>

class DiffPower : public Problem
{
public:
    DiffPower();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
};

#endif // DIFFPOWER_H
