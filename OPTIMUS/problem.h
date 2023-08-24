#ifndef PROBLEM_H
#define PROBLEM_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
# include <QJsonObject>
#include <vector>
#include <iostream>
using namespace std;

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
    void    setDimension(int n);
    int getDimension() const;
    /**
     * @brief getSample
     * @return a sample from the objective problem using uniform distribution
     */
    Data getSample();
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
    Data    getBestx() const;
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
    void    getleftmargin(double *x);
    void    getrightmargin(double *x);
    ~Problem();
};



#endif // PROBLEM_H
