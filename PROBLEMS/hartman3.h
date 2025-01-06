#ifndef HARTMAN3_H
#define HARTMAN3_H
# include <OPTIMUS/problem.h>

class Hartman3 : public Problem
{
public:
    Hartman3();
    double funmin(Data &x);
    Data gradient(Data &x);
};

#endif // HARTMAN3_H
