#ifndef TEST30N_H
#define TEST30N_H
# include <OPTIMUS/problem.h>

class Test30n : public Problem
{
public:
    Test30n();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
};

#endif // TEST30N_H
