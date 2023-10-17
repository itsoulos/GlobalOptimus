#include "differentialevolution.h"

DifferentialEvolution::DifferentialEvolution()
{
    addParam(Parameter("de_np","10n","The number of agents. Default value 10n"));
    addParam(Parameter("de_f","0.8","De factor value"));
    addParam(Parameter("de_cr","0.9","DE CR parameter"));
    addParam(Parameter("de_tsize","8","Tournament size"));
    addParam(Parameter("de_maxiters","1000","DE maximum iters"));
    addParam(Parameter("de_selection","random","Selection method. Available values: random, tournament"));
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
    sampleFromProblem(NP,agentx,agenty);
    for(int i=0;i<NP;i++)
    {
        Data x= agentx[i];
        double y = agenty[i];
        if(i==0 || y<besty)
        {
            besty = y;
            bestx = x;
        }
    }
}

int     DifferentialEvolution::tournament()
{
    int tsize=params["de_tsize"].toString().toInt();
    int minindex=-1;
    double minvalue=1e+100;
    for(int i=0;i<tsize;i++)
    {
        int pos = rand()  % NP;

        double yy=agenty[pos];
        if(i==0 || yy<minvalue)
        {
            minindex=pos;
            minvalue=yy;
        }
    }
    return minindex;
}
void    DifferentialEvolution::step()
{
    ++iter;
    QString selection=params["de_selection"].toString();
    for(int i=0;i<NP;i++)
    {
        Data x;
        double y;
        x= agentx[i];
        y=agenty[i];
        int randomA,randomB,randomC;
        do
        {
            if(selection == "random")
            {
                randomA = rand() % NP;
                randomB = rand() % NP;
                randomC = rand() % NP;
            }
            else
            {
                randomA=tournament();
                randomB=tournament();
                randomC=tournament();
            }
        }while(randomA == randomB || randomB == randomC || randomA == randomC);
        Data xa,xb,xc;
        xa = agentx[randomA];
        xb = agentx[randomB];
        xc = agentx[randomC];
        int R = rand() % myProblem->getDimension();
        Data trialx = x;
        for(int j=0;j<myProblem->getDimension();j++)
        {
            double rj = myProblem->randomDouble();
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
            agentx[i]=trialx;
            agenty[i]=ft;
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
    return false;
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
