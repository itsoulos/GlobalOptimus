#ifndef METHODLOADER_H
#define METHODLOADER_H
# include <METHODS/gradientdescent.h>
# include <METHODS/multistart.h>
# include <METHODS/genetic.h>
# include <METHODS/bfgs.h>
# include <METHODS/lbfgs.h>
# include <METHODS/differentialevolution.h>
# include <METHODS/ipso.h>
# include <METHODS/neuralminimizer.h>
# include <METHODS/parallelde.h>
# include <METHODS/parallelpso.h>
# include <METHODS/neldermead.h>
# include <METHODS/adam.h>
# include <METHODS/armadillo1.h>
# include <METHODS/simanmethod.h>
# include <METHODS/usermethod.h>
# include <METHODS/ofalgorithm.h>
# include <METHODS/gwooptimizer.h>
# include <METHODS/ego.h>
# include <METHODS/hybridmethod.h>
# include <METHODS/woa.h>
# include <METHODS/mewoa.h>
# include <METHODS/jDElsgo.h>
# include <METHODS/saop.h>
# include <METHODS/acop.h>
# include <QStringList>
# include <QJsonObject>
/**
 * @brief The MethodLoader class loads all the Optimizers into an array
 */
class MethodLoader
{
private:
    /**
     * @brief method, the list of optimizers
     */
    QVector<Optimizer*> method;
    /**
     * @brief methodName, the list of optimizer names.
     */
    QStringList methodName;
public:
    /**
     * @brief MethodLoader, the default constructor of the class.
     */
    MethodLoader();
    /**
     * @brief getMethodList
     * @return the names of the methods.
     */
    QStringList getMethodList() const;
    /**
     * @brief getSelectedMethod
     * @param name the name of the selected method.
     * @return a pointer to the optimizer with the name of the parameter.
     */
    Optimizer   *getSelectedMethod(QString name);
    /**
     * @brief getMethodParams
     * @param name
     * @return
     */
    QJsonObject getMethodParams(QString name) const;
    /**
     * @brief getMethodParamsNames
     * @param name
     * @return
     */
    QStringList  getMethodParamsNames(QString name) const;

    ~MethodLoader();
};

#endif // METHODLOADER_H
