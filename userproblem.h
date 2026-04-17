#ifndef USERPROBLEM_H
#define USERPROBLEM_H

#include "problem.h"

class UserProblem : public Problem
{
public:
    explicit UserProblem(int n);
    double funmin(Data &x) override;
    Data gradient(Data &x) override;
    ~UserProblem();
};

#endif

