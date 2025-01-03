#include <GE/ruleprogram.h>
#define NANVALUE 1e+100
double cmyif(const double *x)
{
    if(fabs(x[0])>1e-7)
        return x[1];
    else return NANVALUE;
}

double cmyelse(const double *x)
{
    return x[0];
}
vector<string> split (string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

RuleProgram::RuleProgram(int dim,Model *m)
    :Cprogram(dim,m)
{
    mparser.resize(10);
    for(int i=0;i<mparser.size();i++)
    {
        mparser[i].AddFunction("myif",cmyif,2);
        mparser[i].AddFunction("myelse",cmyelse,1);
    }
}

double	RuleProgram::fitness(vector<int> &genome)
{
    int redo = 0;

    string str = printRandomProgram(genome,redo);
    if(redo>=REDO_MAX) return NANVALUE;
    clist = split(str,"#");

    //QString qstr=str.c_str();
    //QStringList subStr  = qstr.split("#");
    for(int i=0;i<clist.size();i++)
    {
        string st =clist[i];
      //  printf("String = %s\n",st.c_str());
        int d = mparser[i].Parse(st,vars);
        if(d!=-1)
        {
       //     printf("Wrong expression %s value = %d \n",st.c_str(),d);
            return NANVALUE;
        }

    }

    lastExpr = str;
    return currrentModel->getTrainError();
}

double  RuleProgram::getOutput(const double *x)
{
    double finalValue = NANVALUE;
    for(int j=0;j<clist.size();j++)
    {

        double vv = mparser[j].Eval(x);
        if(vv>-1e+10)
        {
            finalValue = vv;
            break;
        }
    }
    double ff = finalValue;
    return ff;
}


void RuleProgram::makeRules()
{
    ifexpr.set("IFEXPR",0);
    boolexpr.set("BOOLEXPR",0);
    ifexpr2.set("IFEXPR2",0);
    cAnd.set("&",1);
    cOr.set("|",1);
    cEq.set("=",1);
    cNeq.set("!=",1);
    cGt.set(">",1);
    cGe.set(">=",1);
    cLt.set("<",1);
    cLe.set("<=1",1);
    cNot.set("!",1);
    cIf.set("if",1);
    comma.set(",",1);
    separator.set("#",1);
    cMyIf.set("myif",1);
    cMyElse.set("myelse",1);
    relop.set("RELOP",0);
    boolop.set("BOOLOP",0);
    ifexpr.set("IFEXPR",0);
    boolexpr.set("BOOLEXPR",0);
    ifexpr2.set("IFEXPR2",0);
    int r;
    r=newRule();
    rule[r]->addSymbol(&ifexpr);
    Start.addRule(rule[r]);
    setStartSymbol(&Start);

    r=newRule();
    rule[r]->addSymbol(&cMyIf);
    rule[r]->addSymbol(&Lpar);
    rule[r]->addSymbol(&boolexpr);
    rule[r]->addSymbol(&comma);
    rule[r]->addSymbol(&Expr);
    rule[r]->addSymbol(&Rpar);
    ifexpr2.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&cMyIf);
    rule[r]->addSymbol(&Lpar);
    rule[r]->addSymbol(&boolexpr);
    rule[r]->addSymbol(&comma);
    rule[r]->addSymbol(&Expr);
    rule[r]->addSymbol(&Rpar);
    rule[r]->addSymbol(&separator);
    rule[r]->addSymbol(&ifexpr2);
    ifexpr2.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&ifexpr2);
    rule[r]->addSymbol(&separator);
    rule[r]->addSymbol(&cMyElse);
    rule[r]->addSymbol(&Lpar);
    rule[r]->addSymbol(&Expr);
    rule[r]->addSymbol(&Rpar);
    ifexpr.addRule(rule[r]);


    r=newRule();
    rule[r]->addSymbol(&Expr);
    rule[r]->addSymbol(&relop);
    rule[r]->addSymbol(&Expr);
    boolexpr.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&Expr);
    rule[r]->addSymbol(&relop);
    rule[r]->addSymbol(&Expr);
    rule[r]->addSymbol(&boolop);
    rule[r]->addSymbol(&boolexpr);
    boolexpr.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&cGt);
    relop.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&cGe);
    relop.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&cLt);
    relop.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&cLe);
    relop.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&cEq);
    relop.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&cNeq);
    relop.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&cAnd);
    boolop.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&cOr);
    boolop.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&Lpar);
    rule[r]->addSymbol(&Expr);
    rule[r]->addSymbol(&Rpar);
    Expr.addRule(rule[r]);


    r=newRule();
    rule[r]->addSymbol(&function);
    rule[r]->addSymbol(&Lpar);
    rule[r]->addSymbol(&Expr);
    rule[r]->addSymbol(&Rpar);
    Expr.addRule(rule[r]);


    r=newRule();
    rule[r]->addSymbol(&Expr);
    rule[r]->addSymbol(&binaryop);
    rule[r]->addSymbol(&Expr);
    Expr.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&Lpar);
    rule[r]->addSymbol(&Minus);
    rule[r]->addSymbol(&Lpar);
    rule[r]->addSymbol(&Expr);
    rule[r]->addSymbol(&Rpar);
    rule[r]->addSymbol(&Rpar);
     Expr.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&Digit0);
    DigitList.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&Digit0);
    rule[r]->addSymbol(&Digit0);
//    rule[r]->addSymbol(&DigitList);
    DigitList.addRule(rule[r]);


    r=newRule();
    rule[r]->addSymbol(&Digit0);
    rule[r]->addSymbol(&Digit0);
    rule[r]->addSymbol(&Digit0);
//    rule[r]->addSymbol(&DigitList);
    DigitList.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&DigitList);
    rule[r]->addSymbol(&Dot);
    rule[r]->addSymbol(&DigitList);
    terminal.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&XXlist);
    terminal.addRule(rule[r]);
    Expr.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&terminal);
    Expr.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&Lpar);
    rule[r]->addSymbol(&XXlist);
    rule[r]->addSymbol(&binaryop);
    rule[r]->addSymbol(&DigitList);
    rule[r]->addSymbol(&Dot);
    rule[r]->addSymbol(&DigitList);
    rule[r]->addSymbol(&Rpar);
    // Expr.addRule(rule[r]);


    r=newRule();
    rule[r]->addSymbol(&Plus);
    binaryop.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&Minus);
    binaryop.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&Mult);
    binaryop.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&Div);
    binaryop.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&Sin);
    function.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&Cos);
    function.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&Exp);
    function.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&Log);
    function.addRule(rule[r]);

    r=newRule();
    rule[r]->addSymbol(&Lpar);
    rule[r]->addSymbol(&DigitList);
    rule[r]->addSymbol(&Dot);
    rule[r]->addSymbol(&DigitList);

    rule[r]->addSymbol(&binaryop);
    rule[r]->addSymbol(&XXlist);
    rule[r]->addSymbol(&Rpar);
   //Expr.addRule(rule[r]);


}

RuleProgram::~RuleProgram()
{
    //nothing here
}
