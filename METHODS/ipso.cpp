#include "ipso.h"

iPso::iPso()
{
    centerPso = false;
    addParam(Parameter("ipso_particles", 100,10,10000, "Number of pso particles"));
    addParam(Parameter("ipso_generations", 100,10,1000, "Maximum number of pso generations"));
    addParam(Parameter("ipso_c1", 0.5, 0.1,2.0,"Pso c1 parameter"));
    addParam(Parameter("ipso_c2", 0.5,0.1,2.0, "Pso c2 parameter"));
    addParam(Parameter("ipso_inertia_start", 0.4,0.0,1.0, "Start value for inertia"));
    addParam(Parameter("ipso_inertia_end", 0.9,0.0,1.0, "End value for inertia"));
    addParam(Parameter("ipso_localsearch_rate", 0.1,0.0,1.0, "Local search rate for pso"));
    QStringList ipso_stoppingrule;
    ipso_stoppingrule<<"doublebox"<<"ali"<<"mean_fitness"<<"best_fitness";
    addParam(Parameter("ipso_stoppingrule",ipso_stoppingrule[0],
                       ipso_stoppingrule,
                       "Termination criterion (mean_fitness,best_fitness,doublebox,ali)"));
    QStringList ipso_gradientcheck;
    ipso_gradientcheck<<"true"<<"false";
    addParam(Parameter("ipso_gradientcheck",ipso_gradientcheck[0],
                       ipso_gradientcheck,
                       "Check for gradients near to local minimum"));
    addParam(Parameter("ipso_inertiatype",0,0,7,"The used inertia equation(0-7)"));
}

double  iPso::fitness(Data &x)
{
    if (!myProblem->isPointIn(x))
        return 1e+100;
    return myProblem->statFunmin(x);
}

double getDistance(Data &x1,Data &x2)
{
    double sum=0.0;
    for(int i=0;i<x1.size();i++)
        sum+=(x1[i]-x2[i])*(x1[i]-x2[i]);
    return sqrt(sum);
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

void    iPso::calcFitnessArray()
{
    double wmin = getParam("ipso_inertia_start").getValue().toDouble();
    double wmax = getParam("ipso_inertia_end").getValue().toDouble();
    int maxGenerations = getParam("ipso_generations").getValue().toInt();
    double c1 = getParam("ipso_c1").getValue().toDouble();
    double c2 = getParam("ipso_c2").getValue().toDouble();
    int genome_size = myProblem->getDimension();
    Data distances;

    double inertia;
    int inertia_type = 3;
    QString t = getParam("ipso_inertiatype").getValue();
    inertia_type=t.toInt();

    // inecria weight => εάν θα διατηρηθεί η ταχύτητα
    switch ( inertia_type )//
    {
    case 0:
    {
        double R = myProblem->randomDouble();
        inertia = fabs((1.0 / (4.0 +(R/2.0))));             //charilogis
        //printf("inertia: %10.6lf \n", inertia);
        break;
    }
    case 1:
    {
        inertia = wmax - generation * 1.0 / 15 * (wmax - wmin);
        //printf("inertia: %10.6lf \n", inertia);
        break;
    }
    case 2:
    {
        if(countn==0) countn=1;
        double average_n=countn==0?0:sumn*1.0/countn;
        if(average_n<=0) average_n=15.0;
        inertia = wmax - generation * 1.0 / average_n * (wmax - wmin);
        //printf("inertia: %10.6lf \n", inertia);
        break;
    }
    case 3:
    {
        inertia = fabs((1.0 / (2.0 - besty)));             //charilogis
        //printf("inertia: %10.6lf \n", inertia);
        break;
    }
    case 4:                                                //3 (w2)
    {
        double R = myProblem->randomDouble();
        inertia = 0.5 +(R/2.0);
        //printf("inertia: %10.6lf \n", inertia);
        break;
    }
    case 5:                                                //4 (w3)
    {
        double g = (double)(maxGenerations - generation) / maxGenerations;
        inertia = (g * (wmax - wmin)) + wmin;
        //printf("inertia: %10.6lf \n", inertia);
        break;
    }
    case 6:                                                //5 (w8)
    {
        inertia = pow ((2.0 / generation), 0.3);
        break;
    }
    case 7:                                                //6 (w9)
    {
        double g = (double)(15 - generation) / 15;
        inertia = (g * (wmin - wmax)) + wmax;
        //printf("inertia: %10.6lf \n", inertia);
        break;
    }
    case 8:
    {
        double g = (double)(maxGenerations - generation) / maxGenerations;
        inertia = (g * (wmin - wmax)) + wmax;
        //printf("inertia: %10.6lf \n", inertia);
        break;
    }
    case 9:                                                //7 (w7)
    {
        double b = (double)(generation) / maxGenerations;
        double s = -0.5 ;  //(σταθερά > -1)
        inertia = (1.0-b)/s*(1.0 - b);
        //printf("inertia: %10.6lf \n", inertia);
        break;
    }
    case 10:                                                //8 (w12)
    {
        inertia = wmax - generation * 1.0 / maxGenerations * (wmax - wmin);
        //printf("inertia: %10.6lf \n", inertia);
        break;
    }
    case 11:							//(w6)
    {
        inertia = 2.0 * fabs(besty_tmp - besty);

        if(generation<=1) zeta = myProblem->randomDouble();
        else zeta = 4.0 *zeta*(1.0-zeta);
        inertia = (maxGenerations-generation)*1.0/maxGenerations*(wmax-wmin)+zeta*wmin;
        //printf("besty_tmp: %10.6lf besty %10.6lf inertia: %10.6lf \n", besty_tmp, besty,inertia);
        break;
    }

    default:
    {
        //inertia = 0;
    }
    }

    double localsearch_rate = getParam("ipso_localsearch_rate").getValue().toDouble();
    for (int i = 0; i < ipso_particles; i++)
    {
        double R = myProblem->randomDouble();
        if (inertia_type == 0)
            inertia = (fabs(fitness_array[i]) - fabs(worsty))/(fabs(besty) - fabs(worsty));
        //double m = fabs((fitness_array[i] - worsty)/(besty - worsty));

        Data oldg = particle[i]; //αποθήκευση παλιάς θέσης
        for (int j = 0; j < genome_size; j++)
        {
            double r1 = myProblem->randomDouble();
            double r2 = myProblem->randomDouble();


            double tj = velocity[i][j];              //αποθήκευση παλιάς ταχύτητας
            double part1 = inertia * velocity[i][j]; //πολλαπλασιαστής ταχύτητας(inecria weight)
            double part2 = c1 * r1 * (bestParticle[i][j] - particle[i][j]);
            double part3 = c2 * r2 * (bestx[j] - particle[i][j]);
            velocity[i][j] = part1 + part2 + part3;          // υπολογισμός ταχύτητας
            /*
            const double gamma=0.5; //proposed by Ali
            double umax=gamma * (rmargin[j]-lmargin[j]);
            double umin=-umax;
            if(velocity[i][j]<umin) velocity[i][j]=umin;
            if(velocity[i][j]>umax) velocity[i][j]=umax;
*/
            double trialf = particle[i][j] + velocity[i][j]; //υπολογισμός θέσης
            if (trialf < lmargin[j] || trialf > rmargin[j])
            {
                velocity[i][j] = tj;
                continue;
            }
            else
            {
                particle[i][j] = trialf; // καινούργια θέση
            }
        }
        distances.push_back(getDistance(particle[i], oldg)); // αποθήκευση της απόστασης => παλιά με καινούργια
    }

    double oldMinValue=1e+100;
    for (int i = 0; i < ipso_particles; i++)
    {
        if (distances[i] > 1e-6)
        {
            if (localsearch_rate > 0.0 && myProblem->randomDouble() <= localsearch_rate 
			    &&  !checkGradientCriterion(particle[i]))
            {
                Data dg = particle[i];
                fitness_array[i] = localSearch(particle[i]);
                RC += getDistance(dg, particle[i]);
                localSearchCount++;
                {
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
                    {
                        minimax.push_back(particle[i]);
                    }
                    minimax.push_back(dg);
                }
            }
            else
            {
                int imin=-1;
                double dmin=1e+100;
                for(int j=0;j<minimax.size();j++)
                {
                    if(getDistance(minimax[j],particle[i])<dmin)
                    {
                        dmin=getDistance(minimax[j],particle[i]);
                        imin = j;
                    }
                }
                if(imin!=-1)
                {
                    RC+=getDistance(minimax[imin],particle[i]);
                    localSearchCount++;
                }
                fitness_array[i] = fitness(particle[i]);
            }
            if(fitness_array[i]<oldMinValue) oldMinValue = fitness_array[i];
        }
    }
}
void	iPso::updateCenter()
{

    for(int j=0;j<myProblem->getDimension();j++) center[j]=0.0;
    for(int i=0;i<ipso_particles;i++)
    {
        for(int j=0;j<myProblem->getDimension();j++)
        {
            center[j]+=particle[i][j];
        }
    }
    for(int j=0;j<myProblem->getDimension();j++)
    {
        center[j]/=ipso_particles;
        if(center[j]<lmargin[j]) center[j]=lmargin[j];
        if(center[j]>rmargin[j]) center[j]=rmargin[j];
    }
}

void    iPso::updateBest()
{

    int mod = 10;

    for (int i = 0; i < ipso_particles; i++)
    {

        /*
        //if( fabs(fitness_array[i]-bestfitness_array[i])>1e-5  && rand()%mod==0)
        if( rand()%mod==0)
        {
            int imin=-1;
            double dmin=1e+100;
            for(int j=0;j<minimax.size();j++)
            {
                double d=getDistance(genome[i],minimax[j]);
                if(d<dmin)
                {
                    imin=j;
                    dmin=d;
                }
            }
            if(imin>=0 && dmin<RC/localSearchCount && problem->getGradientCriterion(genome[i],
                        minimax[imin]))
                ;
            else{

                bool notFoundBetter=true;

                if(notFoundBetter)
                {

            Data gg=genome[i];
            bool found=false;
            localSearch(genome[i],fitness_array[i]);
            for(int j=0;j<minimax.size();j++)
            {
                if(getDistance(genome[i],minimax[j])<1e-5) {found=true;break;}
            }
            if(!found)
            minimax.push_back(genome[i]);
            minimax.push_back(gg);
            RC+=getDistance(gg,genome[i]);
            localSearchCount++;
                }
                }
        }*/
        if (fitness_array[i] < bestFitness_array[i])
        {
            bestFitness_array[i] = fitness_array[i];
            bestParticle[i] = particle[i];
        }
        besty_tmp = besty;
        if (besty > bestFitness_array[i])
        {
            bestIndex = i;
            bestx = bestParticle[i];
            besty = bestFitness_array[i];
        }


    }
    if(centerPso)
    {
        double centerValue = myProblem->statFunmin(center);
        if(centerValue<besty)
        {
            besty = centerValue;
            bestx = center;
        }
    }
    worsty = *max_element (bestFitness_array.begin(), bestFitness_array.end());
    newSum = accumulate(bestFitness_array.begin(), bestFitness_array.end(), 0);
    newSum = newSum / ipso_particles;
}



void    iPso::init()
{
    ipso_particles = getParam("ipso_particles").getValue().toInt();
    center.resize(myProblem->getDimension());
    particle.resize(ipso_particles);

    bestx.resize(myProblem->getDimension());
    lmargin = myProblem->getLeftMargin();
    rmargin = myProblem->getRightMargin();

    countn=0;
    generation = 0;
    besty = worsty = oldbesty = oldworsty = 1e+100;

    x1 = 0.0;
    x2 = 0.0;
    stopat = 0.0;
    variance = 0.0;
    RC = 0.0;
    localSearchCount = 0;
    minimax.clear();
    sumn=0;

    sampleFromProblem(ipso_particles,particle,fitness_array);

    bestParticle.resize(ipso_particles);
    velocity.resize(ipso_particles);
    fitness_array.resize(ipso_particles);
    bestFitness_array.resize(ipso_particles);
    for (int i = 0; i < ipso_particles; i++)
    {
        bestParticle[i].resize(myProblem->getDimension());
        velocity[i].resize(myProblem->getDimension());
        bestParticle[i] = particle[i];
        bestFitness_array[i] = fitness_array[i];

        for (int j = 0; j < myProblem->getDimension(); j++)
        {
            double left = -(rmargin[j] - lmargin[j]) / 20.0;
            double right = (rmargin[j] - lmargin[j]) / 20.0;
            velocity[i][j] = left + myProblem->randomDouble() * (right - left);
            //velocity[i][j] = (left + myProblem->randomDouble() * (right - left))* myProblem->getDimension();
            //velocity[i][j] = myProblem->getDimension()/(fabs(lmargin[j])+fabs(rmargin[j]));
            //velocity[i][j] = myProblem->getDimension() * fabs(oldbesty - besty);
            //velocity[i][j] = (fabs(lmargin[j])+fabs(rmargin[j])/myProblem->getDimension());
            //velocity[i][j] = left + (myProblem->getDimension()/100.0) * (right - left);
            //velocity[i][j] = 0;
        }
    }
    updateBest();
    if(centerPso)
        updateCenter();
    sum = accumulate(bestFitness_array.begin(), bestFitness_array.end(), 0);
    if (sum == 0)
    {
        // assert(sum > 0);
    }
    sum = sum / ipso_particles;
    // printf("\n   0] Generation  start : %10.6lf \n", sum);
    n = 0;
    besty_tmp = 1e+100;
}

void    iPso::step()
{
    ++generation;
    calcFitnessArray();
    if(centerPso)
        updateCenter();;
    updateBest();
}

bool    iPso::terminated()
{
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
