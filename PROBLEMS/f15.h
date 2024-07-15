#ifndef F15_H
#define F15_H
# include <OPTIMUS/problem.h>

class f15 : public Problem
{
public:
    f15 ();
    double funmin(Data &x);
    Data gradient(Data &x);
};

#endif // F15_H
