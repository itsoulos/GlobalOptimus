#ifndef KATSUURA_H
#define KATSUURA_H

#include <OPTIMUS/problem.h>

class Katsuura : public Problem
{
public:
    Katsuura();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
    
private:
    const double D = 10.0;  // Parameter controlling the ruggedness
};

#endif // KATSUURA_H