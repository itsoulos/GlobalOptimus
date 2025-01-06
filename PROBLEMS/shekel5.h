#ifndef SHEKEL5_H
#define SHEKEL5_H
# include <OPTIMUS/problem.h>

class Shekel5 : public Problem
{
public:
    Shekel5();
    double funmin(Data &x);
    Data gradient(Data &x);
};

#endif // SHEKEL5_H
