#ifndef PROBLEMLOADER_H
#define PROBLEMLOADER_H
#include <OPTIMUS/problem.h>
#include <QJsonObject>
#include <PROBLEMS/rastriginproblem.h>
#include <PROBLEMS/test2nproblem.h>
#include <PROBLEMS/bf1.h>
#include <PROBLEMS/bf2.h>
#include <PROBLEMS/branin.h>
#include <PROBLEMS/camel.h>
#include <PROBLEMS/cm.h>
#include <PROBLEMS/diffpower.h>
#include <PROBLEMS/easom.h>
#include <PROBLEMS/elp.h>
#include <PROBLEMS/exp.h>
#include <PROBLEMS/gkls250.h>
#include <PROBLEMS/gkls2100.h>
#include <PROBLEMS/gkls350.h>
#include <PROBLEMS/goldstein.h>
#include <PROBLEMS/griewank2.h>
#include <PROBLEMS/griewank10.h>
#include <PROBLEMS/hansen.h>
#include <PROBLEMS/hartman3.h>
#include <PROBLEMS/hartman6.h>
#include <PROBLEMS/potential.h>
#include <PROBLEMS/rosenbrock.h>
#include <PROBLEMS/gross.h>
#include <PROBLEMS/shekel5.h>
#include <PROBLEMS/shekel7.h>
#include <PROBLEMS/shekel10.h>
#include <PROBLEMS/sinu.h>
#include <PROBLEMS/test30n.h>
#include <MLMODELS/mlpproblem.h>
#include <MLMODELS/rbfproblem.h>
#include <PROBLEMS/diracproblem.h>
#include <PROBLEMS/userproblem.h>
#include <PROBLEMS/fuchss.h>
#include <PROBLEMS/levy.h>
#include <PROBLEMS/salkin.h>
#include <PROBLEMS/hess.h>
#include <PROBLEMS/fuch1.h>
#include <OPTIMUS/optimizer.h>
#include <PROBLEMS/Ackley.h>

#include <PROBLEMS/schaffer.h>
#include <PROBLEMS/Extended_f10.h>
#include <PROBLEMS/bf3.h>
#include <PROBLEMS/equal_maxima.h>
#include <PROBLEMS/himmelblau.h>
#include <PROBLEMS/shubert.h>
#include <PROBLEMS/vincent.h>
#include <PROBLEMS/E_ev.h>
#include <PROBLEMS/gallagher101.h>
#include <PROBLEMS/gallagher21.h>
#include <PROBLEMS/ellipsoidal.h>
#include <PROBLEMS/rastrigin.h>
#include <PROBLEMS/buche_rastrigin.h>
#include <PROBLEMS/linear_slope.h>
#include <PROBLEMS/attractive_sector.h>
#include <PROBLEMS/step_ellipsoidal.h>
#include <PROBLEMS/rotated_rosenbrock.h>
#include <PROBLEMS/discus.h>
#include <PROBLEMS/sharp_ridge.h>
#include <PROBLEMS/different_powers.h>
#include <PROBLEMS/weierstrass.h>
#include <PROBLEMS/griewank_rosenbrock.h>
#include <PROBLEMS/katsuura.h>
#include <PROBLEMS/lunacek_bi_rastrigin.h>
#include <PROBLEMS/schwefel.h>
#include <PROBLEMS/griewank.h>
#include <PROBLEMS/michalewicz.h>
#include <PROBLEMS/zakharov.h>
#include <PROBLEMS/problem1d.h>

/**
 * @brief The ProblemLoader class
 */
class ProblemLoader
{
private:
    /**
     * @brief myProblem
     */
    Problem *myProblem;
    /**
     * @brief myParams
     */
    QJsonObject myParams;
    /**
     * @brief problemList
     */
    QStringList problemList;
    /**
     * @brief selectedProblem
     */
    QString selectedProblem;

public:
    /**
     * @brief ProblemLoader
     */
    ProblemLoader();
    /**
     * @brief getProblemList
     * @return the list of the supported objective functions.
     */
    QStringList getProblemList() const;
    /**
     * @brief loadProblem
     * @param name
     * @return loads a problem and returns a pointer to it.
     */
    Problem *loadProblem(QString name);
    /**
     * @brief setParams
     * @param p alters the parameter values of the objective problem
     */
    void setParams(QJsonObject &p);
    /**
     * @brief getParams
     * @return the parameters of the problem in json format
     */
    QJsonObject getParams() const;
    /**
     * @brief getSelectedProblemName
     * @return the name of the running objective problem
     */
    QString getSelectedProblemName() const;
    /**
     * @brief getProblemReport
     * @param selectedProblem
     * @param myProblem
     * @return a string report for the objective problem myProblem.
     */
    static QString getProblemReport(QString selectedProblem, Problem *myProblem);
    /**
     * @brief getProblemFromName
     * @param name
     * @return a pointer to the problem provided by parameter name
     */
    static Problem *getProblemFromName(QString name);
    ~ProblemLoader();
};

#endif // PROBLEMLOADER_H
