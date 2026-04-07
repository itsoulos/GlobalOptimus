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
	vector<int> grule;
	grule.resize(gsize);
	for(int i=0;i<gsize;i++)
		grule[i]=MAX_RULE;
	initPopulation(gcount,gsize,p,grule,seed);
}

Population::Population(int gcount,int gsize,Program *p,vector<int> &rule,int seed)
{
	initPopulation(gcount,gsize,p,rule,seed);
}
void Population::initPopulation(int gcount,int gsize,Program *p,vector<int> &rule,int seed)
{
	elitism=1;
	selection_rate = 0.1;
	mutation_rate  = 0.1;
	genome_count   = gcount;
	genome_size    = gsize;
	generation     = 0;
	program        = p;
	maxrule.resize(gsize);
    for(int i=0;i<gsize;i++)
    {
        maxrule[i]=rule[i];
    }
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
                g[j]=genome[i][j]=rand()%maxrule[j];
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

int Population::tournament()
{
    const int tournament_size =(genome_count<=100)?4:20;

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
    return max_index;
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
                        parent[i]=tournament();
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
		if(hasSteadyState)
		{
		for(int j=0;j<g.size();j++) g[j]=children[i][j];
		double f=fitness(g);
		if(fabs(f)<fabs(fitness_array[genome_count-i-1]))
		memcpy(genome[genome_count-i-1],
				children[i],genome_size * sizeof(int));
		}
		else
		{
		memcpy(genome[genome_count-i-1],
				children[i],genome_size * sizeof(int));
		}
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
	vector<int> g;
	g.resize(genome_size);
	
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
				genome[i][j]=rand() % maxrule[j];
			}
		}
	}
}

void    Population::evaluateFitnessAt(int pos)
{
    vector<int> g;
    g.resize(genome_size);
    for(int j=0;j<genome_size;j++) g[j]=genome[pos][j];
    fitness_array[pos]=fitness(g);
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
      //     printf(" %d:%.5lg ",i+1,dmin);
      //      fflush(stdout);
        }
		
	}
    //printf("\n");
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

static mt19937 gen(random_device{}());

double randDouble(double a, double b) {
    uniform_real_distribution<> dist(a, b);
    return dist(gen);
}

int randInt(int a, int b) {
    uniform_int_distribution<> dist(a, b);
    return dist(gen);
}

vector<int> Population::neighbor(const vector<int>& x, int stepSize ) {
    vector<int> y = x;

    int i = randInt(0, x.size() - 1);

    // αλλαγή σε μία διάσταση
    y[i] += randInt(-stepSize, stepSize);
    if(y[i]<0) y[i]=0;

    return y;
}

vector<int> Population::simulatedAnnealing(
    vector<int> &x,
    double T0,
    double Tmin,
    double alpha,
    int iterPerTemp
    ) {
    double T = T0;

    vector<int> best = x;
    double bestVal = fitness(x);

    while (T > Tmin) {

        for (int k = 0; k < iterPerTemp; k++) {

            vector<int> y = neighbor(x);

            double fx = (fitness(x));
            double fy = (fitness(y));

            double delta = fabs(fy) - fabs(fx);

            if (delta < 0) {
                // καλύτερη λύση
                x = y;
            } else {
                // πιθανότητα αποδοχής
                double p = exp(-delta / T);

                if (randDouble(0,1) < p)
                    x = y;
            }

            // update best
            double val = fitness(x);
            if (fabs(val) < fabs(bestVal)) {
                best = x;
                bestVal = val;
            }
        }
        //printf("T=%20.10lf BEST=%20.10lf\n",T,bestVal);
        // cooling
        T *= alpha;
    }

    return best;
}


vector<int> Population::discreteGradient(vector<int>& x)
{
    int n = x.size();
    vector<int> grad(n);

    double fx = fitness(x);

    for (int i = 0; i < n; i++) {

        vector<int> x_plus = x;
        vector<int> x_minus = x;

        x_plus[i] += 1;
        x_minus[i] -= 1;
        if(x_minus[i]<0) x_minus[i]=0;

        double f_plus = fitness(x_plus);
        double f_minus = fitness(x_minus);

        if (f_plus < fx)
            grad[i] = +1;
        else if (f_minus < fx)
            grad[i] = -1;
        else
            grad[i] = 0;
    }
    return grad;
}

vector<int> Population::discreteStep(vector<int>& x,vector<int>& grad)
{
    vector<int> res = x;

    for (int i = 0; i < (int)x.size(); i++)
    {
        res[i] += grad[i];
    }

    return res;
}

void Population::integerLocalSearch(vector<int> &x,int maxSteps)
{
    double bestVal = fitness(x);
    int stepSize = 100;
    for (int step = 0; step < maxSteps; step++) {

        vector<int> grad = discreteGradient(x);

        vector<int> candidate = x;

        for (int i = 0; i < x.size(); i++)
        {
            candidate[i] += grad[i] * stepSize;
            if(candidate[i]<0) candidate[i]=0;
        }

        double val = fitness(candidate);

        if (val < bestVal) {
            x = candidate;
            bestVal = val;
        } else {
            stepSize = max(1, stepSize / 2); // learning rate decay
        }
        printf("GD[%4d]=%20.10lg\n",step,bestVal);
    }


}


vector<int> Population::integerAdam(
    vector<int> x,
    int steps,
    double alpha,
    double beta1,
    double beta2,
    double eps
    ) {
    int n = x.size();

    vector<double> m(n, 0.0);
    vector<double> v(n, 0.0);

    double bestVal = fitness(x);

    for (int t = 1; t <= steps; t++) {

        vector<int> g_int = discreteGradient(x);

        // convert gradient to double
        vector<double> g(n);
        for (int i = 0; i < n; i++)
            g[i] = (double)g_int[i];

        // update moments
        for (int i = 0; i < n; i++) {
            m[i] = beta1 * m[i] + (1 - beta1) * g[i];
            v[i] = beta2 * v[i] + (1 - beta2) * g[i] * g[i];
        }

        // bias correction
        vector<double> m_hat(n), v_hat(n);
        for (int i = 0; i < n; i++) {
            m_hat[i] = m[i] / (1 - pow(beta1, t));
            v_hat[i] = v[i] / (1 - pow(beta2, t));
        }

        // candidate update
        vector<int> candidate = x;

        for (int i = 0; i < n; i++) {

            double step = alpha * m_hat[i] / (sqrt(v_hat[i]) + eps);
            double p = fabs(step);

            if (rand()*1.0/RAND_MAX < p) {
                candidate[i] += (step > 0 ? 1 : -1);
            }

            if(candidate[i]<0) candidate[i]=0;
        }

        double val = fitness(candidate);

        if (val < bestVal) {
            x = candidate;
            bestVal = val;
        }
        else {
            alpha *= 0.7; // decay learning rate
        }
    }

    return x;
}
/* Evolve the next generation */
void	Population::nextGeneration()
{
	calcFitnessArray();
	
	
	
    const int mod=localsearch_generations;
    const int count=localsearch_items;
    if((generation+1) % mod==0)
	{
		for(int i=0;i<count;i++)
            localSearch(i==0?0:rand()%genome_count);
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
        }
        double f= pop->fitness(currentGenome);
        return f;

    }
    virtual Data	 gradient(Data &x)
    {

        int n = x.size();
        vector<double> grad(n);
        for(int i=0;i<(int)x.size();i++) {
            currentGenome[i]=(int)fabs(x[i]);
        double fx = pop->fitness(currentGenome);

        for (int i = 0; i < n; i++) {

            vector<int> x_plus = currentGenome;
            vector<int> x_minus = currentGenome;

            x_plus[i] += 1;
            x_minus[i] -= 1;
            if(x_minus[i]<0) x_minus[i]=0;

            double f_plus = pop->fitness(x_plus);
            double f_minus = pop->fitness(x_minus);

            if (f_plus < fx)
                grad[i] = +1;
            else if (f_minus < fx)
                grad[i] = -1;
            else
                grad[i] = 0;
        }
        return grad;
    }
    }
};


void 	Population::localSearch(int pos)
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
                printf("MUTATE %lf => %lf \n",fitness_array[pos],trial_fitness);
                fitness_array[pos]=trial_fitness;
            }
            else	genome[pos][ipos]=old_value;
        }
    }
    else
    if(localMethod==GELOCAL_DE)
    {
		
        int randomA,randomB,randomC;
        do
        {
            randomA =  tournament();
            randomB =  tournament();
            randomC =  tournament();
        }while(randomA == randomB || randomB == randomC || randomC == randomA);
        double CR= 0.9;
        double F = 0.8;
        int randomIndex = rand() % genome_size;
    for(int i=0;i<genome_size;i++)
	{
        if(i==randomIndex || rand()*1.0/RAND_MAX <=CR)
        {
            int old_value = genome[pos][i];
            F = -0.5 + 2.0 * rand()*1.0/RAND_MAX;
            genome[pos][i]=((int)(genome[randomA][i]+abs(F*(genome[randomB][i]-genome[randomC][i]))))%maxrule[i];
            if(genome[pos][i]<0)
            {
             genome[pos][i]=old_value;
             continue;
            }

            for(int j=0;j<genome_size;j++) g[j]=genome[pos][j];
            double trial_fitness=fitness(g);
            if(fabs(trial_fitness)<fabs(fitness_array[pos]))
            {
           //   printf("NEW DE VALUE[%d] = %lf=>%lf\n",pos,fitness_array[pos],trial_fitness);
                fitness_array[pos]=trial_fitness;
            }
            else	genome[pos][i]=old_value;
        }

	}
    }
    else
    if(localMethod == GELOCAL_SIMAN)
    {
        double f = fitness_array[pos];
        /*IntegerAnneal lt(program);
        lt.setPoint(g,fitness_array[pos]);
        lt.Solve();
        lt.getPoint(g,fitness_array[pos]);*/
        g=simulatedAnnealing(g);
          g=integerAdam(g);
        fitness_array[pos]=fitness(g);
        for(int j=0;j<genome_size;j++) genome[pos][j]=g[j];

     printf("SIMAN[%d] %lf=>%lf\n",pos,f,fitness_array[pos]);
    }
    else
    if(localMethod == GELOCAL_BFGS)
    {
        g=integerAdam(g);/*
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
    delete method;*/
        double y = fitness(g);
    double ff = fitness_array[pos];

    if(fabs(y)<=fabs(ff))
    {
        fitness_array[pos]=y;
       printf("New value [%lg]->[%lg]\n",ff,y);
        for(int i=0;i<genome_size;i++)
        {
            //genome[pos][i]=(int)fabs(x[i]);
            genome[pos][i]=g[i];
            if(genome[pos][i]<0) genome[pos][i]=0;
        }
    }
    //else printf("FAILED VALUES[%lg]=>[%lg]\n",ff,y);
    //delete pr;
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

void	Population::enableSteadyState()
{
	hasSteadyState = true;
}

void	Population::disableSteadyState()
{
	hasSteadyState=false;
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
