#ifndef HIMMELBLAU_H
#define HIMMELBLAU_H
# include <OPTIMUS/problem.h>

class Himmelblau: public Problem
{
public:
    Himmelblau();
    double funmin(Data &x);
    Data gradient(Data &x);
};

#endif // HIMMELBLAU_H




