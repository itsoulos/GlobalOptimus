#ifndef F17_H
#define F17_H
# include <OPTIMUS/problem.h>

class f17 : public Problem
{
public:
    f17 ();
    double funmin(Data &x);
    Data gradient(Data &x);
};

#endif // F17_H
