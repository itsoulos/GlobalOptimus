#ifndef F14_H
#define F14_H
# include <OPTIMUS/problem.h>

class f14 : public Problem
{
public:
    f14 ();
    double funmin(Data &x);
    Data gradient(Data &x);
};


#endif // F14_H
