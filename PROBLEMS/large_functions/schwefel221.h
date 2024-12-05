#ifndef SCHWEFEL221_H
#define SCHWEFEL221_H
# include <OPTIMUS/problem.h>
class schwefel221 : public Problem
{
public:
    schwefel221 ();
    double funmin(Data &x);
    Data gradient(Data &x);
void init(QJsonObject &params);
};
#endif// SCHWEFEL221_H


