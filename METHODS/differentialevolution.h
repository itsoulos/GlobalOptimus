#ifndef DIFFERENTIALEVOLUTION_H
#define DIFFERENTIALEVOLUTION_H
# include <OPTIMUS/collection.h>
# include <OPTIMUS/optimizer.h>

class DifferentialEvolution : public Optimizer
{
private:
    vector<Data> agentx;
    Data agenty;
    int NP;
    double F;
    double CR;
    int maxiters;
    int iter;
    Data bestx;
    double besty;
    int tournament();
    double sumFitness=0.0;
    int    sumItersCount = 0;
     vector<double> migrantWeights;
public:
    DifferentialEvolution();
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void showDebug();
    virtual void done();
    void calculateMigrantWeights();
    virtual ~DifferentialEvolution();

};

#endif // DIFFERENTIALEVOLUTION_H
