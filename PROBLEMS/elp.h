#ifndef ELP_H
#define ELP_H
# include <OPTIMUS/problem.h>

class Elp : public Problem
{
public:
    Elp();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
};

#endif // ELP_H
