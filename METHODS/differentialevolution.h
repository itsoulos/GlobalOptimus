#ifndef DIFFERENTIALEVOLUTION_H
#define DIFFERENTIALEVOLUTION_H
# include <OPTIMUS/collection.h>
# include <OPTIMUS/optimizer.h>

class DifferentialEvolution : public Optimizer
{
private:
    Collection agent;
    int NP;
    double F;
    double CR;
    int maxiters;
    int iter;
    Data bestx;
    double besty;
    int tournament();
public:
    DifferentialEvolution();
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void showDebug();
    virtual void done();
    ~DifferentialEvolution();
};

#endif // DIFFERENTIALEVOLUTION_H
