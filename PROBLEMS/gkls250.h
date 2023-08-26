#ifndef GKLS250_H
#define GKLS250_H
# include <OPTIMUS/problem.h>

class Gkls250 : public Problem
{
public:
    Gkls250();
    double funmin(Data &x);
    Data gradient(Data &x);
};

#endif // GKLS250_H
