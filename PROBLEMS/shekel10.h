#ifndef SHEKEL10_H
#define SHEKEL10_H
# include <OPTIMUS/problem.h>

class Shekel10 : public Problem
{
private:
    double a[10][4];
    double c[10];
public:
    Shekel10();
    double funmin(Data &x);
    Data gradient(Data &x);
};

#endif // SHEKEL10_H
