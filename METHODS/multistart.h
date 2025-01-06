#ifndef MULTISTART_H
#define MULTISTART_H
# include <OPTIMUS/problem.h>
# include <OPTIMUS/collection.h>
# include <OPTIMUS/optimizer.h>
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
    Multistart();
    /**
     * @brief init
     */
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void showDebug();
    virtual void done();
    /**
     * @brief getBest
     * @param x
     * @param y
     */
    void getBest(Data &x,double &y);
    virtual ~Multistart();
};

#endif // MULTISTART_H
