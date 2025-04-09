#ifndef SIMANMETHOD_H
#define SIMANMETHOD_H
# include <OPTIMUS/collection.h>
# include <OPTIMUS/optimizer.h>

/**
 * @brief The SimanMethod class implements the Simulated Annealing method.
 */
class SimanMethod : public Optimizer
{
private:
    double T0;
    Data xpoint;
    double ypoint;
    Data bestx;
    double besty;
    QString coolingMethod;
    int neps;
    double eps;
    int k;
    bool hasInitialized;
    void    updateTemp();
public:
    /**
     * @brief SimanMethod The constructor of Simulated Annealing.
     */
    SimanMethod();
    /**
     * @brief terminated
     * @return true when the optimizer should be terminated.
     */
    virtual bool terminated();
    /**
     * @brief step Executes a step of the SA method.
     */
    virtual void step();
    /**
     * @brief init Initializes the SA algorithm.
     */
    virtual void init();
    /**
     * @brief setPoint Sets the initial point of SA.
     * @param x
     * @param y
     */
    void    setPoint(Data &x,double &y);
    /**
     * @brief getPoint Returns the best point located by SA.
     * @param x
     * @param y
     */
    void    getPoint(Data &x,double &y);
    /**
     * @brief done It is executed when SA terminates.
     */
    virtual void done();
    /**
     * @brief showDebug Displays debug information.
     */
    virtual void showDebug();
    virtual ~SimanMethod();
};

#endif // SIMANMETHOD_H
