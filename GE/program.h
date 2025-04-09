# ifndef __PROGRAM__H
# include <GE/symbol.h>
# include <GE/rule.h>
# include <GE/doublestack.h>
# include <vector>
using namespace std;

/**
 * @brief The Program class implements an abstract class
 * for the programs of Grammatical Evolution.
 */
class Program
{
	protected:
		Symbol *start_symbol;
	public:
        /**
         * @brief Program The constructor of the class.
         */
		Program();
        /**
         * @brief setStartSymbol Sets the start symbol of the grammar.
         * @param s
         */
		void	setStartSymbol(Symbol *s);
        /**
         * @brief getStartSymbol
         * @return The start symbol of the grammar.
         */
		Symbol	*getStartSymbol() const;
        /**
         * @brief printRandomProgram
         * @param genome
         * @param redo
         * @return a new program in string format.
         */
		string	printRandomProgram(vector<int> &genome,int &redo);
        /**
         * @brief hasFailed
         * @return
         */
        int     hasFailed();
        /**
         * @brief parse
         * @param slist
         * @return
         */
        int     parse(vector<Symbol*> slist);
        /**
         * @brief fitness
         * @param genome
         * @return the fitness for chromosome genome.
         */
		virtual double	fitness(vector<int> &genome);
        /**
         * @brief updatePositive
         * @param genome
         */
		void	updatePositive(vector<int> &genome);
        /**
         * @brief updateNegative
         * @param genome
         */
		void	updateNegative(vector<int> &genome);
        /**
         * @brief updateAll
         * @param genome1
         * @param genome2
         */
		void	updateAll(vector<int> &genome1,vector<int> &genome2);
        virtual ~Program();
};

# define __PROGRAM__H
# endif
