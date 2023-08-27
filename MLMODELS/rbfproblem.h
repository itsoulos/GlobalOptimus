#ifndef RBFPROBLEM_H
#define RBFPROBLEM_H

# include <MLMODELS/dataset.h>
# include <MLMODELS/model.h>
# include <OPTIMUS/problem.h>
# include <math.h>
class RbfProblem :public Problem, Model
{
private:
    Data weight;
    vector<Data> centers;
    Data variances;
public:
    RbfProblem();
    void    setParameters(Data &x);
    void    getParameters(Data &x);
    double  funmin(Data &x);
    Data    gradient(Data &x);
    double  getOutput(Data &x);
    double  gaussian(Data &x,Data &center,double variance);
    virtual void init(QJsonObject &params);
    virtual QJsonObject done(Data &x);
    ~RbfProblem();
};

#endif // RBFPROBLEM_H
