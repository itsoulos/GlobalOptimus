#ifndef FUCH1_H
#define FUCH1_H
# include <OPTIMUS/problem.h>
# include <MLMODELS/mlpproblem.h>
# include <MLMODELS//rbfproblem.h>
class Gross : public Problem
{
private:
    int patternDimension=1;
    int nodes=10;
    int npoints=100;

    MlpProblem *mlp;
    RbfProblem *rbf;
    double gamma;

    double neural(Data &x);
    double neuralDeriv(Data &x);
    double neuralDeriv2(Data &x);
    double vext(double x);
    double model(Data &x);
    double modelDeriv2(Data &x);
public:
    Gross();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
    virtual QJsonObject done(Data &x);
    ~Gross();
};

#endif // FUCH1_H
