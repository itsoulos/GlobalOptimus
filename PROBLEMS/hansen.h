#ifndef HANSEN_H
#define HANSEN_H
# include <OPTIMUS/problem.h>

class Hansen : public Problem
{
public:
    Hansen();
    double funmin(Data &x);
    Data gradient(Data &x);
};

#endif // HANSEN_H
