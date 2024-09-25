#ifndef VINCENT_H
#define VINCENT_H
# include <OPTIMUS/problem.h>

class Vincent: public Problem
{
public:
    Vincent();
    double funmin(Data &x);
    Data gradient(Data &x);
};

#endif // VINCENT_H


