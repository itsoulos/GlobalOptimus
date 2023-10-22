#ifndef SIMANMETHOD_H
#define SIMANMETHOD_H
# include <OPTIMUS/collection.h>
# include <OPTIMUS/optimizer.h>

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
    void    updateTemp();
public:
    SimanMethod();
    virtual bool terminated();
    virtual void step();
    virtual void init();
    virtual void done();
    ~SimanMethod();
};

#endif // SIMANMETHOD_H
