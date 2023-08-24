#include "optimizer.h"
# include <METHODS/gradientdescent.h>
# include <METHODS/bfgs.h>
Optimizer::Optimizer()
{
    myProblem = NULL;
    addParam(Parameter("opt_debug","yes","Set it to yes to show messages"));
    addParam(Parameter("opt_localsearch","bfgs","The method used in localsearch"));
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
    if(help.size()!=0)
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
    done();
}

double  Optimizer::localSearch(Data &x)
{

    QString localMethod = getParam("opt_localsearch").getValue();
    double y=1e+10;
    Optimizer *local=NULL;
    if(localMethod == "gradient")
    {
        local = new GradientDescent();
        local->setProblem(myProblem);
        local->setParam("opt_debug","no");
        ((GradientDescent *)local)->setParam("gd_linesearch","armijo");
         y = myProblem->statFunmin(x);
        ((GradientDescent *)local)->setPoint(x,y);
        local->solve();
        ((GradientDescent *)local)->getPoint(x,y);

    }
    else
    if(localMethod == "bfgs")
    {
        local = new Bfgs();
        local->setProblem(myProblem);
        ((Bfgs *)local)->setPoint(x,y);
        local->solve();
        ((Bfgs *)local)->getPoint(x,y);
    }
    else
    if(localMethod == "lbfgs")
    {

    }
    if(local!=NULL) delete local;
    return y;
}

void    Optimizer::done()
{

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
