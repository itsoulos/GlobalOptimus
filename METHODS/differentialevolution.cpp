#include "differentialevolution.h"

DifferentialEvolution::DifferentialEvolution()
{
    addParam(Parameter("de_np","10n","The number of agents. Default value 10n"));
    addParam(Parameter("de_f",0.8,0.0,1.0,"De factor value"));
    addParam(Parameter("de_cr","0.9","DE CR parameter"));
    addParam(Parameter("de_tsize",8,2,20,"Tournament size"));
    addParam(Parameter("de_maxiters",1000,10,10000,"DE maximum iters"));

    QStringList de_fselection;
    de_fselection<<"number"<<"ali"<<"random";
    addParam(Parameter("de_fselection",de_fselection[0],de_fselection,"The differential weight method. Values: number, ali, random"));

    QStringList yes_no;
    yes_no<<"no"<<"yes";
    addParam(Parameter("de_localsearch",yes_no[0],yes_no,"Perform local search at every iteration. Values: no,yes"));

    QStringList de_selection;
    de_selection<<"random"<<"tournament";
    addParam(Parameter("de_selection",de_selection[0],
                       de_selection,
                       "Selection method. Available values: random, tournament"));
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
    int tsize=getParam("de_tsize").getValue().toInt();
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
    QString selection=getParam("de_selection").getValue();
    QString  de_fselection = getParam("de_fselection").getValue();
    double bestMax = 0;
    double bestMin = 0;
    bool is_local = false;
    if(de_fselection == "ali")
    {

        bestMax = *max_element(agenty.begin(), agenty.end());
        bestMin = *min_element(agenty.begin(),agenty.end());
        if (bestMax/bestMin <1)
            F = 1.0 - (bestMax/bestMin);
        else
            F = 1.0 - (bestMin/bestMax);
    }
    if(getParam("de_localsearch").getValue()=="yes")
        is_local = true;
    bool is_random = de_fselection=="random";
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
                if(is_random)
                    F= -0.5 +2.0 * myProblem->randomDouble();
                trialx[j]=xa[j]+F*(xb[j]-xc[j]);
            }
            else trialx[j]=x[j];
        }
        if(!myProblem->isPointIn(trialx)) trialx = x;
        double ft = is_local?localSearch(trialx):myProblem->statFunmin(trialx);
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
    QString term = terminationMethod;
    if(term == "maxiters")
        return iter>=maxiters;
    else
        if(term == "doublebox")
        return iter>=maxiters || doubleBox.terminate(besty);
    else
        if(term == "similarity")
        return iter>=maxiters || similarity.terminate(besty);
    return false;
}

void    DifferentialEvolution::showDebug()
{
    bool debug = getParam("opt_debug").getValue()=="yes"?true:false;
    if(debug)
        methodLogger->printMessage(QString::asprintf("DE. Iteration=%5d BEST VALUE=%20.10lg",
                                                     iter,besty));
}

void    DifferentialEvolution::done()
{
    besty = localSearch(bestx);
    showDebug();
}

DifferentialEvolution::~DifferentialEvolution()
{
    //nothing here
}
