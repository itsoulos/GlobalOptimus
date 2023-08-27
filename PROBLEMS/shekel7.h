#ifndef SHEKEL7_H
#define SHEKEL7_H
# include <OPTIMUS/problem.h>

class Shekel7 : public Problem
{
public:
    Shekel7();
    double funmin(Data &x);
    Data gradient(Data &x);
};

#endif // SHEKEL7_H
