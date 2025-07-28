#ifndef ZAKHAROV_H
#define ZAKHAROV_H

#include <OPTIMUS/problem.h>

class Zakharov : public Problem
{
public:
    Zakharov();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
};

#endif // ZAKHAROV_H