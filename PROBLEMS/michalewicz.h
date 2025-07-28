#ifndef MICHALEWICZ_H
#define MICHALEWICZ_H

#include <OPTIMUS/problem.h>

class Michalewicz : public Problem
{
public:
    Michalewicz();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
    
private:
    const double m = 10.0;  // Steepness parameter (typically 10)
};

#endif // MICHALEWICZ_H