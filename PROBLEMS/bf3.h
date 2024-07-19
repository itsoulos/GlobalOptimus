#ifndef BF3_H
#define BF3_H
# include <OPTIMUS/problem.h>

class bf3 : public Problem
{
public:
    bf3();
    double funmin(Data &x);
    Data gradient(Data &x);
};

#endif // BF3_H
