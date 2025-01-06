#ifndef RULEPROGRAM_H
#define RULEPROGRAM_H

# include <GE/cprogram.h>

class RuleProgram : public Cprogram
{
private:
    string lastExpr;
    QStringList list;
    vector<string> clist;
    Symbol ifexpr,boolexpr,ifexpr2;
    Symbol      cAnd,cOr,cEq,cNeq,boolop;
    Symbol      cGt,cGe,cLt,cLe,cNot,cIf,comma,separator,cMyIf,cMyElse,relop;
    vector<FunctionParser> mparser;
    virtual void			makeRules();
public:
    RuleProgram(int dim,Model *m);
    virtual double	fitness(vector<int> &genome);
    double  getOutput(const double *x);
    virtual ~RuleProgram();
};

#endif // RULEPROGRAM_H
