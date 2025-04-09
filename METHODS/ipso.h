#ifndef IPSO_H
#define IPSO_H

# include <OPTIMUS/collection.h>
# include <OPTIMUS/optimizer.h>

/**
 * @brief The iPso class implements the Improved PSO optimizer.
 */
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

    /**
     * @brief fitness
     * @param x
     * @return the fitness value of particle x.
     */
    double fitness(Data &x);
    /**
     * @brief checkGradientCriterion
     * @param x
     * @return Check if gradient criterion holds for particle x.
     */
    bool checkGradientCriterion(Data &x);
    /**
     * @brief calcFitnessArray Calculates the fitness values for all particles.
     */
    void calcFitnessArray();
    /**
     * @brief updateBest Updates the best particle.
     */
    void updateBest();
    /**
     * @brief updateCenter Updates the center particle (for center pso).
     */
    void updateCenter();
public:
    /**
     * @brief iPso The constructor of the class.
     */
    iPso();
    /**
     * @brief init Initializes the particles and velocities.
     */
    virtual void init();
    /**
     * @brief step Performs a step of the PSO.
     */
    virtual void step();
    /**
     * @brief terminated
     * @return true when PSO should be terminated.
     */
    virtual bool terminated();
    /**
     * @brief showDebug Displays debug information.
     */
    virtual void showDebug();
    /**
     * @brief done It is called when PSO finishes.
     */
    virtual void done();
    virtual ~iPso();
};

#endif // IPSO_H
