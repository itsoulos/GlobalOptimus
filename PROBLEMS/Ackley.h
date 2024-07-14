#ifndef ACKLEY_H
#define ACKLEY_H
# include <OPTIMUS/problem.h>
class Ackley : public Problem
{
public:
    Ackley ();
    double funmin(Data &x);
    Data gradient(Data &x);

};
#endif // ACKLEY_H
