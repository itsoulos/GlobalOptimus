#ifndef SHUBERT_H
#define SHUBERT_H
# include <OPTIMUS/problem.h>
class shubert : public Problem
{
public:
    shubert();
    double funmin(Data &x);
    Data gradient(Data &x);
};

#endif // SHUBERT_H


