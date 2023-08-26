#ifndef EASOM_H
#define EASOM_H
# include <OPTIMUS/problem.h>

class Easom : public Problem
{
public:
    Easom();
    double funmin(Data &x);
    Data gradient(Data &x);
};

#endif // EASOM_H
