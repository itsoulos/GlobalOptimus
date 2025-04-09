#ifndef ADAM_H
#define ADAM_H

# include <OPTIMUS/optimizer.h>
# include <LINESEARCH/goldensearch.h>
# include <LINESEARCH/fibonaccisearch.h>
# include <LINESEARCH/armijosearch.h>
# include <math.h>
/**
 * @brief The Adam class implements the Adam local optimization method.
 */
class Adam : public Optimizer
{
private:
    Data xpoint;
    double ypoint;
    bool haveInitialized;
    double learningRate;
    int     iterations;
    Data M_0,U_0,rM,lM;
    double b1,b2;
    QString linesearch;
    LineSearch *lt;
public:
    /**
     * @brief Adam The constructor of the class.
     */
    Adam();
    /**
     * @brief init Initializes the variables of the method.
     */
    virtual void init();
    /**
     * @brief step Perform a step of the method.
     */
    virtual void step();
    /**
     * @brief terminated
     * @return true when the method should be terminated.
     */
    virtual bool terminated();
    /**
     * @brief updateRate Updates the search rate.
     */
    void    updateRate();
    /**
     * @brief setPoint Sets the initial point of optimizer.
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
    virtual ~Adam();
};

#endif // ADAM_H
