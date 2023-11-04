#ifndef HESS_H
#define HESS_H
# include <OPTIMUS/problem.h>


class Hess : public Problem
{
public:
    Hess();
    double funmin(Data &x);
    Data gradient(Data &x);
    void	feq(vector<double> &x,vector<double> &eq);
    void	fineq(vector<double> &x,vector<double> &ineq);
    int     isFeasible(vector<double> &x);
    virtual QJsonObject done(Data &x);
    int geteq();
    int getineq();
    ~Hess();
};

#endif // HESS_H
