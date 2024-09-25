#ifndef F1_H
#define F1_H
# include <OPTIMUS/problem.h>

class F1 : public Problem
{
public:
    F1 ();
    double funmin(Data &x);
    Data gradient(Data &x);

};

#endif // F1_H
