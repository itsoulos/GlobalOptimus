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
# include <QStringList>
# include <QJsonObject>
class MethodLoader
{
private:
    //list
    QVector<Optimizer*> method;
    QStringList methodName;
public:
    MethodLoader();
    QStringList getMethodList() const;
    Optimizer   *getSelectedMethod(QString name);
    QJsonObject getMethodParams(QString name) const;
    ~MethodLoader();
};

#endif // METHODLOADER_H
