#ifndef POTENTIAL_H
#define POTENTIAL_H
# include <OPTIMUS/problem.h>

class Potential : public Problem
{
public:
    Potential();
    double funmin(Data &x);
    Data gradient(Data &x);
    double gpot(double r);
    double dpot(double r);
    virtual void init(QJsonObject &params);
};

#endif // POTENTIAL_H
