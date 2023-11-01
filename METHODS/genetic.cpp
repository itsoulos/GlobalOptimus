#include "genetic.h"
Genetic::Genetic()
{

    addParam(Parameter("gen_count","200","Number of chromosomes"));
    addParam(Parameter("gen_maxiters","200","Maximum number of generations"));
    addParam(Parameter("gen_srate","0.10","Selection rate"));
    addParam(Parameter("gen_mrate","0.05","Mutation rate"));
    addParam(Parameter("gen_lrate","0.005","Localsearch rate"));
    addParam(Parameter("gen_tsize","8","Tournament size"));
    addParam(Parameter("gen_selection","roulette","Selection method. Available values roulette,tournament"));
    addParam(Parameter("gen_crossover","double","Crossover method. Available values uniform, onepoint, double"));
    addParam(Parameter("gen_mutation","double","Mutation method. Available values random, double"));
    addParam(Parameter("gen_termination","doublebox","Termination method. Avaible values: maxiters,similarity,doublebox"));
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
    terminationMethod=getParam("gen_termination").getValue();
    localsearchRate=getParam("gen_lrate").getValue().toDouble();
    generation = 0;
    //init process
   // chromosomes.clear();
    population.resize(chromosomeCount);
    fitnessArray.resize(chromosomeCount);

    sampleFromProblem(chromosomeCount,population,fitnessArray);
     childrenArray.resize(chromosomeCount);
    for(int i=0;i<chromosomeCount;i++)
    {
        childrenArray[i].resize(myProblem->getDimension());
    }
}

void    Genetic::step()
{
    ++generation;
    CalcFitnessArray();
    Selection();
    Crossover();
    Mutate();
}

bool    Genetic::terminated()
{
    double besty,worsty;
    besty = fitnessArray[0];
    //chromosomes.getBestWorstValues(besty,worsty);
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
    double besty,worsty;
    besty = fitnessArray[0];
     if(getParam("opt_debug").getValue()=="yes")
    printf("GENETIC. GENERATION=%4d BEST VALUE=%20.10lg\n",
           generation,besty);
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
        /*
        Data x;
        double y;
        chromosomes.getPoint(i,x,y);

        y = myProblem->statFunmin(x);
        if(localsearchRate>0.0)
        {
            double r = rand()*1.0/RAND_MAX;
            if(r<localsearchRate)
            {
                y = localSearch(x);
            }
        }
        if(isnan(y) || isinf(y)) y = 1e+10;
            chromosomes.replacePoint(i,x,y);
    */
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
   // chromosomes.sort();
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
    Data xpoint;
    double ypoint;
   // chromosomes.sort();
   // chromosomes.getPoint(0,xpoint,ypoint);
   // ypoint = localSearch(xpoint);
    fitnessArray[0]=localSearch(population[0]);
    if(getParam("opt_debug").getValue()=="yes")
        printf("GENETIC. terminate: %lf \n",fitnessArray[0]);
}
Genetic::~Genetic()
{

}
