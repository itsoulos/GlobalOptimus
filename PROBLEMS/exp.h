#ifndef EXP_H
#define EXP_H
# include <OPTIMUS/problem.h>

class Exp : public Problem
{
public:
    Exp();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
};

#endif // EXP_H
