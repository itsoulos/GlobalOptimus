#ifndef BF2_H
#define BF2_H
# include <OPTIMUS/problem.h>

class Bf2 : public Problem
{
public:
    Bf2();
    double funmin(Data &x);
    Data gradient(Data &x);
};

#endif // BF2_H
