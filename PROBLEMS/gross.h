#ifndef FUCH1_H
#define FUCH1_H
# include <OPTIMUS/problem.h>
# include <MLMODELS/mlpproblem.h>
class Gross : public Problem
{
private:
    int patternDimension=1;
    int nodes=10;
    int npoints=100;

    MlpProblem *mlp;
    double gamma;

    double vext(double x);
public:
    Gross();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
    virtual QJsonObject done(Data &x);
    ~Gross();
};

#endif // FUCH1_H
