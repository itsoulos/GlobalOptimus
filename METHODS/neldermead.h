#ifndef NELDERMEAD_H
#define NELDERMEAD_H
# include <OPTIMUS/problem.h>
# include <OPTIMUS/collection.h>
# include <OPTIMUS/optimizer.h>

class NelderMead : public Optimizer
{
private:
    /** @brief vector of couples firs t=> f(sample), second => sample.
     **/
    vector<pair<double, Data>> population;
    int max_generations;
    Data lmargin, rmargin, fitness_array, bestPoint, centerPoint, reflectedPoint,
        expandedPoint, contractedPoint;
    double x1, x2, stopat, variance, ybestPoint, sum, newSum, alpha, gamma, ro, sigma,
        worst, secondWorst, yreflectedPoint, yexpandedPoint, ycontractedPoint;
    int generation, population_count, population_size, bestMinIndex, n;
    /** @brief order, sorting progress.
     **/
    void order();
    /** @brief center, center finding.
     **/
    void center();
    /** @brief reflection, reflection progress.
     **/
    void reflection();
    /** @brief expansion, expansion progress.
     **/
    void expansion();
    /** @brief contraction, contraction internal progress.
     **/
    void contraction();
    /** @brief contractionB, contraction external progress.
     **/
    void contractionB();
    /** @brief shrink, shrinking progress.
     **/
    void shrink();
    bool haveInitialized;
public:
    NelderMead();
    void setPoint(Data &x,double &y);
    void getPoint(Data &x,double &y);
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void showDebug();
    virtual void done();
     ~NelderMead();
};

#endif // NELDERMEAD_H
