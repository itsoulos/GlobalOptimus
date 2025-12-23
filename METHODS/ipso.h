#ifndef IPSO_H
#define IPSO_H

# include <OPTIMUS/collection.h>
# include <OPTIMUS/optimizer.h>
class iPso : public Optimizer
{

private:
    double wmin ;
    double wmax ;
    int    maxGenerations ;
    double c1 ;
    double c2 ;
    bool ipso_isneural ;
    bool ipso_neuralsampling;
    QString t;


    int genome_size ;
    Data distances;

    double inertia = 0.5;
    bool centerPso;
    int lastImprovementGen;
   double  lastBesty ;
   vector<vector<double>> samples;

   int  restartCount ;
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
    // Απλός restart μηχανισμός
    bool restart_enabled ;
    int stall_generations;
    int max_grestarts;
     int max_generations;
    vector<Data> minimax;
    int generation, bestIndex;
    double x1, x2, stopat, variance, besty, worsty, oldbesty, oldworsty;
    Data lmargin, rmargin;
    double RC;
    int localSearchCount;
    QString mechStr ;
    int moveMode; // 0=standard, 1=turbulence, 2=spiral

    double turbulence_rate ;
    double turbulence_scale;
    double spiral_rate ;
    double spiral_scale ;
    double ipso_neuralw;

    double mean_range = 0.0;

    double fitness(Data &x);
    bool checkGradientCriterion(Data &x);
    double getDistance(Data &x, Data &y);
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
    virtual ~iPso();
};

#endif // IPSO_H
