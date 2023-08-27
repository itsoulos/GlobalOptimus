#ifndef GRIEWANK2_H
#define GRIEWANK2_H
# include <OPTIMUS/problem.h>

class Griewank2 : public Problem
{
public:
    Griewank2();
    double funmin(Data &x);
    Data gradient(Data &x);
};

#endif // GRIEWANK2_H
