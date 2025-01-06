#ifndef CAMEL_H
#define CAMEL_H
# include <OPTIMUS/problem.h>

class Camel : public Problem
{
public:
    Camel();
    double funmin(Data &x);
    Data gradient(Data &x);
};

#endif // CAMEL_H
