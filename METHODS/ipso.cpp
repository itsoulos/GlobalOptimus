#include "ipso.h"
#include <MLMODELS/mlpproblem.h>
# include <MLMODELS/rbfproblem.h>

static inline bool isBad(double v)
{
    return !std::isfinite(v);
}

iPso::iPso()
{
    centerPso = false;


    addParam(Parameter("ipso_particles",      100, 10, 10000, "Number of pso particles"));
    addParam(Parameter("ipso_generations",    100, 10, 1000,  "Maximum number of pso generations"));
    addParam(Parameter("ipso_c1",            0.5,  0.1,  2.0, "Pso c1 parameter"));
    addParam(Parameter("ipso_c2",            0.5,  0.1,  2.0, "Pso c2 parameter"));
    addParam(Parameter("ipso_inertia_start", 0.4,  0.0,  1.0, "Start value for inertia"));
    addParam(Parameter("ipso_inertia_end",   0.9,  0.0,  1.0, "End value for inertia"));

    addParam(Parameter("ipso_localsearch_rate", 0.1, 0.0, 1.0, "Local search rate for pso"));

    // Neural parameters
    QStringList isNeural;
    isNeural << "no" << "yes";
    addParam(Parameter("ipso_isneural",      isNeural[0], isNeural, "Select yes for mlp training"));
    addParam(Parameter("ipso_neuralw",       2.0, -1.0, 100.0,
                       "Bound limit for mlp training. Set -1 for adaptive."));
    addParam(Parameter("ipso_neuralsampling",isNeural[0], isNeural,
                       "Set to yes to use neural sampling method"));

    QStringList gradCheck;
    gradCheck << "false" << "true";
    addParam(Parameter("ipso_gradientcheck", gradCheck[0], gradCheck,
                       "Enable gradient-based redundancy check"));

    addParam(Parameter("ipso_lmargin", 1e-3, 0.0, 1e+6,
                       "Left distance margin for gradient criterion"));
    addParam(Parameter("ipso_rmargin", 1e+3, 0.0, 1e+9,
                       "Right distance margin for gradient criterion"));

    // Stopping rule
    QStringList ipso_stoppingrule;
    ipso_stoppingrule << "doublebox" << "similarity";
    addParam(Parameter("ipso_stoppingrule", ipso_stoppingrule[0],
                       ipso_stoppingrule,
                       "Select stopping rule. values: doublebox, similarity"));


    QStringList yesno;
   yesno << "yes" << "no";
    addParam(Parameter("ipso_restart", yesno [1], yesno, "Enable PSO restart"));
    addParam(Parameter("ipso_restart_stall", 50, 1, 100000, "Generations without improvement before restart"));

    addParam(Parameter("ipso_restart_max",3, 0, 100, "Maximum number of restarts"));

    // Movement mechanisms
    QStringList ipso_movemechanism;
    ipso_movemechanism << "standard" << "turbulence" << "spiral";
    addParam(Parameter("ipso_movemechanism", ipso_movemechanism[0],
                       ipso_movemechanism,
                       "Select movement mechanism: standard, turbulence or spiral"));

    addParam(Parameter("ipso_turbulence_rate",  0.2, 0.0, 1.0,
                       "Probability to apply turbulence move when swarm is stuck"));
    addParam(Parameter("ipso_turbulence_scale", 0.1, 0.0, 1.0,
                       "Relative step size for turbulence move (fraction of search range)"));

    addParam(Parameter("ipso_spiral_rate",      0.3, 0.0, 1.0,
                       "Probability to apply spiral move around global best"));
    addParam(Parameter("ipso_spiral_scale",     0.2, 0.0, 1.0,
                       "Strength of spiral contraction/expansion around best"));

    generation          = 0;
    ipso_particles      = 0;
    besty = worsty      = 1e+100;
    oldbesty = oldworsty= 1e+100;
    x1 = x2 = 0.0;
    stopat = variance = 0.0;
    RC = 0.0;
    localSearchCount = 0;
    sumn = 0.0;
    sum  = 0.0;
    lastImprovementGen = 0;
    lastBesty = 1e+100;
    restartCount = 0;
}
void iPso::init()
{
    ipso_particles = getParam("ipso_particles").getValue().toInt();
    // Απλός restart μηχανισμός
     restart_enabled = (getParam("ipso_restart").getValue() == "yes");
    stall_generations = (getParam("ipso_restart_stall").getValue().toInt());
    max_grestarts = (getParam("ipso_restart_max").getValue().toInt());
     max_generations = getParam("ipso_generations").getValue().toInt();
     wmin = getParam("ipso_inertia_start").getValue().toDouble();
     wmax = getParam("ipso_inertia_end").getValue().toDouble();
     maxGenerations = getParam("ipso_generations").getValue().toInt();
     c1 = getParam("ipso_c1").getValue().toDouble();
     c2 = getParam("ipso_c2").getValue().toDouble();
     t = getParam("ipso_stoppingrule").getValue();
     ipso_isneural      = (getParam("ipso_isneural").getValue() == "yes");
     ipso_neuralsampling= (getParam("ipso_neuralsampling").getValue() == "yes");
     ipso_neuralw = getParam("ipso_neuralw").getValue().toDouble();
     mechStr = getParam("ipso_movemechanism").getValue();
      moveMode = 0; // 0=standard, 1=turbulence, 2=spiral
     if (mechStr == "turbulence") moveMode = 1;
     else if (mechStr == "spiral") moveMode = 2;

      turbulence_rate  = getParam("ipso_turbulence_rate").getValue().toDouble();
     turbulence_scale = getParam("ipso_turbulence_scale").getValue().toDouble();
     spiral_rate      = getParam("ipso_spiral_rate").getValue().toDouble();
     spiral_scale     = getParam("ipso_spiral_scale").getValue().toDouble();


     mean_range = 0.0;
    center.resize(myProblem->getDimension());
    particle.resize(ipso_particles);

    bestx.resize(myProblem->getDimension());
    lmargin = myProblem->getLeftMargin();
    rmargin = myProblem->getRightMargin();

    generation = 0;
    besty = worsty = oldbesty = oldworsty = 1e+100;

    x1 = x2 = 0.0;
    stopat = variance = 0.0;
    RC = 0.0;
    localSearchCount = 0;
    minimax.clear();
    sumn = 0.0;
    sum  = 0.0;
    lastImprovementGen = 0;
    lastBesty = 1e+100;
    restartCount = 0;
    fitness_array.resize(ipso_particles, 1e+100);

    if (!ipso_isneural || (ipso_isneural && !ipso_neuralsampling))
    {
        sampleFromProblem(ipso_particles, particle, fitness_array);
    }
    else
    {
        MlpProblem *neural = dynamic_cast<MlpProblem*>(myProblem);
        if (neural)
        {
            for (int i = 0; i < ipso_particles; i++)
            {
                Data x = neural->getSampleNoViolate();
                double y = neural->funmin(x);
                particle[i]      = x;
                fitness_array[i] = y;
            }
        }
        else
        {
            sampleFromProblem(ipso_particles, particle, fitness_array);
        }
    }

    bestParticle.resize(ipso_particles);
    velocity.resize(ipso_particles);
    bestFitness_array.resize(ipso_particles, 1e+100);

    for (int i = 0; i < ipso_particles; i++)
    {
        bestParticle[i].resize(myProblem->getDimension());
        velocity[i].resize(myProblem->getDimension());
        bestParticle[i]      = particle[i];
        bestFitness_array[i] = fitness_array[i];

        for (int j = 0; j < myProblem->getDimension(); j++)
        {
            double left  = -(rmargin[j] - lmargin[j]) / 20.0;
            double right =  (rmargin[j] - lmargin[j]) / 20.0;
            velocity[i][j] = left + myProblem->randomDouble() * (right - left);
        }
    }

    updateBest();
    if (centerPso)
        updateCenter();

    sum = std::accumulate(bestFitness_array.begin(),
                          bestFitness_array.end(), 0.0);
    oldworsty = worsty;
    oldbesty  = besty;
}

double iPso::fitness(Data &x)
{
    if (!myProblem->isPointIn(x))
        return 1e+100;
    return myProblem->statFunmin(x);
}

double iPso::getDistance(Data &x, Data &y)
{
    double r = 0.0;

    for (int i = 0; i < x.size(); i++)
    {
        double z = x[i] - y[i];
        r += z * z;
    }
    return std::sqrt(r);
}

bool    iPso::checkGradientCriterion(Data &x)
{

    QString t = getParam("ipso_gradientcheck").getValue();
    if(t=="false") return false;
    double dmin = 1e+100;
    int imin = 0;
    if (minimax.size() > 0)
    {
        dmin = getDistance(minimax[0], x);
        for (int j = 0; j < minimax.size(); j++)
        {
            double d = getDistance(minimax[j], x);
            if (d < dmin)
            {
                imin = j;
                dmin = d;
            }
        }
    }
    else
        return false;
    if (dmin < 1e-6 || (dmin < RC / localSearchCount && myProblem->getGradientCriterion(x, minimax[imin])))
        return true;
    return false;
}
void iPso::updateCenter()
{
    if (center.size() != myProblem->getDimension())
        center.resize(myProblem->getDimension());

    for (int j = 0; j < center.size(); j++)
        center[j] = 0.0;

    if (bestParticle.empty())
        return;

    for (int i = 0; i < bestParticle.size(); i++)
        for (int j = 0; j < center.size(); j++)
            center[j] += bestParticle[i][j];

    for (int j = 0; j < center.size(); j++)
        center[j] /= bestParticle.size();
}

void iPso::updateBest()
{
    besty  = 1e+100;
    worsty = -1e+100;

    for (int i = 0; i < ipso_particles; i++)
    {
        double f = fitness_array[i];
        if (f < besty)
        {
            besty = f;
            bestx = particle[i];
        }
        if (f > worsty)
            worsty = f;

        if (i >= (int)bestFitness_array.size())
            continue;

        if (f < bestFitness_array[i])
        {
            bestFitness_array[i] = f;
            bestParticle[i]      = particle[i];
        }
    }

    if (besty + 1e-12 < lastBesty)
    {
        lastBesty = besty;
        lastImprovementGen = generation;
    }
}

void iPso::calcFitnessArray()
{

    genome_size = myProblem->getDimension();

    inertia = 0.5;
    QString tIn = getParam("ipso_inertiatype").getValue();
    int inertia_type = tIn.toInt();

    double g = (double)generation / std::max(1, maxGenerations);

    switch (inertia_type)
    {
    case 0:
    {
        double R = myProblem->randomDouble();
        inertia = std::fabs(1.0 / (4.0 + (R / 2.0)));
        break;
    }
    case 1:
        inertia = (wmax - wmin) * std::exp(10.0 * g - std::exp(10.0 * g)) + wmin;
        break;
    case 2:
        inertia = 0.9 - ((double)generation) * (0.5 / (double)maxGenerations);
        break;
    case 3:
        inertia = 0.5 * myProblem->randomDouble();
        break;
    case 4:
        inertia = myProblem->randomDouble();
        break;
    case 5:
        inertia = 1.0 / (1.0 + 1.5 * std::exp(-2.6 * g));
        break;
    case 6:
        inertia = wmin + (wmax - wmin) *
                  (1.0 + std::exp(5.0 - (5 * std::pow((2 * g), 2))));
        break;
    case 7:
    {
        double s = -0.5;
        inertia = (1.0 - s) / s * (1.0 - s);
        break;
    }
    case 10:
        inertia = wmax - generation * 1.0 / maxGenerations * (wmax - wmin);
        break;
    case 11:
    {
        double b = g;
        inertia = (wmax - wmin) * (0.5 + myProblem->randomDouble())
                  * std::fabs(std::cos(3.14 * b)) + wmin;
        break;
    }
    case 12:
    {
        double s = (10.0 / std::max(1, maxGenerations)) + 0.001;
        inertia = 0.5 + (0.5 - (2.0 - generation) * s) /
                         std::sqrt(1.0 + std::pow(2.0 - generation, 2) * s);
        break;
    }
    case 13:
        inertia = 0.1 + 0.5 * (myProblem->randomDouble() + 0.5) * std::exp(-2.0 * g);
        break;
    default:
        inertia = 0.7;
        break;
    }


    for (int j = 0; j < genome_size; ++j)
        mean_range += (rmargin[j] - lmargin[j]);
    if (genome_size > 0)
        mean_range /= genome_size;

    std::vector<double> vmax(genome_size);
    for (int j = 0; j < genome_size; ++j)
    {
        double range = rmargin[j] - lmargin[j];
        vmax[j] = 0.2 * range;
    }

    distances.clear();
    distances.reserve(ipso_particles);

    // PSO move + turbulence/spiral
    for (int i = 0; i < ipso_particles; i++)
    {
        Data oldg = particle[i];

        for (int j = 0; j < genome_size; j++)
        {
            double r1 = myProblem->randomDouble();
            double r2 = myProblem->randomDouble();

            double part1 = inertia * velocity[i][j];
            double part2 = c1 * r1 * (bestParticle[i][j] - particle[i][j]);
            double part3 = c2 * r2 * (bestx[j] - particle[i][j]);

            double vnew = part1 + part2 + part3;

            if (vnew >  vmax[j]) vnew =  vmax[j];
            if (vnew < -vmax[j]) vnew = -vmax[j];

            velocity[i][j] = vnew;

            double trialf = particle[i][j] + velocity[i][j];

            if (trialf < lmargin[j])
            {
                trialf = lmargin[j];
                velocity[i][j] = 0.0;
            }
            else if (trialf > rmargin[j])
            {
                trialf = rmargin[j];
                velocity[i][j] = 0.0;
            }

            particle[i][j] = trialf;
        }

        double moveDist = getDistance(particle[i], oldg);
        distances.push_back(moveDist);

        if (mean_range > 0.0)
        {
            if (moveMode == 1)
            {
                double dist_to_best = getDistance(particle[i], bestx);
                double stuck_threshold = 0.05 * mean_range;

                if (dist_to_best < stuck_threshold &&
                    myProblem->randomDouble() < turbulence_rate)
                {
                    for (int j = 0; j < genome_size; ++j)
                    {
                        double range = rmargin[j] - lmargin[j];
                        double step = turbulence_scale * range *
                                      (myProblem->randomDouble() - 0.5);

                        double xnew = particle[i][j] + step;
                        if (xnew < lmargin[j]) xnew = lmargin[j];
                        if (xnew > rmargin[j]) xnew = rmargin[j];

                        particle[i][j] = xnew;
                    }
                }
            }

            if (moveMode == 2 &&
                myProblem->randomDouble() < spiral_rate)
            {
                for (int j = 0; j < genome_size; ++j)
                {
                    double range = rmargin[j] - lmargin[j];
                    double dx = particle[i][j] - bestx[j];

                    double contraction = 1.0 - spiral_scale;
                    double noise = (myProblem->randomDouble() - 0.5) *
                                   range * spiral_scale;

                    double xnew = bestx[j] + contraction * dx + noise;
                    if (xnew < lmargin[j]) xnew = lmargin[j];
                    if (xnew > rmargin[j]) xnew = rmargin[j];

                    particle[i][j] = xnew;
                }
            }
        }

        for (int j = 0; j < genome_size; ++j)
        {
            if (isBad(particle[i][j]))
            {
                particle[i][j] = lmargin[j] +
                                 myProblem->randomDouble() * (rmargin[j] - lmargin[j]);
                velocity[i][j] = 0.0;
            }
        }
    }

    double localsearch_rate = getParam("ipso_localsearch_rate").getValue().toDouble();
    Data xl(genome_size), xr(genome_size);

    MlpProblem *neural = nullptr;
    if (ipso_isneural)
        neural = dynamic_cast<MlpProblem*>(myProblem);

    double oldMinValue = 1e+100;

    if ((int)fitness_array.size() != ipso_particles)
        fitness_array.resize(ipso_particles, 1e+100);

    for (int i = 0; i < ipso_particles; i++)
    {
        if (distances[i] > 1e-8 &&
            localsearch_rate > 0.0 &&
            myProblem->randomDouble() <= localsearch_rate &&
            !checkGradientCriterion(particle[i]))
        {
            Data dg = particle[i];

            if (!ipso_isneural)
            {
                fitness_array[i] = localSearch(particle[i]);
            }
            else
            {
                double fv = ipso_neuralw;

                    for (int k = 0; k < myProblem->getDimension(); k++)
                    {
                        xl[k] = -fv * std::fabs(particle[i][k]);
                        xr[k] =  fv * std::fabs(particle[i][k]);
                    }

                    if (ipso_neuralw < 0)
                    {
                        for (int k = 0; k <  myProblem->getDimension(); k++)
                        {
                            xl[k] = -2.0 * std::fabs(particle[i][k]);
                            xr[k] =  2.0 * std::fabs(particle[i][k]);
                        }
                       MlpProblem *neural = dynamic_cast<MlpProblem*>(myProblem);
                    //   if (neural)
                    //       neural->findBoundsWithSiman(xl, xr, particle[i]);
                        for (int k = 0; k <  myProblem->getDimension(); k++)
                        {
                            if (particle[i][k] < xl[k])
                                xl[k] = particle[i][k];
                            if (particle[i][k] > xr[k])
                                xr[k] = particle[i][k];
                        }


                    for (int k = 0; k <  myProblem->getDimension(); k++)
                    {
                        if (xl[k] < lmargin[k]) xl[k] = lmargin[k];
                        if (xr[k] > rmargin[k]) xr[k] = rmargin[k];
                    }

                    Data oldL = myProblem->getLeftMargin();
                    Data oldR = myProblem->getRightMargin();
                    myProblem->setLeftMargin(xl);
                    myProblem->setRightMargin(xr);

                    double oldf = myProblem->funmin(particle[i]);
                    if (!std::isfinite(oldf)|| std::fabs(oldf)>= 1e+10)
                    {
                        myProblem->setLeftMargin(oldL);
                        myProblem->setRightMargin(oldR);
                        continue;
                    }
                    fitness_array[i] = localSearch(particle[i]);

                    myProblem->setLeftMargin(oldL);
                    myProblem->setRightMargin(oldR);
                }

            }

            RC += getDistance(dg, particle[i]);
            localSearchCount++;

            bool found = false;
            for (int j = 0; j < minimax.size(); j++)
            {
                if (getDistance(minimax[j], particle[i]) < 1e-5)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                minimax.push_back(particle[i]);
        }

        if (fitness_array[i] >= 1e+99 || !std::isfinite(fitness_array[i]))
            fitness_array[i] = fitness(particle[i]);

        if (isBad(fitness_array[i]))
            fitness_array[i] = 1e+10;

        if (fitness_array[i] < oldMinValue)
            oldMinValue = fitness_array[i];
    }
}


void iPso::step()
{
    ++generation;
    calcFitnessArray();
    if (centerPso)
        updateCenter();
    updateBest();

            if (restart_enabled && generation - lastImprovementGen > stall_generations &&
                restartCount < max_grestarts)
    {
        Data   best_keep    = bestx;
        double best_keep_f  = besty;

        for (int i = 0; i < ipso_particles; i++)
        {
            if (i == 0)
            {
                particle[i]      = best_keep;
                fitness_array[i] = best_keep_f;
                continue;
            }
            myProblem->getSample();
            fitness_array[i] = fitness(particle[i]);
        }

        restartCount++;
        lastImprovementGen = generation;
        lastBesty = besty;

        for (int i = 0; i < ipso_particles; i++)
        {
            bestParticle[i]      = particle[i];
            bestFitness_array[i] = fitness_array[i];
        }
    }
}

bool iPso::terminated()
{



    // Neural termination
    if (ipso_isneural && std::fabs(besty) < 1e-6)
        return true;

        int max_generations = getParam("ipso_generations").getValue().toInt();

        if(generation>=max_generations) return true;
        bool charilogis = false;
        bool charilogis2 = false;
        bool aliflag=false;
        QString t = getParam("ipso_stoppingrule").getValue();
        if(t=="mean_fitness") charilogis=true;
        else if(t=="best_fitness") charilogis2=true;
        else if(t=="ali") aliflag=true;

        if (charilogis)
        {
            double dd = fabs(newSum - sum);
            //printf("%4d] Generation  change: %10.6lf \n", generation, dd);
            sum = newSum;

            if (dd < 1e-8)
            {
                n++;
                sumn+=1;
            }
            else
            {
                n = 0;
                countn++;
            }
            if ( n > 5)
                return true;

            return generation >= max_generations;
        }
        else
            if (charilogis2){
                if (fabs(besty-besty_tmp)<1e-6)
                {
                    n++;
                    sumn+=1;
                }
                else
                {
                    n = 0;
                    countn++;
                }
                if (n > 5)
                    return true;
                return generation >= max_generations;
            }else
                if(aliflag)
                {
                    double fmin,fmax;
                    for(int i=0;i<fitness_array.size();i++)
                    {
                        if(i==0 || fitness_array[i]<fmin) fmin=fitness_array[i];
                        if(i==0 || fitness_array[i]>fmax) fmax=fitness_array[i];
                    }
                    return fabs(fmax-fmin)<1e-3  || generation>=max_generations;
                }
                else
                {
                    return doubleBox.terminate(besty);
                }
    }



void    iPso::showDebug()
{
    if(generation%10==0 && getParam("opt_debug").getValue()=="yes")
        methodLogger->printMessage(
            QString::asprintf("IPSO. Iteration=%4d besty = %10.5lg ",
                              generation,
                              besty));
}

void    iPso::done()
{
 besty = localSearch(bestx);
}

iPso::~iPso()
{

}
