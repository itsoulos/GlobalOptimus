#ifndef ROSENBROCK_H
#define ROSENBROCK_H
# include <OPTIMUS/problem.h>

class Rosenbrock : public Problem
{
public:
    Rosenbrock();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
};

#endif // ROSENBROCK_H
