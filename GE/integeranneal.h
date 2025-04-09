#ifndef INTEGERANNEAL_H
#define INTEGERANNEAL_H
# include <OPTIMUS/problem.h>
# include <OPTIMUS/optimizer.h>
# include <GE/program.h>
/**
 * @brief The IntegerAnneal class implements
 * the simulated annealing algorithm for Grammatical Evolution problems.
 */
class IntegerAnneal
{
private:

    Program *myProblem;
    vector<int> xpoint;
    double ypoint;
    vector<int> bestx;
    double besty;
    int neps=100;
    double T0;
    int k;
public:
    /**
     * @brief IntegerAnneal The constructor of the class for Program pr.
     * @param pr
     */
    IntegerAnneal(Program *pr);
    /**
     * @brief setNeps Sets the number of samples
     * @param n
     */
    void    setNeps(int n);
    /**
     * @brief setT0 Sets the initial temperature.
     * @param t
     */
    void    setT0(double t);
    /**
     * @brief setPoint Sets the initial point of the algorithm.
     * @param g
     * @param y
     */
    void    setPoint(vector<int> &g,double &y);
    /**
     * @brief getPoint Returns the final point located by the algorithm.
     * @param g
     * @param y
     */
    void    getPoint(vector<int> &g,double &y);
    /**
     * @brief Solve Executes the main loop of the Simulated Annealing.
     */
    void    Solve();
    /**
     * @brief updateTemp Updates the temperature.
     */
    void    updateTemp();
    ~IntegerAnneal();
};
#endif // INTEGERANNEAL_H
