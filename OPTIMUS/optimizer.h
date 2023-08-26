#ifndef OPTIMIZER_H
#define OPTIMIZER_H
# include <OPTIMUS/problem.h>
# include <OPTIMUS/parameter.h>
# include <OPTIMUS/doublebox.h>
# include <OPTIMUS/similarity.h>
# include <QJsonArray>

/**
 * @brief The Optimizer class implements a general class
 * for optimization methods.
 */
class Optimizer
{
protected:
   QJsonObject       params;
   QJsonObject       paramsHelp;
   Problem          *myProblem;
   DoubleBox        doubleBox;
   Similarity       similarity;
public:
    /**
     * @brief Optimizer  the constructor of the class
     */
    Optimizer();
    void        setProblem(Problem *p);
    void        addParam(Parameter p);
    void        setParam(QString name,QString value,QString help="");
    Parameter   getParam(QString name);
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void solve();
    virtual void done();
    virtual void showDebug();
    double  localSearch(Data &x);
    Problem     *getProblem();
    QStringList getParameterNames() const;
    ~Optimizer();
};

#endif // OPTIMIZER_H
