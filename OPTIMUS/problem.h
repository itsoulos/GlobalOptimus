#ifndef PROBLEM_H
#define PROBLEM_H

#include <functional>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
# include <QJsonObject>
#include <vector>
#include <iostream>
using namespace std;
#define totcal_1 totcal_
#define units_1 units_
#define integer long
#define double double
//#define real float
typedef long int ftnlen;
typedef long int logical;
/**
 * @brief Data, a double precision vector
 */
typedef vector<double> Data;
/**
 * @brief Matrix, a double precision vector of two dimensions
 */
typedef vector<Data> Matrix;

/**
 * @brief The Problem class implements the base class that
 * describes optimization problems.
 */
class Problem
{
protected:
    double knownOptimumValue;
        Data knownOptimumLocation;
        bool hasKnownOptimum;
    /**
     * @brief dimension, the dimension of the target problem
     */
    int dimension;
    /**
     * @brief left, the left bounds of the problem
     */
    Data left;
    /**
     * @brief right, the right bounds of the problem
     */
    Data right;
    /**
     * @brief bestx, the best located point of the objective problemm
     */
    Data bestx;
    /**
     * @brief besty, the best located value (minimum) of the objective problem
     */
    double besty;
    /**
     * @brief functionCalls, the number of function calls used
     */
    int functionCalls;
public:

    /**
     * @brief Problem, the base constructor of the class
     * @param n, the problem dimension
     */
    Problem(int n);
    /**
     * @brief getDimension
     * @return the dimension of the problem
     */
    Problem();
    /**
     * @brief setDimension
     * @param n
     */
    void    setDimension(int n);
    /**
     * @brief getDimension
     * @return
     */
    int getDimension() const;
    /**
     * @brief getSample
     * @return a sample from the objective problem using uniform distribution
     */
    virtual Data getSample();
    /**
     * @brief setLeftMargin, changes the left bound of the objective problem
     * @param x
     */
    void setLeftMargin(Data &x);
    /**
     * @brief setRightMargin, changes the right bound of the objective problem
     * @param x
     */
    void setRightMargin(Data &x);
    /**
     * @brief getLeftMargin
     * @return the left bounds of the objective problem
     */
    Data getLeftMargin() const;
    /**
     * @brief getRightMargin
     * @return the right bounds of the objective problem
     */
    Data getRightMargin() const;
    /**
     * @brief funmin
     * @param x
     * @return  the value of the objective problem at point x
     */
    virtual double funmin(Data &x) = 0;
    /**
     * @brief gradient
     * @param x
     * @return
     */
    virtual Data gradient(Data &x) = 0;
    /**
     * @brief granal
     * @param x
     * @param g
     */
    virtual void    granal(Data &x,Data &g);
    /**
     * @brief statFunmin Kalei prota tin funmin(x)
     * kai diatirei to bestvalue kai kanei update
     * ta function calls
     * @param x
     */
    double statFunmin(Data &x);
    /**
     * @brief grms
     * @param x
     * @return the measured gradient mean square at point x
     */
    double grms(Data &x);
    /**
     * @brief getBestx
     * @return the best located point with the minimum function value
     */
    Data    getBestx() ;
    /**
     * @brief getBesty
     * @return the minimum function value
     */
    double  getBesty() const;
    /**
     * @brief getFunctionCalls
     * @return the number of function calls used
     */
    int     getFunctionCalls() const;
    /**
     * @brief isPointIn
     * @param x
     * @return true if point x lies inside the bounds of the objective problem
     */
    bool    isPointIn(Data &x);
    /**
     * @brief init, passes a series of parameters to the objective problem
     * @param params
     */
    virtual void init(QJsonObject &params);
    /**
     * @brief done
     * @param x
     * @return an object holding information after the minimization has been
     * completed
     */
    virtual QJsonObject done(Data &x);
    /**
     * @brief getleftmargin returns in x the left bound of the problem.
     * @param x
     */
    void    getleftmargin(double *x);
    /**
     * @brief getrightmargin returns in x the right bound of the problem.
     * @param x
     */
    void    getrightmargin(double *x);
    /**
     * @brief getDimension
     * @return the dimension of the objective problem.
     */
    int     getDimension();
    /**
     * @brief getdimension
     * @return the dimension of the objective problem.
     */
    int     getdimension();
    /**
     * @brief randomize initializes the random generator.
     * @param seed
     */
    void    randomize(int seed=1);
    /**
     * @brief randomDouble
     * @return a random double in range [0,1].
     */
    double  randomDouble();
    /**
     * @brief getGradientCriterion
     * @param x1
     * @param x2
     * @return true if the gradient criterion holds for x1,x2
     */
    bool     getGradientCriterion(Data &x1,Data &x2);
    /**
     * @brief resetFunctionCalls Reset the counter used for function calls.
     */
    void    resetFunctionCalls();
    /**
     * @brief setKnownOptimum This method is used in testing. Sets
     * the known location and value for the global minimum.
     * @param value
     * @param location
     */
    void    setKnownOptimum(double value, const Data& location);
    /**
     * @brief hasOptimum
     * @return true if the problem has a known global optimum.
     */
    bool    hasOptimum() const;
    /**
     * @brief getKnownOptimumValue
     * @return the known global minimum value.
     */
    double  getKnownOptimumValue() const;
    /**
     * @brief getKnownOptimumLocation
     * @return the known global minimum location.
     */
    Data    getKnownOptimumLocation() const;
    /**
     * @brief ~Problem
     */
    virtual ~Problem();
};



#endif // PROBLEM_H
