# ifndef __POPULATION__H
# include <GE/program.h>

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

		void	select();
		void	crossover();
		void	mutate();
		void	calcFitnessArray();
		void	replaceWorst();
		int	elitism;

	public:
		Population(int gcount,int gsize,Program *p);
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
		~Population();
		
};
# define __POPULATION__H
# endif
