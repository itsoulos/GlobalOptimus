#ifndef EQUAL_MAXIMA_H
#define EQUAL_MAXIMA_H
# include <OPTIMUS/problem.h>

class Equal_Maxima : public Problem
{
public:
    Equal_Maxima();
    double funmin(Data &x);
    Data gradient(Data &x);
};

#endif // EQUAL_MAXIMA_H


