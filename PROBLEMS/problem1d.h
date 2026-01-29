#ifndef PROBLEM1D_H
#define PROBLEM1D_H
#include <OPTIMUS/problem.h>
# include <MLMODELS/nncmodel.h>
class problem1d : public Problem
{
private:
    double Lz=20.0;
    double h = 0.01;
    double D= 0.0796;
    double beta=3.14;
    int npoints = 100;
    double x0,x1;
    double z0,z1;
    NNCModel *model=NULL;
public:
    problem1d();
    virtual double funmin(Data &x);
    virtual Data gradient(Data &x);
    virtual void init(QJsonObject &params);
    virtual QJsonObject done(Data &x);
    ~problem1d();
};

#endif // PROBLEM1D_H
