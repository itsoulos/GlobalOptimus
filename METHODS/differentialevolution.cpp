#include "differentialevolution.h"

DifferentialEvolution::DifferentialEvolution()
{
    addParam(Parameter("de_np","10n","The number of agents. Default value 10n"));
    addParam(Parameter("de_f","0.8","De factor value"));
    addParam(Parameter("de_cr","0.9","DE CR parameter"));
    addParam(Parameter("de_maxiters","1000","DE maximum iters"));
    addParam(Parameter("de_termination","maxiters","De termination. Available values: maxiters,doublebox,similarity"));
}

void    DifferentialEvolution::init()
{
    if(getParam("de_np").getValue()=="10n")
        NP = 10 * myProblem->getDimension();
    else
        NP = getParam("de_np").getValue().toInt();
    F = getParam("de_f").getValue().toDouble();
    CR = getParam("de_cr").getValue().toDouble();
    maxiters = getParam("de_maxiters").getValue().toInt();
    iter = 0;

    //sampling
    for(int i=0;i<NP;i++)
    {
        Data x= myProblem->getSample();
        double y = myProblem->statFunmin(x);
        if(i==0 || y<besty)
        {
            besty = y;
            bestx = x;
        }
        agent.addPoint(x,y);
    }
}

void    DifferentialEvolution::step()
{
    ++iter;
    for(int i=0;i<NP;i++)
    {
        Data x;
        double y;
        agent.getPoint(i,x,y);
        int randomA,randomB,randomC;
        do
        {
            randomA = rand() % NP;
            randomB = rand() % NP;
            randomC = rand() % NP;
        }while(randomA == randomB || randomB == randomC || randomA == randomC);
        Data xa,xb,xc;
        double ya,yb,yc;
        agent.getPoint(randomA,xa,ya);
        agent.getPoint(randomB,xb,yb);
        agent.getPoint(randomC,xc,yc);
        int R = rand() % myProblem->getDimension();
        Data trialx = x;
        for(int j=0;j<myProblem->getDimension();j++)
        {
            double rj = rand() *1.0/RAND_MAX;
            if(rj<CR || j==R)
            {
                trialx[j]=xa[j]+F*(xb[j]-xc[j]);
            }
            else trialx[j]=x[j];
        }
        if(!myProblem->isPointIn(trialx)) trialx = x;
        double ft = myProblem->statFunmin(trialx);
        if(ft<y)
        {
            agent.replacePoint(i,trialx,ft);
            if(ft<besty)
            {
                besty = ft;
                bestx = trialx;
            }
        }
    }
}

bool    DifferentialEvolution::terminated()
{
    QString term = getParam("de_termination").getValue();
    if(term == "maxiters")
        return iter>=maxiters;
    else
        if(term == "doublebox")
        return doubleBox.terminate(besty);
    else
        if(term == "similarity")
        return similarity.terminate(besty);
}

void    DifferentialEvolution::showDebug()
{
    printf("DE. Iteration=%5d BEST VALUE=%20.10lg\n",
           iter,besty);
}

void    DifferentialEvolution::done()
{
    besty = localSearch(bestx);
}

DifferentialEvolution::~DifferentialEvolution()
{
    //nothing here
}
