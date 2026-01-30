# ifndef __CPROGRAM__H
# include <GE/symbol.h>
# include <GE/rule.h>
# include <GE/fparser.hh>
# include <GE/program.h>
# include <MLMODELS/model.h>
/**
 * @brief The Cprogram class implements a basic program
 * in C - like language used in the Grammatical Evolution.
 */
class Cprogram : public Program
{
	protected:
        Model       *currrentModel;
		string		vars;
		FunctionParser 	parser;
		int			dimension;
        vector<Rule*> rule;
		Symbol		Start, Expr, function, binaryop, terminal,
					XXlist,DigitList, Digit0, Digit1, MinMax;
		Symbol		Sin, Cos, Exp, Log, Abs, Sqrt, Min, PI, Max;
		Symbol		Plus, Minus, Mult, Div, Pow;
		Symbol		Lpar, Rpar, Dot, Comma;
		Symbol		Tan, Int, Log10;
		vector<Symbol>	Digit;
		vector<Symbol>	XX;
		int			newRule();
        virtual void			makeTerminals();
        virtual void			makeNonTerminals();
        virtual void			makeRules();
    public:
        /**
         * @brief Cprogram The constructor of the class.
         * @param dim The input dimension
         * @param m The used machine learning model
         */
        Cprogram(int dim,Model *m);
        /**
         * @brief makeGrammar Creates the grammar of the program.
         */
        void makeGrammar();
        /**
         * @brief Parse
         * @param expr
         * @return 1 if the program can be parsed from the grammar
         */
		int	Parse(string expr);
        /**
         * @brief Eval
         * @param X
         * @return the value of the program estimated at X
         */
		double	Eval(const double *X);
        /**
         * @brief EvalDeriv
         * @param X
         * @param pos
         * @return the derivative of the program evaluated at X and
         * for pos variable.
         */
        double  EvalDeriv(const double *X,int pos);
        /**
         * @brief EvalDeriv2
         * @param X
         * @param pos
         * @return the second derivative of the program evaluated at X and
         * for pos variable.
         */
        double  EvalDeriv2(const double *X,int pos);
        /**
         * @brief EvalError
         * @return 1 if there was an evaluation error in the parsing process.
         */
        int     EvalError();
        /**
         * @brief getStartSymbol
         * @return  the start symbol of the grammar.
         */
        Symbol          *getStartSymbol();
        /**
         * @brief fitness
         * @param genome
         * @return the fitness value for chromosome genome.
         */
        virtual double	fitness(vector<int> &genome);

        int    setChromosomeInParser(vector<int> &genome);
        virtual ~Cprogram();
};
# define __CPROGRAM__H
# endif
