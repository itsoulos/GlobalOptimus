# ifndef __SYMBOL__H
# include <string>
# include <vector>
using namespace std;


class Rule;
/**
 * @brief The Symbol class Creates the symbols based on the grammar file and adds them to the rules
 */
class Symbol
{
	private:
		string name;
		vector<Rule*> rule;
		int	count_rules;
		int	is_terminal;
	public:
        /**
         * @brief Symbol The constructor of the class.
         */
		Symbol();
        /**
         * @brief set The name and the status (terminal, non terminal) for the symbol.
         * @param s
         * @param status
         */
		void	set(string s,int status);
        /**
         * @brief setName Sets the name of the symbol.
         * @param s
         */
		void	setName(string s);
        /**
         * @brief getName
         * @return The name of the symbol.
         */
		string	getName() const;
        /**
         * @brief setTerminalStatus Sets the terminal status (true,false).
         * @param status
         */
		void	setTerminalStatus(int status);
        /**
         * @brief getTerminalStatus
         * @return the terminal status.
         */
        int     getTerminalStatus() const;
        /**
         * @brief addRule Adds a new production rule for the current symbol.
         * @param r
         */
		void	addRule(Rule *r);
        /**
         * @brief cut Resizes the rules to N.
         * @param N
         */
		void	cut(int N);
        /**
         * @brief getRule
         * @param pos
         * @return the production rule at position pos.
         */
		Rule	*getRule(int pos) const;
        /**
         * @brief getCountRules
         * @return  The count of production rules.
         */
        int     getCountRules() const;
		~Symbol();	
};

# define __SYMBOL__H
# endif
