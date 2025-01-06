#ifndef BF1_H
#define BF1_H
# include <OPTIMUS/problem.h>

class Bf1 : public Problem
{
public:
    Bf1();
    double funmin(Data &x);
    Data gradient(Data &x);
};

#endif // BF1_H
