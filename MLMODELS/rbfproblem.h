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
    void    getWeightDerivative(int index,Data &x,double &g);
    void    getVarianceDerivative(int index,Data &x,double &g);
    void    getCenterDerivative(int index,Data &x,Data &g);
public:
    RbfProblem();
    void    setParameters(Data &x);
    void    getParameters(Data &x);
    virtual double  funmin(Data &x);
    virtual Data    gradient(Data &x);
    double  getOutput(Data &x);
    void    runKmeans(vector<Data> &point, int K,vector<Data> &centers,
                                Data &variances);
    double  gaussian(Data &x,Data &center,double variance);
    virtual void init(QJsonObject &params);
    virtual QJsonObject done(Data &x);
    ~RbfProblem();
};

#endif // RBFPROBLEM_H
