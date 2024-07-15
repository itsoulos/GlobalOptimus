#ifndef F12_H
#define F12_H
# include <OPTIMUS/problem.h>

class f12 : public Problem
{
public:
    f12 ();
    double funmin(Data &x);
    Data gradient(Data &x);
    double u(double xi, double a, double k, double m);

};

#endif // F12_H
