#ifndef MULTISTART_H
#define MULTISTART_H
# include <OPTIMUS/problem.h>
# include <OPTIMUS/collection.h>
# include <OPTIMUS/optimizer.h>
/**
 * @brief The Multistart class implements the Multi Start global optimizer.
 */
class Multistart : public Optimizer
{
private:
    Collection minima;
    int iteration;
    int maxiters;
    Data bestx;
    double besty;
    int nsamples;
public:
    /**
     * @brief Multistart The constructor of multistart.
     */
    Multistart();
    /**
     * @brief init Initializes the samples.
     */
    virtual void init();
    /**
     * @brief step Performs a step of the multistart.
     */
    virtual void step();
    /**
     * @brief terminated
     * @return true, when the method should be terminated.
     */
    virtual bool terminated();
    /**
     * @brief showDebug Dislays debug information.
     */
    virtual void showDebug();
    /**
     * @brief done It is called when multistart finishes.
     */
    virtual void done();
    /**
     * @brief getBest Return the best located value.
     * @param x
     * @param y
     */
    void    getBest(Data &x,double &y);
    virtual ~Multistart();
};

#endif // MULTISTART_H
