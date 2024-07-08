#ifndef PROBLEMLOADER_H
#define PROBLEMLOADER_H
# include <OPTIMUS/problem.h>
# include <QJsonObject>
# include <PROBLEMS/rastriginproblem.h>
# include <PROBLEMS/test2nproblem.h>
# include <PROBLEMS/bf1.h>
# include <PROBLEMS/bf2.h>
# include <PROBLEMS/branin.h>
# include <PROBLEMS/camel.h>
# include <PROBLEMS/cm.h>
# include <PROBLEMS/diffpower.h>
# include <PROBLEMS/easom.h>
# include <PROBLEMS/elp.h>
# include <PROBLEMS/exp.h>
# include <PROBLEMS/gkls250.h>
# include <PROBLEMS/gkls2100.h>
# include <PROBLEMS/gkls350.h>
# include <PROBLEMS/goldstein.h>
# include <PROBLEMS/griewank2.h>
# include <PROBLEMS/griewank10.h>
# include <PROBLEMS/hansen.h>
# include <PROBLEMS/hartman3.h>
# include <PROBLEMS/hartman6.h>
# include <PROBLEMS/potential.h>
# include <PROBLEMS/rosenbrock.h>
# include <PROBLEMS/gross.h>
# include <PROBLEMS/shekel5.h>
# include <PROBLEMS/shekel7.h>
# include <PROBLEMS/shekel10.h>
# include <PROBLEMS/sinu.h>
# include <PROBLEMS/test30n.h>
# include <MLMODELS/mlpproblem.h>
# include <MLMODELS/rbfproblem.h>
# include <PROBLEMS/diracproblem.h>
# include <PROBLEMS/userproblem.h>
# include <PROBLEMS/fuchss.h>
# include <PROBLEMS/levy.h>
# include <PROBLEMS/salkin.h>
# include <PROBLEMS/hess.h>
# include <PROBLEMS/fuch1.h>
# include <OPTIMUS/optimizer.h>
class ProblemLoader
{
private:
    Problem *myProblem;
    QJsonObject myParams;
    QStringList problemList;
    QString selectedProblem;

public:
    ProblemLoader();
    QStringList     getProblemList() const;
    Problem         *loadProblem(QString name);
    void            setParams(QJsonObject &p);
    QJsonObject     getParams() const;
    QString         getSelectedProblemName() const;
    static QString getProblemReport(QString selectedProblem,Problem *myProblem);
    static Problem *getProblemFromName(QString name);
    ~ProblemLoader();
};

#endif // PROBLEMLOADER_H
