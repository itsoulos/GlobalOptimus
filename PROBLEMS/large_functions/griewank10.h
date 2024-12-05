#ifndef GRIEWANK10_H
#define GRIEWANK10_H
# include <OPTIMUS/problem.h>

class Griewank10 : public Problem
{
public:
    Griewank10();
    double funmin(Data &x);
    Data gradient(Data &x);
     void init(QJsonObject &params);
};

#endif // GRIEWANK10_H
