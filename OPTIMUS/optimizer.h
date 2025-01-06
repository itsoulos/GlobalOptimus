#ifndef OPTIMIZER_H
#define OPTIMIZER_H
# include   <OPTIMUS/problem.h>
# include   <OPTIMUS/parameter.h>
# include   <OPTIMUS/doublebox.h>
# include   <OPTIMUS/similarity.h>
# include   <OPTIMUS/mean.h>
# include   <SAMPLER/maxwellsampler.h>
# include   <SAMPLER/neuralsampler.h>
# include   <SAMPLER/triangularsampler.h>
# include   <SAMPLER/uniformsampler.h>
# include   <SAMPLER/rbfsampler.h>
# include   <SAMPLER/kmeanssampler.h>
# include   <QJsonArray>
# include   <QStringList>
# include   <OPTIMUS/logger.h>
# include   <OPTIMUS/filelogger.h>
# include   <OPTIMUS/editlogger.h>
# include   <OPTIMUS/parameterlist.h>
/**
 * @brief The Optimizer class implements a general class
 * for optimization methods.
 */
class Optimizer
{
protected:
    /**
     * @brief paramList the list of parameters
     */
    ParameterList paramList;

   /**
    * @brief myProblem the objective problem
    */
   Problem          *myProblem;
   /**
    * @brief doubleBox the DoubleBox termination rule
    */
   DoubleBox        doubleBox;
   /**
    * @brief similarity the similarity termination rule
    */
   Similarity       similarity;
   /**
    * @brief mean the meanFitness termination rule
    */
   Mean             mean;
   /**
    * @brief problemSampler the sample used in most optimization methods
    */
   ProblemSampler   *problemSampler;
   /**
    * @brief terminationMethod the used termination rule
    */
   QString          terminationMethod;
   /**
    * @brief methodLogger the use logging class
    */
   Logger           *methodLogger;
public:
    /**
     * @brief Optimizer  the constructor of the class
     */
    Optimizer();
    /**
     * @brief setMethodLogger, changes the logger method
     * @param p
     */
    void        setMethodLogger(Logger *p);
    /**
     * @brief getMethodLogger
     * @return the used logger
     */
    Logger      *getMethodLogger();
    /**
     * @brief showDebug, executed when a log message should be displayed
     */
    virtual     void showDebug();
    /**
     * @brief setProblem, changes the objective problem to p
     * @param p
     */
    void        setProblem(Problem *p);
    /**
     * @brief addParam adds a new parameter
     * @param p
     */
    void        addParam(Parameter p);
    /**
     * @brief setParam adds a new parameter
     * @param name
     * @param value
     * @param help
     */
    void        setParam(QString name,QString value,QString help="");
    /**
     * @brief getParam
     * @param name
     * @return alter the parameter
     */
    Parameter   getParam(QString name);
    /**
     * @brief getParams
     * @return the parameters in json format
     */
    QJsonObject getParams();
    /**
     * @brief setParams changes the parameters
     * @param x
     */
    void        setParams(QJsonObject &x);
    /**
     * @brief init, executed before the method starts
     */
    virtual void init();
    /**
     * @brief step, the step of the optimizer
     */
    virtual void step();
    /**
     * @brief terminated
     * @return true if the method should terminate
     */
    virtual bool terminated();
    /**
     * @brief solve the main execution function of the optimizer
     */
    virtual void solve();
    /**
     * @brief done, executed when the method terminates
     */
    virtual void done();
    /**
     * @brief localSearch
     * @param x
     * @return a new point obtained by the usage of the prefered local search method.
     */
    double  localSearch(Data &x);
    /**
     * @brief getProblem
     * @return a pointer to the objective problem
     */
    Problem     *getProblem();
    /**
     * @brief getParameterNames
     * @return the list of the parameter names
     */
    QStringList getParameterNames() const;
    /**
     * @brief sampleFromProblem, samples from the objective function a series of samples.
     * @param N, the number of samples.
     * @param xsample, the vector holding the samples.
     * @param ysample, the function values of the samples.
     */
    void        sampleFromProblem(int &N,Matrix &xsample,Data &ysample);
    /**
     * @brief getTerminationMethod
     * @return the termination method in string format
     */
    QString     getTerminationMethod() const;
    /**
     * @brief getParameterList
     * @return
     */
    ParameterList getParameterList();
    virtual ~Optimizer();
};

#endif // OPTIMIZER_H
