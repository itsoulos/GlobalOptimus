#ifndef F13_H
#define F13_H
# include <OPTIMUS/problem.h>

class f13 : public Problem
{
public:
    f13 ();
    double funmin(Data &x);
    Data gradient(Data &x);
};

#endif // F13_H
