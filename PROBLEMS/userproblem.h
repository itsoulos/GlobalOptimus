#ifndef USERPROBLEM_H
#define USERPROBLEM_H

# include <OPTIMUS/problem.h>

/**
 * @brief The UserProblem class defines the user defined problem.
 * The user should implement all methods in this file and re -compile
 * the GlobalOptimus app. Afterwards he should select the UserProblem
 * as the main problem.
 */
class UserProblem : public Problem
{
public:
    /**
     * @brief UserProblem the default constructor
     */
    UserProblem();
    /**
     * @brief funmin computes the objective function
     * @param x is the input point
     * @return the value of function f(x)
     */
    double funmin(Data &x);
    /**
     * @brief gradient computes the gradient of the objective function
     * @param x is the input point
     * @return the gradinet g(x)
     */
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
    ~UserProblem();
};

#endif // USERPROBLEM_H
