# include <GE/population.h>
# include <stdlib.h>
# include <string.h>
# include <math.h>
# include <iostream>
# include <OPTIMUS/problem.h>
# include <METHODS/bfgs.h>
# include <METHODS/simanmethod.h>
# include <GE/integeranneal.h>
# define MAX_RULE	256

/* Population constructor */
/* Input: genome count , genome size, pointer to Program instance */
Population::Population(int gcount,int gsize,Program *p,int seed)
{
	elitism=1;
	selection_rate = 0.1;
	mutation_rate  = 0.1;
	genome_count   = gcount;
	genome_size    = gsize;
	generation     = 0;
	program        = p;
    localMethod = GELOCAL_NONE;
    localsearch_generations=50;
    localsearch_items = 20;
	/* Create the population and based on genome count and size */
	/* Initialize the genomes to random */
	double f;
	genome=new int*[genome_count];
	children=new int*[genome_count];
    generator.seed(1);
	vector<int> g;
	g.resize(genome_size);
    randomSeed=seed;
    generator.seed(randomSeed);
    using param_t = std::uniform_int_distribution<>::param_type;
    intDistrib.param(param_t(0, MAX_RULE-1));
    intDistribGcount.param(param_t(0,genome_count-1));
    intDistribGsize.param(param_t(0,genome_size-1));
	for(int i=0;i<genome_count;i++)
	{
		genome[i]=new int[genome_size];
		children[i]=new int[genome_size];
			for(int j=0;j<genome_size;j++)
                g[j]=genome[i][j]=rand()%MAX_RULE;
	}
	fitness_array=new double[genome_count];
}

void    Population::setCrossoverItems(int n)
{
    if(n>0 && n<genome_count)
        crossover_items = n;
}
int     Population::getCrossoverItems() const
{
    return crossover_items;
}

void    Population::setLocalSearchRate(double r)
{
    if(r>=0.0 && r<=1.0)
        lrate = r;
}
double  Population::getLocalSearchRate() const
{
    return lrate;
}
/* Reinitialize the population to random */
void	Population::reset()
{
	generation = 0;
	for(int i=0;i<genome_count;i++)
		for(int j=0;j<genome_size;j++)
                genome[i][j]=intDistrib(generator);//rand()%MAX_RULE;
	for(int i=0;i<genome_count;i++)
            fitness_array[i]=1e+100;
}

/* Return the fitness of a genome */
double 	Population::fitness(vector<int> &g)
{
	return program->fitness(g);
}


void    Population::setLocalMethod(int m)
{
    localMethod =m;
}
void    Population::getChromosome(int pos,vector<int> &x,double &y)
{
    x.resize(genome_size);
    for(int i=0;i<(int)x.size();i++)
        x[i]=genome[pos][i];
    y=fitness_array[pos];
}

void    Population::setChromosome(int pos,vector<int> &x,double y)
{
   for(int i=0;i<genome_size;i++)
    genome[pos][i]=x[i];
   fitness_array[pos]=y;
}

/* The selection of the chromosomes according to the fitness value is performed */
void	Population::select()
{
	int itemp[genome_size];
	for(int i=0;i<genome_count;i++)
	{
		for(int j=0;j<genome_count-1;j++)
		{
            if(fitness_array[j+1]<fitness_array[j])
			{
				double dtemp;
				dtemp=fitness_array[j];
				fitness_array[j]=fitness_array[j+1];
				fitness_array[j+1]=dtemp;
				
				memcpy(itemp,genome[j],genome_size*sizeof(int));
				memcpy(genome[j],genome[j+1],genome_size*sizeof(int));
				memcpy(genome[j+1],itemp,genome_size*sizeof(int));
			}
		}
	}
}

/* Crossover operation: based on tournament selection */
/* Select the tournament_size based on the genome count : */
/*     (if genome_count > 100 ) tournament_size = 10   else   tournament_size = 4 */
/* Select 2 chromosomes based on the tournament size and cross them over based on the crossover probability */
/* There is 1 crossover point and it is random */
void	Population::crossover()
{
        int parent[2];
        int nchildren=(int)((1.0 - selection_rate) * genome_count);
	if(!(nchildren%2==0)) nchildren++;
        const int tournament_size =(genome_count<=100)?4:20;
        int count_children=0;
        while(1)
        {
		// The two parents are selected here according to the tournament selection procedure
                for(int i=0;i<2;i++)
                {
                        double max_fitness=1e+10;
                        int    max_index=-1;
			int r;
			// Select the best parents of  the candidates 
                        for(int j=0;j<tournament_size;j++)
                        {
                r= rand() % (genome_count);
                                if(j==0 || fitness_array[r]<max_fitness)
                                {
                                        max_index=r;
                                        max_fitness=fitness_array[r];
                                }
                        }
                        parent[i]=max_index;
			
                }
		int pt1,pt2;
		// The one-point crossover is performed here (the point is pt1)
        pt1=rand() % genome_size;
		memcpy(children[count_children],
				genome[parent[0]],pt1 * sizeof(int));
		memcpy(&children[count_children][pt1],
			&genome[parent[1]][pt1],(genome_size-pt1)*sizeof(int));
		memcpy(children[count_children+1],
				genome[parent[1]],pt1 * sizeof(int));
		memcpy(&children[count_children+1][pt1],
			&genome[parent[0]][pt1],(genome_size-pt1)*sizeof(int));
		count_children+=2;
		if(count_children>=nchildren) break;
	}
	
	vector<int> g;
	g.resize(genome_size);
	
	for(int i=0;i<nchildren;i++)
	{
		//for(int j=0;j<g.size();j++) g[j]=children[i][j];
		//double f=fitness(g);
		//if(fabs(f)<fabs(fitness_array[genome_count-i-1]))
		memcpy(genome[genome_count-i-1],
				children[i],genome_size * sizeof(int));
	}
}

void	Population::setElitism(int s)
{
	elitism = s;
}

/* Mutate the current population */
/* Standard mutation algorithm: mutate all chromosomes in the population based on the mutation probability */
void	Population::mutate()
{
	int start = elitism * (int)(genome_count*selection_rate);
	start = elitism;
	start = 1;
	for(int i=start;i<genome_count;i++)
	{
		for(int j=0;j<genome_size;j++)
		{
            double r=rand()*1.0/RAND_MAX;
			if(r<mutation_rate)
			{
                genome[i][j]=rand() % MAX_RULE;
			}
		}
	}
}

/* Evaluate the fitness for all chromosomes in the current population */
void	Population::calcFitnessArray()
{
	vector<int> g;
	g.resize(genome_size);

	double dmin=1e+100;
	int icount=0;
	for(int i=0;i<genome_count;i++)
	{
		for(int j=0;j<genome_size;j++) g[j]=genome[i][j];	
		fitness_array[i]=fitness(g);
		//else 
		//localSearch(i);
        if(lrate>0.0)
        {
            double  r =  rand() *1.0 /RAND_MAX;
            if(r<=lrate)
	    {
                localSearch(i);
	    }
        }
		if(fabs(fitness_array[i])<dmin)
		{
			dmin=fabs(fitness_array[i]);
		}
        if(fabs(fitness_array[i])>=1e+100) icount++;
        if(isinf(fitness_array[i]) || isnan(fitness_array[i]))
            fitness_array[i]=1e+100;
        if((i+1)%50==0)
		{
	//		printf(" %d:%.5lg ",i+1,dmin);
	//		fflush(stdout);
        }
		
	}
	return;
	for(int i=0;i<genome_count;i++)
	{
		if(fabs(fitness_array[i])>=1e+8 && generation>5)
		{
			do
			{
				localSearch(i);
	//			for(int j=0;j<genome_size;j++) 
	//				  genome[i][j]=g[j]=rand() % MAX_RULE;
	//			fitness_array[i]=fitness(g);
				if(fabs(fitness_array[i])<1e+8) break;
			}while(1);
		}
	}
	
}

/* Return the current generation */
int	Population::getGeneration() const
{
	return generation;
}

/* Return the genome count */
int	Population::getCount() const
{
	return genome_count;
}

/* Return the size of the population */
int	Population::getSize() const
{
	return genome_size;
}


void    Population::setLocalSearchGenerations(int g)
{
    localsearch_generations = g;
}
void    Population::setLocalSearchItems(int i)
{
    localsearch_items = i;
}

/* Evolve the next generation */
void	Population::nextGeneration()
{
	calcFitnessArray();
	
	
    const int mod=5;//localsearch_generations;
    const int count=5;//localsearch_items;
	if((generation+1) % mod==0) 
	{
		for(int i=0;i<count;i++)
			localSearch(rand()%genome_count);
    }
	
	select();
	crossover();
	if(generation) mutate();
	++generation;
}

void	Population::replaceWorst()
{
	vector<int> xtrial;
	xtrial.resize(genome_size);
	int randpos;
	randpos=rand() % genome_count;
	for(int i=0;i<genome_size;i++)
	{
		double gamma;
        gamma=-0.5+2.0*rand()*1.0/RAND_MAX;
		xtrial[i]=(int)(fabs((1.0+gamma)*genome[0][i]-gamma*genome[randpos][i]));
	}
	double ftrial = fitness(xtrial);
	if(ftrial>fitness_array[genome_count-1])
	{
		for(int i=0;i<genome_size;i++)
		genome[genome_count-1][i]=xtrial[i];
		fitness_array[genome_count-1]=ftrial;
	}
}

/* Set the mutation rate */
void	Population::setMutationRate(double r)
{
	if(r>=0 && r<=1) mutation_rate = r;
}

/* Set the selection rate */
void	Population::setSelectionRate(double r)
{
	if(r>=0 && r<=1) selection_rate = r;
}

/* Return the selection rate */
double	Population::getSelectionRate() const
{
	return selection_rate;
}

/* Return the mutation rate */
double	Population::getMutationRate() const
{
	return mutation_rate;
}

/* Return the best fitness for this population */
double	Population::getBestFitness() const
{
	return  fitness_array[0];
}

/* Return the best chromosome */
vector<int> Population::getBestGenome() const
{
	vector<int> g;g.resize(genome_size);
	for(int i=0;i<genome_size;i++) g[i]=genome[0][i];
	return g;
}


class PopulationProblem: public Problem
{
private:
    Population *pop;
    vector<int> currentGenome;
public:
    PopulationProblem(Population *p,vector<int> &x)
        :Problem(x.size())
    {
        pop = p;
        currentGenome.resize(x.size());
        //currentGenome = x;
        setDimension(x.size());
        left.resize(x.size());
        right.resize(x.size());
        for(int i=0;i<x.size();i++)
        {
        left[i]=0;
        right[i]=MAX_RULE;
        }
        setLeftMargin(left);
        setRightMargin(right);
    }

    double dmax(double a,double b){return a>b?a:b;}
    virtual double funmin(Data &x)
    {
        for(int i=0;i<(int)x.size();i++) {
        currentGenome[i]=(int)fabs(x[i]);
      if(isnan(x[i]) || isinf(x[i])) return 1e+100;
        //printf("xsize =%d c = %d x= %lf\n",x.size(),currentGenome[i],x[i]);
    //   if(currentGenome[i]<0) currentGenome[i]=0;
        }
        double f= pop->fitness(currentGenome);
        return f;

    }
    virtual Data	gradient(Data &x)
    {

        Data g;
        g.resize(x.size());
        for(int i=0;i<(int)x.size();i++)
        {
        double eps=pow(1e-18,1.0/3.0)*dmax(1.0,fabs(x[i]));
        x[i]+=eps;
        double v1=funmin(x);
        x[i]-=2.0 *eps;
        double v2=funmin(x);
        g[i]=(v1-v2)/(2.0 * eps);

        x[i]+=eps;
        }
        return g;
    }

};

void	Population::localSearch(int pos)
{

    if(localMethod == GELOCAL_NONE) return;
	vector<int> g;
	g.resize(genome_size);

	for(int i=0;i<genome_size;i++) g[i]=genome[pos][i];

    if(localMethod == GELOCAL_CROSSOVER)
    {
    for(int iters=1;iters<=crossover_items;iters++)
	{
        int gpos=rand() % genome_count;
        int cutpoint=rand() % genome_size;
		for(int j=0;j<cutpoint;j++) g[j]=genome[pos][j];
		for(int j=cutpoint;j<genome_size;j++) g[j]=genome[gpos][j];
		double f=fitness(g);
		if(fabs(f)<fabs(fitness_array[pos]))
		{
			for(int j=0;j<genome_size;j++) genome[pos][j]=g[j];
            fitness_array[pos]=f;
		}
		else
		{
			for(int j=0;j<cutpoint;j++) g[j]=genome[gpos][j];
			for(int j=cutpoint;j<genome_size;j++) g[j]=genome[pos][j];
			double f=fitness(g);
			if(fabs(f)<fabs(fitness_array[pos]))
			{

				for(int j=0;j<genome_size;j++) genome[pos][j]=g[j];
                fitness_array[pos]=f;
			}
		}
	}
    }
    else
    if(localMethod==GELOCAL_MUTATE)
    {
		
    for(int i=0;i<genome_size;i++)
	{
        int ipos =rand() % genome_size;
                int new_value;
                for(int k=0;k<20;k++)
                {
			int old_value,range,direction;
again:
		
                old_value = genome[pos][ipos];
                range = 10;
                direction = rand() % 2==1?1:-1;
                new_value =  old_value + direction * (rand() % range);
                if(new_value<0) goto again; 
                genome[pos][ipos]=new_value;
                for(int j=0;j<genome_size;j++) g[j]=genome[pos][j];
                double trial_fitness=fitness(g);
                if(fabs(trial_fitness)<fabs(fitness_array[pos]))
                {
//                    printf("MUTATE[%d] %lf=>%lf\n",pos,fitness_array[pos],trial_fitness);

                    fitness_array[pos]=trial_fitness;
                }
                else	genome[pos][ipos]=old_value;
                }
	}
    }
    else
    if(localMethod == GELOCAL_SIMAN)
    {
        double f = fitness_array[pos];
        IntegerAnneal lt(program);
        lt.setPoint(g,fitness_array[pos]);
        lt.Solve();
        lt.getPoint(g,fitness_array[pos]);
        for(int j=0;j<genome_size;j++) genome[pos][j]=g[j];

      //  printf("SIMAN[%d] %lf=>%lf\n",pos,f,fitness_array[pos]);
    }
    else
    if(localMethod == GELOCAL_BFGS)
    {
    PopulationProblem *pr=new PopulationProblem(this,g);
    double y;
    Data x;
    x.resize(genome_size);
    y=fitness_array[pos];
    for(int i=0;i<genome_size;i++) x[i]=g[i];
    Optimizer *method=NULL;
    if(localMethod == GELOCAL_BFGS)
     method=new Bfgs();
    else
     method = new SimanMethod();
    method->setParam("opt_localsearch","none");
    method->setProblem(pr);
    ((Bfgs *)method)->setPoint(x,y);
     method->solve();
    x = pr->getBestx();
    y = pr->funmin(x);
    delete method;
    double ff = fitness_array[pos];
    if(fabs(y)<=fabs(ff))
    {
        fitness_array[pos]=y;
      //  printf("New value [%lg]->[%lg]\n",ff,y);
        for(int i=0;i<genome_size;i++)
        {
        genome[pos][i]=(int)fabs(x[i]);
            if(genome[pos][i]<0) genome[pos][i]=0;
        }
    }
    //else printf("FAILED VALUES[%lg]=>[%lg]\n",ff,y);
    delete pr;
    printf("BFGS[%d] %lf=>%lf\n",pos,ff,fitness_array[pos]);

    }

}

/* Evaluate and return the best fitness for all chromosomes in the population */
double	Population::evaluateBestFitness() 
{
	vector<int> g;g.resize(genome_size);
	for(int i=0;i<genome_size;i++) g[i]=genome[0][i];	
	return fitness(g);
}

void    Population::setRandomSeed(int s)
{
    randomSeed=s;
    generator.seed(randomSeed);
}

/* Destructor */
Population::~Population()
{
	for(int i=0;i<genome_count;i++)
	{
		delete[] children[i];
		delete[] genome[i];
	}
	delete[] genome;
	delete[] children;
	delete[] fitness_array;
}
