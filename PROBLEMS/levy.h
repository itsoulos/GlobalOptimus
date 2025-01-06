#ifndef LEVY_H
#define LEVY_H

# include <OPTIMUS/problem.h>

class Levy : public Problem
{
public:
    Levy();
    double funmin(Data &x);
    Data gradient(Data &x);
    void	feq(vector<double> &x,vector<double> &eq);
    void	fineq(vector<double> &x,vector<double> &ineq);
    int     isFeasible(vector<double> &x);
    virtual QJsonObject done(Data &x);
    int geteq();
    int getineq();
    ~Levy();
};

#endif // LEVY_H
