#ifndef EXTENDED_F10_H
#define EXTENDED_F10_H
# include <OPTIMUS/problem.h>

class  Extended_f10 : public Problem
{
public:
     Extended_f10();
    double funmin(Data &x);
    Data gradient(Data &x);
};
#endif // EXTENDED_F10_H
