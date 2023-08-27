#ifndef GKLS2100_H
#define GKLS2100_H
# include <OPTIMUS/problem.h>

class Gkls2100 : public Problem
{
public:
    Gkls2100();
    double funmin(Data &x);
    Data gradient(Data &x);
};

#endif // GKLS2100_H
