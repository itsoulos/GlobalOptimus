# ifndef __RULE__H
# include <GE/symbol.h>
# include <GE/doublestack.h>
# include <vector>
# define REDO_MAX	1
using namespace std;


/**
 * @brief The Rule class Creates the rules based on the grammar file and adds the symbols
 */
class Rule
{
	private:
		vector<Symbol*> data;	
		int	length;
	public:
        /**
         * @brief Rule The main constructor of the class.
         */
		Rule();
        /**
         * @brief addSymbol Adds a symbol to the rule.
         * @param s
         */
		void	addSymbol(Symbol *s);
        /**
         * @brief getSymbolPos
         * @param s
         * @return the position of symbol s.
         */
        int     getSymbolPos(string s);
        /**
         * @brief getSymbol
         * @param pos
         * @return the symbol at position pos.
         */
		Symbol	*getSymbol(int pos) const;
        /**
         * @brief setSymbol Sets the symbol at position pos.
         * @param pos
         * @param s
         */
		void	setSymbol(int pos,Symbol *s);
        /**
         * @brief getLength
         * @return the length of rule (number of symbols).
         */
        int     getLength() const;
        /**
         * @brief printRule
         * @param genome
         * @param pos
         * @param redo
         * @return in string format the rule evaluated for chromosome genome.
         */
		string	printRule(vector<int> genome,int &pos,int &redo);
        /**
         * @brief getValue
         * @param genome
         * @param pos
         * @param redo
         * @param stack
         * @param X
         * @return
         */
		double	getValue(vector<int> genome,int &pos,int &redo,DoubleStack &stack,double *X);
		~Rule();
};
# define __RULE__H
# endif
