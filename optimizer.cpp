#include "optimizer.h"
Optimizer::Optimizer()
{
    myProblem = NULL;
    addParam(Parameter("opt_debug","yes","Set it to yes to show messages"));
}

void        Optimizer::setProblem(Problem *p)
{
    myProblem = p;
}

void        Optimizer::addParam(Parameter p)
{
   params[p.getName()]=p.getValue();
   paramsHelp[p.getName()]=p.getHelp();
}

void        Optimizer::setParam(QString name,QString value,QString help)
{
    params[name]=value;
    paramsHelp[name]=help;
}

Parameter   Optimizer::getParam(QString name)
{
    if(!params.contains(name))
    {
        Parameter pt(name,"","");
        return pt;
    }
    Parameter pt;
    pt.setName(name);
    pt.setValue(params[name].toString());
    pt.setHelp(paramsHelp[name].toString());
    return pt;

}

void    Optimizer::init()
{

}

void    Optimizer::step()
{

}

bool    Optimizer::terminated()
{
    return true;
}


void    Optimizer::showDebug()
{

}
void    Optimizer::solve()
{
    bool debug = getParam("opt_debug").getValue()=="yes"?true:false;
    init();
    do
    {
        step();
        if(debug) showDebug();
    }while(!terminated());

}

double  Optimizer::localSearch(Data &x)
{
    return 0.0;
}

Problem     *Optimizer::getProblem()
{
    return myProblem;
}

QStringList Optimizer::getParameterNames() const
{
    return params.keys();
}

Optimizer::~Optimizer()
{

}
