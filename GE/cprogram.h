# ifndef __CPROGRAM__H
# include <GE/symbol.h>
# include <GE/rule.h>
# include <GE/fparser.hh>
# include <GE/program.h>
# include <MLMODELS/model.h>
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
        Cprogram(int dim,Model *m);
        void makeGrammar();
		int	Parse(string expr);
		double	Eval(const double *X);
        double  EvalDeriv(const double *X,int pos);
        double  EvalDeriv2(const double *X,int pos);
		int	EvalError();
		Symbol	*getStartSymbol();
        virtual double	fitness(vector<int> &genome);
        virtual ~Cprogram();
};
# define __CPROGRAM__H
# endif
