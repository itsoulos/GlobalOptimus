#ifndef SALKIN_H
#define SALKIN_H

# include <OPTIMUS/problem.h>

class Salkin : public Problem
{
public:
    Salkin();
    double funmin(Data &x);
    Data gradient(Data &x);
    void	feq(vector<double> &x,vector<double> &eq);
    void	fineq(vector<double> &x,vector<double> &ineq);
    int     isFeasible(vector<double> &x);
    virtual QJsonObject done(Data &x);
    int geteq();
    int getineq();
    ~Salkin();
};

#endif // SALKIN_H
