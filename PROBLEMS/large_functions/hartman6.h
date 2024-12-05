#ifndef HARTMAN6_H
#define HARTMAN6_H
# include <OPTIMUS/problem.h>

class Hartman6 : public Problem
{
public:
    Hartman6();
    double funmin(Data &x);
    Data gradient(Data &x);
    void init(QJsonObject &params);
};

#endif // HARTMAN6_H
