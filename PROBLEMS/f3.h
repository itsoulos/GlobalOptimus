#ifndef F3_H
#define F3_H
# include <OPTIMUS/problem.h>

class F3 : public Problem
{
public:
    F3 ();
    double funmin(Data &x);
    Data gradient(Data &x);

};

#endif // F3_H
