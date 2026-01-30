#ifndef PROBLEM1D_H
#define PROBLEM1D_H
#include <OPTIMUS/problem.h>
# include <MLMODELS/mlpproblem.h>
# include <MLMODELS/nncmodel.h>
# define USE_MLP
class problem1d : public Problem
{
private:
    double Lz=50.0;
    double Lt=10.0;
    double h = 0.01;
    double beta=3.14;
    double gamma = 1.0; //accepted values: 0,1,-1
    int npoints = 100;
    double t0,t1;
    double z0,z1;

#ifdef USE_MLP
    MlpProblem *model=NULL;
    int modelSize=10;
    Data params;
#else
    NNCModel *model=NULL;
    int    modelSize=100;
#endif
public:
    problem1d();
    void    setModelSize(int n);
    double boundaryInT0(double z);
    double boundaryInZ0RealPart(double t);
    double boundaryInZ0ImagePart(double t);
    double boundaryInZ1RealPart(double t);
    double boundaryInZ1ImagePart(double t);
    double realPartFunmin();
    double imagePartFunmin();
    virtual double funmin(Data &x);
    virtual Data gradient(Data &x);
    virtual void init(QJsonObject &params);
    virtual QJsonObject done(Data &x);
    ~problem1d();
};

#endif // PROBLEM1D_H
