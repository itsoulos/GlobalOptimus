#ifndef BRANIN_H
#define BRANIN_H
# include <OPTIMUS/problem.h>

class Branin : public Problem
{
public:
    Branin();
    double funmin(Data &x);
    Data gradient(Data &x);
};

#endif // BRANIN_H
