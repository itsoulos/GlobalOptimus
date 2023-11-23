#ifndef FUCH1_H
#define FUCH1_H
# include <OPTIMUS/problem.h>
# include <MLMODELS/mlpproblem.h>
class Fuch1 : public Problem
{
private:
    int patternDimension=1;
    int nodes=5;
    int npoints=100;

    Dataset *trainSet;
    MlpProblem *mlp;

    double e1(double omega);
    double e2(double omega);
    double q1(double q,double omega,double d);
    double q2(double q,double omega,double d);
    void    makeDataset(double omega);
public:
    Fuch1();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
    virtual QJsonObject done(Data &x);
    ~Fuch1();
};

#endif // FUCH1_H
