# ifndef __POPULATION__H
# include <GE/program.h>
#include <random>
using namespace std;

# define GELOCAL_NONE       0x0
# define GELOCAL_CROSSOVER  0x1
# define GELOCAL_MUTATE     0x2
# define GELOCAL_BFGS       0x4
# define GELOCAL_SIMAN      0x8
/**
 * @brief The Population class class holds the current populatio for
 * Grammatical Evolution.
 */
class Population
{
	private:
		int	**children;
		int	**genome;
		double *fitness_array;
		double	mutation_rate,selection_rate;
		vector<int>  maxrule;
		int	genome_count;
		int	genome_size;
		int	generation;
        double lrate=0.0;
        int  crossover_items = 100;
		Program	*program;
        int  localMethod;
		void	select();
		void	crossover();
		void	mutate();
		void	calcFitnessArray();
		void	replaceWorst();
		int	elitism;
		bool	hasSteadyState=false;
        int localsearch_generations;
        int localsearch_items;
        int randomSeed=1;
        mt19937 generator;
        uniform_int_distribution<> intDistrib;
        uniform_int_distribution<> intDistribGcount;
        uniform_int_distribution<> intDistribGsize;
        uniform_real_distribution<> doubleDistrib;
        void initPopulation(int gcount,int gsize,Program *p,vector<int> &r,int seed);
	public:
        /**
         * @brief Population The constructor of the class.
         * Initializes the Genetic population.
         * @param gcount
         * @param gsize
         * @param p
         * @param seed
         */
        Population(int gcount,int gsize,Program *p,int seed=1);
        Population(int gcount,int gsize,Program *p,vector<int> &rule,int seed=1);
        /**
         * @brief setLocalMethod
         * @param m
         */
        void    setLocalMethod(int m);
        /**
         * @brief fitness
         * @param g
         * @return the fitness of a chromosome g.
         */
		double 	fitness(vector<int> &g);
        /**
         * @brief setElitism Sets the elitism count
         * @param s
         */
		void	setElitism(int s);
        /**
         * @brief getGeneration
         * @return the generation number
         */
        int     getGeneration() const;
        /**
         * @brief getCount
         * @return the count of chromosomes.
         */
        int     getCount() const;
        /**
         * @brief getSize
         * @return the size of each chromosome.
         */
        int     getSize() const;
        /**
         * @brief nextGeneration Creates the next generation
         * using the genetic operations.
         */
		void	nextGeneration();
        /**
         * @brief setMutationRate Sets the mutation rate.
         * @param r
         */
		void	setMutationRate(double r);
        /**
         * @brief setSelectionRate Sets the selection rate.
         * @param r
         */
		void	setSelectionRate(double r);
        /**
         * @brief getSelectionRate
         * @return the selection rate.
         */
		double	getSelectionRate() const;
        /**
         * @brief getMutationRate
         * @return the mutation rate.
         */
		double	getMutationRate() const;
        /**
         * @brief getBestFitness
         * @return the best fitness value in the population.
         */
		double	getBestFitness() const;
        /**
         * @brief evaluateBestFitness
         * @return Evaluate and return the best fitness value in the population.
         */
		double	evaluateBestFitness();
        /**
         * @brief getBestGenome
         * @return the best Chromosome in the population.
         */
		vector<int> getBestGenome() const;
        /**
         * @brief getChromosome Return chromosome and value at pos.
         * @param pos
         * @param x
         * @param y
         */
        void    getChromosome(int pos,vector<int> &x,double &y);
        /**
         * @brief setChromosome Sets chromosome and value at pos.
         * @param pos
         * @param x
         * @param y
         */
        void    setChromosome(int pos,vector<int> &x,double y);
        /**
         * @brief reset Initialize the population.
         */
		void	reset();
        /**
         * @brief localSearch Enchances with localsearch chromosome at pos.
         * @param gpos
         */
        void	localSearch(int gpos);
        /**
         * @brief setLocalSearchGenerations
         * @param g
         */
        void    setLocalSearchGenerations(int g);
        /**
         * @brief setLocalSearchItems
         * @param i
         */
        void    setLocalSearchItems(int i);
        /**
         * @brief setLocalSearchRate
         * @param r
         */
        void    setLocalSearchRate(double r);
        /**
         * @brief getLocalSearchRate
         * @return
         */
        double  getLocalSearchRate() const;
        /**
         * @brief setCrossoverItems
         * @param n
         */
        void    setCrossoverItems(int n);
        /**
         * @brief getCrossoverItems
         * @return
         */
        int     getCrossoverItems() const;
        /**
         * @brief setRandomSeed Sets the random seed of the population.
         * @param s
         */
        void    setRandomSeed(int s);
        /**
         * @brief enableSteadyState Enable steady state.
         */
        void	enableSteadyState();
        /**
         * @brief disableSteadyState Disable steady state.
         */
        void	disableSteadyState();
        /**
         * @brief tournament
         * @return with tournament selection a chromosome position.
         */
        int     tournament() ;
        /**
         * @brief evaluateFitnessAt Evaluate the fitness at position pos.
         * @param pos
         */
        void    evaluateFitnessAt(int pos);
		~Population();
		
};
# define __POPULATION__H
# endif
