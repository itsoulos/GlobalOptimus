# ifndef __POPULATION__H
# include <GE/program.h>

# define GELOCAL_NONE       0x0
# define GELOCAL_CROSSOVER  0x1
# define GELOCAL_MUTATE     0x2
# define GELOCAL_BFGS       0x4
# define GELOCAL_SIMAN      0x8
/* The Population class holds the current population. */
/* The mutation, selection and crossover operators are defined here */
class Population
{
	private:
		int	**children;
		int	**genome;
		double *fitness_array;
		double	mutation_rate,selection_rate;
		int	genome_count;
		int	genome_size;
		int	generation;
		Program	*program;
        int  localMethod;
		void	select();
		void	crossover();
		void	mutate();
		void	calcFitnessArray();
		void	replaceWorst();
		int	elitism;
        int localsearch_generations;
        int localsearch_items;
	public:
		Population(int gcount,int gsize,Program *p);
        void setLocalMethod(int m);
		double 	fitness(vector<int> &g);
		void	setElitism(int s);
		int	getGeneration() const;
		int	getCount() const;
		int	getSize() const;
		void	nextGeneration();
		void	setMutationRate(double r);
		void	setSelectionRate(double r);
		double	getSelectionRate() const;
		double	getMutationRate() const;
		double	getBestFitness() const;
		double	evaluateBestFitness();
		vector<int> getBestGenome() const;
        void    getChromosome(int pos,vector<int> &x,double &y);
        void    setChromosome(int pos,vector<int> &x,double y);
		void	reset();
        void	localSearch(int gpos);
        void    setLocalSearchGenerations(int g);
        void    setLocalSearchItems(int i);
		~Population();
		
};
# define __POPULATION__H
# endif
