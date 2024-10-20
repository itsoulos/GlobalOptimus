#ifndef RBFPROBLEM_H
#define RBFPROBLEM_H

# include <MLMODELS/dataset.h>
# include <MLMODELS/model.h>
# include <OPTIMUS/problem.h>
# include <math.h>
class RbfProblem :public Problem, public Model
{
private:
    Data weight;
    vector<Data> centers;
    Data variances;
    double      gaussianDerivative(Data &x,Data &m,double v,int pos);
    double      gaussianSecondDerivative(Data &x,Data &m,double v,int pos);

    void    getWeightDerivative(int index,Data &x,double &g);
    void    getVarianceDerivative(int index,Data &x,double &g);
    void    getCenterDerivative(int index,Data &x,Data &g);
    /** test
     */
    Data lastGaussianValues;
public:
    RbfProblem();
    double  getDerivative(Data &x,int pos);
    double  getSecondDerivative(Data &x,int pos);
    void    setParameters(Data &x);
    void    getParameters(Data &x);
    virtual void initModel();
    virtual double  funmin(Data &x);
    virtual Data    gradient(Data &x);
    double  getOutput(Data &x);
    void    runKmeans(vector<Data> &point, int K,vector<Data> &centers,
                                Data &variances);
    double  gaussian(Data &x,Data &center,double variance);
    virtual void init(QJsonObject &params);
    virtual QJsonObject done(Data &x);
    virtual ~RbfProblem();
};

#endif // RBFPROBLEM_H
