#ifndef F16_H
#define F16_H
# include <OPTIMUS/problem.h>

class f16 : public Problem
{
public:
    f16 ();
    double funmin(Data &x);
    Data gradient(Data &x);
};

#endif // F16_H
