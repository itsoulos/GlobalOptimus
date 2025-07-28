#ifndef SCHWEFEL_H
#define SCHWEFEL_H

#include <OPTIMUS/problem.h>

class Schwefel : public Problem
{
public:
    Schwefel();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
    
private:
    double calculateSchwefel(double xi) const;
};

#endif // SCHWEFEL_H