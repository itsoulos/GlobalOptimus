#ifndef GKLS350_H
#define GKLS350_H
# include <OPTIMUS/problem.h>

class Gkls350 : public Problem
{
public:
    Gkls350();
    double funmin(Data &x);
    Data gradient(Data &x);
};

#endif // GKLS350_H
