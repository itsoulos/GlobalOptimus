#ifndef GRADIENTDESCENT_H
#define GRADIENTDESCENT_H
# include <OPTIMUS/optimizer.h>
# include <LINESEARCH/linesearch.h>
# include <LINESEARCH/fibonaccisearch.h>
# include <LINESEARCH/goldensearch.h>
# include <LINESEARCH/armijosearch.h>

/**
 * @brief The GradientDescent class implements the gradient descent local optimizer.
 */
class GradientDescent : public Optimizer
{
public:
    int maxiters;
    int iteration;
    double eps;
    double rate;
    Data xpoint;
    double ypoint;
    bool hasInitialized;
    LineSearch *lt;
    QString lineSearchMethod;//available values: none, armijo, golden, fibonacci
public:
    /**
     * @brief GradientDescent The constructor of the class.
     */
    GradientDescent();
    /**
     * @brief init Initializes the parameters of the method.
     */
    virtual void init();
    /**
     * @brief step Performs a step of the optimizer.
     */
    virtual void step();
    /**
     * @brief terminated
     * @return true when the optimizer should be finished.
     */
    virtual bool terminated();
    /**
     * @brief updaterate Updates the search rate.
     */
    void    updaterate();
    /**
     * @brief updatepoint Updates the current point.
     */
    void    updatepoint();
    /**
     * @brief showDebug Displays debug information.
     */
    virtual void showDebug();
    /**
     * @brief setPoint Sets the initial point.
     * @param x
     * @param y
     */
    void    setPoint(Data &x,double &y);
    /**
     * @brief getPoint Returns the located local minimum.
     * @param x
     * @param y
     */
    void    getPoint(Data &x,double &y);
    virtual ~GradientDescent();
};

#endif // GRADIENTDESCENT_H
