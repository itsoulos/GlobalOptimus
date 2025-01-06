#ifndef GOLDSTEIN_H
#define GOLDSTEIN_H
# include <OPTIMUS/problem.h>

class Goldstein : public Problem
{
public:
    Goldstein();
    double funmin(Data &x);
    Data gradient(Data &x);
};

#endif // GOLDSTEIN_H
