#ifndef IPSO_H
#define IPSO_H

# include <OPTIMUS/collection.h>
# include <OPTIMUS/optimizer.h>
class iPso : public Optimizer
{

private:
    bool centerPso;
    int ipso_particles;
    double sum,newSum, besty_tmp,zeta;
    int n,sumn;
    int countn;
    Data center;
    vector<Data> particle;
    vector<Data> bestParticle;
    vector<Data> velocity;
    Data bestx, worstx;
    Data fitness_array;
    Data bestFitness_array;
    vector<Data> minimax;
    int generation, bestIndex;
    double x1, x2, stopat, variance, besty, worsty, oldbesty, oldworsty;
    Data lmargin, rmargin;
    double RC;
    int localSearchCount;

    double fitness(Data &x);
    bool checkGradientCriterion(Data &x);

    void calcFitnessArray();
    void updateBest();
    void updateCenter();

public:
    iPso();
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void showDebug();
    virtual void done();
    ~iPso();
};

#endif // IPSO_H
