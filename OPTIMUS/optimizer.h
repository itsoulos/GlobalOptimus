#ifndef OPTIMIZER_H
#define OPTIMIZER_H
# include <OPTIMUS/problem.h>
# include <OPTIMUS/parameter.h>
# include <OPTIMUS/doublebox.h>
# include <OPTIMUS/similarity.h>
# include <OPTIMUS/mean.h>
# include <SAMPLER/maxwellsampler.h>
# include <SAMPLER/neuralsampler.h>
# include <SAMPLER/triangularsampler.h>
# include <SAMPLER/uniformsampler.h>
# include <SAMPLER/rbfsampler.h>
# include <SAMPLER/kmeanssampler.h>
# include <QJsonArray>
# include <QStringList>

/**
 * @brief The Optimizer class implements a general class
 * for optimization methods.
 */
class Optimizer
{
protected:
    /**
    * @brief params, these are the method parameters
    */
   QJsonObject       params;
    /**
    * @brief paramsHelp, the help info for every parameter
    */
   QJsonObject       paramsHelp;
   /**
    * @brief myProblem the objective problem
    */
   Problem          *myProblem;
   DoubleBox        doubleBox;
   Similarity       similarity;
   Mean             mean;
   ProblemSampler   *problemSampler;
public:
    /**
     * @brief Optimizer  the constructor of the class
     */
    Optimizer();
    void        setProblem(Problem *p);
    void        addParam(Parameter p);
    void        setParam(QString name,QString value,QString help="");
    Parameter   getParam(QString name);
    QJsonObject getParams() const;
    void        setParams(QJsonObject &x);
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void solve();
    virtual void done();
    virtual void showDebug();
    double  localSearch(Data &x);
    Problem     *getProblem();
    QStringList getParameterNames() const;
    /**
     * @brief sampleFromProblem, samples from the objective function a series of samples.
     * @param N, the number of samples.
     * @param xsample, the vector holding the samples.
     * @param ysample, the function values of the samples.
     */
    void        sampleFromProblem(int &N,Matrix &xsample,Data &ysample);
    virtual ~Optimizer();
};

#endif // OPTIMIZER_H
