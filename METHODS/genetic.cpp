#include "genetic.h"
Genetic::Genetic()
{
    hasInitialized =false;
    addParam(Parameter("gen_count",200,10,2000,"Number of chromosomes"));
    addParam(Parameter("gen_maxiters",200,10,2000,"Maximum number of generations"));
    addParam(Parameter("gen_srate",0.10,0.0,1.0,"Selection rate"));
    addParam(Parameter("gen_mrate",0.05,0.0,1.0,"Mutation rate"));
    addParam(Parameter("gen_lrate",0.005,0.0,1.0,"Localsearch rate"));
    addParam(Parameter("gen_tsize",8,2,20,"Tournament size"));
    QStringList gen_selection;
    gen_selection<<"roulette"<<"tournament";
    addParam(Parameter("gen_selection",gen_selection[0],
                       gen_selection,
                       "Selection method. Available values roulette,tournament"));
    QStringList gen_crossover;
    gen_crossover<<"double"<<"uniform"<<"onepoint";
    addParam(Parameter("gen_crossover",gen_crossover[0],
                       gen_crossover,
                       "Crossover method. Available values uniform, onepoint, double"));
    QStringList gen_mutation;
    gen_mutation<<"double"<<"random";
    addParam(Parameter("gen_mutation",gen_mutation[0],
                       gen_mutation,
                       "Mutation method. Available values random, double"));
    QStringList gen_lsearchmethod;
    gen_lsearchmethod<<"none"<<"crossover"<<"mutate"<<"siman"<<"de";
    addParam(Parameter("gen_lsearchmethod",gen_lsearchmethod[0],
                       gen_lsearchmethod,
                       "Available methods: none,crossover,mutate,siman"));
    addParam(Parameter("gen_lsearchitems",20,0,100,"Number of items for local search"));
    addParam(Parameter("gen_lsearchgens",20,0,100,"Generations for local search"));
    }


void    Genetic::setBest(Data &xx,double y)
{
    bestx = xx;
    besty = y;
    hasInitialized = true;
}

void    Genetic::init()
{
    chromosomeCount=getParam("gen_count").getValue().toInt();
    maxGenerations=getParam("gen_maxiters").getValue().toInt();
    selectionRate=getParam("gen_srate").getValue().toDouble();
    mutationRate=getParam("gen_mrate").getValue().toDouble();
    tournamentSize = getParam("gen_tsize").getValue().toInt();
    selectionMethod=getParam("gen_selection").getValue();
    crossoverMethod=getParam("gen_crossover").getValue();
    mutationMethod=getParam("gen_mutation").getValue();
    localsearchRate=getParam("gen_lrate").getValue().toDouble();
    lsearchGens=getParam("gen_lsearchgens").getValue().toInt();
    lsearchItems=getParam("gen_lsearchitems").getValue().toInt();
    localSearchMethod = getParam("gen_lsearchmethod").getValue();

    generation = 0;
    //init process
    population.resize(chromosomeCount);
    fitnessArray.resize(chromosomeCount);

    sampleFromProblem(chromosomeCount,population,fitnessArray);
     childrenArray.resize(chromosomeCount);
    for(int i=0;i<chromosomeCount;i++)
    {
        childrenArray[i].resize(myProblem->getDimension());
    }
    if(hasInitialized)
    {
        population[0]=bestx;
        fitnessArray[0]=besty;
        hasInitialized=false;
    }
}

void    Genetic::step()
{
    ++generation;
    CalcFitnessArray();
    Selection();
    Crossover();
    Mutate();
    if(generation%lsearchGens==0 &&  localSearchMethod!=LOCAL_NONE)
    {
        LocalSearch(0);
        for(int i=0;i<lsearchItems;i++)
        {
            int pos = rand()  % population.size();
            LocalSearch(pos);
        }
	Selection();
    }
}

bool    Genetic::terminated()
{
    double besty;
    besty = fitnessArray[0];
    if(generation>=maxGenerations) return true;
    if(terminationMethod=="doublebox")
        return doubleBox.terminate(besty);
    else
        if(terminationMethod=="similarity")
        return similarity.terminate(besty);
    return false;
}

void    Genetic::showDebug()
{
    double besty;
    besty = fitnessArray[0];
     if(generation %10==0 && getParam("opt_debug").getValue()=="yes")
        methodLogger->printMessage(
            QString::asprintf("GENETIC. GENERATION=%4d BEST VALUE=%20.10lg",
                               generation,besty));
}

Collection  Genetic::makeChromosomesForRoulette()
{
    //first make all fitness value -f(x)
    //second make all fitness value positive
    //third sort the fitness value
    Collection copy ;
    double maxValue = -1e+100;


    for(int i=0;i<chromosomeCount;i++)
    {
        double y=fitnessArray[i];
        if(i==0 || fabs(y)>=maxValue) maxValue = fabs(y);

    }

    for(int i=0;i<chromosomeCount;i++)
    {
        Data x=population[i];
        double y;
        y = -y;
        y+=maxValue;
        copy.addPointNoCheck(x,y);
    }
    copy.sort();

    return copy;
}

int     Genetic::selectWithTournament(int size)
{
    int minPos = -1;
    double minValue = 1e+100;
    for(int i=0;i<size;i++)
    {
        int randPos = rand() %chromosomeCount;
        double y=fitnessArray[randPos];
        if(y<minValue || i==0)
        {
            minValue = y;
            minPos  = randPos;
        }
    }
    return minPos;
}

int     Genetic::selectWithRoulette(Collection &copy)
{
    double sumFitness = 0.0;
    for(int i=0;i<chromosomeCount;i++)
    {
        Data x;
        double y;
        copy.getPoint(i,x,y);
        sumFitness+=y;
    }
    double randomValue = (rand()*1.0/RAND_MAX)*sumFitness;
    double totalSum =0.0;
    for(int i=0;i<chromosomeCount;i++)
    {
        Data x;
        double y;
        copy.getPoint(i,x,y);
        totalSum+=y;
        if(totalSum >= randomValue)
            return i;
    }
    return chromosomeCount-1;
}

void    Genetic::makeChildrenUniform(Data &parent1,Data &parent2,
                                  Data &x1,Data &x2)
{

    for(int i=0;i<x1.size();i++)
    {
        int r = rand() %2;
        if(r==0)
        {
            x1[i]=parent1[i];
            x2[i]=parent2[i];
        }
        else
        {
            x1[i]=parent2[i];
            x2[i]=parent1[i];
        }
    }
}

void    Genetic::makeChildrenOnePoint(Data &parent1,Data &parent2,
                                   Data &x1,Data &x2)
{

    int randPos = rand() % x1.size();
    for(int i=0;i<x1.size();i++)
    {
        if(i<randPos)
        {
            x1[i]=parent1[i];
        }
        else x1[i]=parent2[i];
    }
    for(int i=0;i<x2.size();i++)
    {
        if(i<randPos)
        {
            x2[i]=parent2[i];
        }
        else x2[i]=parent1[i];
    }
}

void    Genetic::makeChildrenDouble(Data &parent1,Data &parent2,
                                 Data &x1,Data &x2)
{
    for(int i=0;i<x1.size();i++)
    {
        double alpha = rand() *1.0/RAND_MAX;
        alpha = -0.5 + 2.0 * alpha;//[-0.5,1.5]
        x1[i]=alpha * parent1[i]+(1.0-alpha)*parent2[i];
        x2[i]=alpha * parent2[i]+(1.0-alpha)*parent1[i];
    }
}

void    Genetic::CalcFitnessArray()
{
    for(int i=0;i<chromosomeCount;i++)
    {

        fitnessArray[i]=myProblem->statFunmin(population[i]);

        if(localsearchRate>0.0)
        {
            double r = rand()*1.0/RAND_MAX;
            if(r<localsearchRate)
            {
                fitnessArray[i]=localSearch(population[i]);
            }
        }
    }
}

void    Genetic::Selection()
{
Data itemp;
double dtemp;
itemp.resize(myProblem->getDimension());
for (int i = 0; i < population.size(); i++)
{
        for(int j=0;j<population.size()-1;j++)
        {
            if (fitnessArray[j + 1] < fitnessArray[j])
            {
                itemp = population[j];
                population[j] = population[j + 1];
                population[j + 1] = itemp;
                dtemp = fitnessArray[j];
                fitnessArray[j] = fitnessArray[j + 1];
                fitnessArray[j + 1] = dtemp;
            }
        }
}

}
void    Genetic::localCrossover(int pos)
{
    Data g;
    g.resize(myProblem->getDimension());
    for(int iters=1;iters<=100;iters++)
    {
         int gpos=rand() % chromosomeCount;
         int cutpoint=rand() % population[0].size();
         for(int j=0;j<(int)g.size();j++) g[j]=population[pos][j];
        double alpha = rand() *1.0/RAND_MAX;
        alpha = -0.5 + 2.0 * alpha;//[-0.5,1.5]
        g[cutpoint]=alpha * population[pos][cutpoint]+
                (1.0-alpha)*population[gpos][cutpoint];
        if(!myProblem->isPointIn(g)) continue;
    double f=myProblem->statFunmin(g);

    if(f<fitnessArray[pos])
    {
        population[pos][cutpoint]=g[cutpoint];
        fitnessArray[pos]=f;
    }
    else
    {
        g[cutpoint]=alpha * population[gpos][cutpoint]+
                (1.0-alpha)*population[pos][cutpoint];
        if(!myProblem->isPointIn(g)) continue;
        double f=myProblem->statFunmin(g);
        if(f<fitnessArray[pos])
        {

            population[pos][cutpoint]=g[cutpoint];
            fitnessArray[pos]=f;
        }
    }
    }
}

void    Genetic::localMutate(int pos)
{
   int s = myProblem->getDimension();
   for(int i=0;i<s;i++)
   {
       double gold = population[pos][i];
       double delta = 0.05 * rand()*1.0/RAND_MAX*gold;
       double direction = rand() % 2==1?1.0:-1.0;
       double gnew = gold+direction * delta;
       population[pos][i]=gnew;
       if(!myProblem->isPointIn(population[pos]))
       {
           population[pos][i]=gold;
           continue;
       }
       double f = myProblem->funmin(population[pos]);
       if(f<fitnessArray[pos])
       {
           fitnessArray[pos]=f;
       }
       else
       {
           population[pos][i]=gold;
       }
   }
}

void    Genetic::localSiman(int pos)
{
    double T0=1e+8;
    Data bestx;
    double besty;
    bestx = population[pos];
    besty = fitnessArray[pos];
    int i;
    int k=1;
    Data y;
    const int neps = 100;
    y.resize(bestx.size());
    Data xpoint = population[pos];
    double ypoint = fitnessArray[pos];
    while(true)
    {
        for(i=1;i<=neps;i++)
        {
        double fy;
        for(int j=0;j<bestx.size();j++)
        y[j]=xpoint[j];
    for(int j=0;j<30;j++)
    {
    int randPos = rand() % bestx.size();
        double range = 0.1;
	double old = y[randPos];
        int direction = rand() % 2==1?1:-1;
        double newValue =  y[randPos] + direction * rand()*1.0/RAND_MAX*range*y[randPos];
    y[randPos]=newValue;
    if(!myProblem->isPointIn(y)) y[randPos]=old;
    }
        fy = myProblem->statFunmin(y);

        if(isnan(fy) || isinf(fy)) continue;

        if(fy<ypoint)
        {
            xpoint = y;
            ypoint = fy;
            if(ypoint<besty)
            {
                bestx = xpoint;
                besty = ypoint;
            }
        }
        else
        {
            double r = fabs((rand()*1.0)/RAND_MAX);
            double ratio = exp(-(fy-ypoint)/T0);
            double xmin = ratio<1?ratio:1;
            if(r<xmin)
            {
                xpoint = y;
                ypoint = fy;
                if(ypoint<besty)
                {
                    bestx = xpoint;
                    besty = ypoint;
                }
            }
        }
        }
        const double alpha = 0.8;

        T0 =T0 * pow(alpha,k);
        k=k+1;
        if(T0<=1e-6) {
        //printf("Iteration: %4d Temperature: %20.10lg Value: %20.10lg\n",
        //       k,T0,besty);
		break;
	}

    }
    population[pos]=bestx;
    fitnessArray[pos]=besty;
}

void    Genetic::LocalSearch(int pos)
{
    if(localSearchMethod == LOCAL_CROSS)
        localCrossover(pos);
    else
    if(localSearchMethod == LOCAL_MUTATE)
        localMutate(pos);
    else
    if(localSearchMethod == LOCAL_SIMAN)
        localSiman(pos);
    else
    if(localSearchMethod=="de")
    {
        vector<double> g;
        g.resize(population[0].size());

            int randomA,randomB,randomC;
            do
            {
                randomA =  selectWithTournament(tournamentSize);
                randomB =  selectWithTournament(tournamentSize);
                randomC = selectWithTournament(tournamentSize);
            }while(randomA == randomB || randomB == randomC ||
                   randomC == randomA);
            double CR= 0.9;
            double F = 0.8;
            int n = myProblem->getDimension();
            int randomIndex = rand() % n;
        for(int i=0;i<n;i++)
        {
            if(i==randomIndex || myProblem->randomDouble() <=CR)
            {
                double old_value = population[pos][i];
                F = -0.5 + 2.0 * rand()*1.0/RAND_MAX;
                population[pos][i]=population[randomA][i]+abs(F*(population[randomB][i]-population[randomC][i]));
                if(!myProblem->isPointIn(population[pos]))
                {
                    population[pos][i]=old_value;
                    continue;
                }

                for(int j=0;j<n;j++) g[j]=population[pos][j];
                double trial_fitness=myProblem->statFunmin(g);
                if(fabs(trial_fitness)<fabs(fitnessArray[pos]))
                {
                    fitnessArray[pos]=trial_fitness;
                }
                else	population[pos][i]=old_value;
            }
        }
    }

}

void    Genetic::Crossover()
{
    Collection roulette;
    children.clear();
    if(selectionMethod=="roulette")
        roulette=makeChromosomesForRoulette();
    int countChildren =(int)( (1-selectionRate)*chromosomeCount);
    if(countChildren%2==1) countChildren++;
    int generatedChildren = 0;
    while(true)
    {
        int pos1,pos2;
        Data parent1,parent2;
        double y1,y2;
        if(selectionMethod == "roulette")
        {
            pos1 = selectWithRoulette(roulette);
            pos2 = selectWithRoulette(roulette);

            roulette.getPoint(pos1,parent1,y1);
            roulette.getPoint(pos2,parent2,y2);
        }
        else
        {
            pos1 = selectWithTournament(tournamentSize);
            pos2 = selectWithTournament(tournamentSize);
            parent1 = population[pos1];
            parent2 = population[pos2];
            //chromosomes.getPoint(pos1,parent1,y1);
            //chromosomes.getPoint(pos2,parent2,y2);
        }
        Data child1,child2;

        child1.resize(myProblem->getDimension());
        child2.resize(myProblem->getDimension());
        if(crossoverMethod == "uniform")
        {
            makeChildrenUniform(parent1,parent2,child1,child2);
        }
        else
            if(crossoverMethod == "onepoint")
            {
                makeChildrenOnePoint(parent1,parent2,child1,child2);
            }
            else
                if(crossoverMethod == "double")
                {
                    makeChildrenDouble(parent1,parent2,child1,child2);
                }
        if(!myProblem->isPointIn(child1))
            child1=parent1;
        if(!myProblem->isPointIn(child2))
            child2=parent2;
        //y1 and y2 are dummy values
        childrenArray[generatedChildren]=child1;
        childrenArray[generatedChildren+1]=child2;
        // children.addPointNoCheck(child1,y1);
       // children.addPointNoCheck(child2,y2);
        generatedChildren+=2;
        if(generatedChildren>=countChildren) break;
    }
    for(int i=0;i<countChildren;i++)
    {
        Data x;
        double y;
        //children.getPoint(i,x,y);
        int pos = chromosomeCount-i-1;
        population[pos]=childrenArray[i];
        //chromosomes.replacePoint(pos,x,y);
    }
}

double Genetic::deltaIter(int iter,double y)
{
    double r =rand() *1.0/RAND_MAX;
    return y * (1.0-pow(r,1.0-iter*1.0/maxGenerations));
}

void    Genetic::Mutate()
{
    //elitism with size=1
    int startPos =1;
    Data ll,rr;
    ll=myProblem->getLeftMargin();
    rr=myProblem->getRightMargin();
    for(int i=startPos;i<chromosomeCount;i++)
    {
        for(int j=0;j<myProblem->getDimension();j++)
        {
            double r = rand() *1.0/RAND_MAX;
            if(r<mutationRate)
            {
                Data x=population[i];
                double y;
                //chromosomes.getPoint(i,x,y);
                double oldValue = x[j];
                if(mutationMethod=="random")
                {
                    double delta = 0.05 *x[j];
                    int direction = rand()%2==1?1:-1;
                    x[j]=x[j]+direction*delta;

                }
                else
                {
                    int t = rand() %2;
                    double right = rr[j];
                    double left = ll[j];
                    if(t==0)
                        x[j]=x[j]+deltaIter(generation,right-x[j]);
                    else
                        x[j]=x[j]+deltaIter(generation,x[j]-left);
                }
                if(!myProblem->isPointIn(x))
                    x[j]=oldValue;
                population[i]=x;
            }
        }
    }
}

void    Genetic::done()
{
    fitnessArray[0]=localSearch(population[0]);
    if(getParam("opt_debug").getValue()=="yes" )
        methodLogger->printMessage(QString::asprintf("GENETIC. terminate: %lf ",fitnessArray[0]));
}

Genetic::~Genetic()
{

}
