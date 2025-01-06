#ifndef TEST2NPROBLEM_H
#define TEST2NPROBLEM_H
# include <OPTIMUS/problem.h>

class Test2nProblem : public Problem
{
public:
    Test2nProblem();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
};

#endif // TEST2NPROBLEM_H
