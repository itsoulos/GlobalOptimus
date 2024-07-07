#include <METHODS/simanmethod.h>

SimanMethod::SimanMethod()
{
    addParam(Parameter("siman_t0",100000.0,100.0,1e+8,"Initial temperature"));
    addParam(Parameter("siman_neps",100,10,10000,"Number of iterations"));
    addParam(Parameter("siman_eps",0.00001,0.0,1.0,"Small value for termination"));
    QStringList siman_coolmethod;
    siman_coolmethod<<"exp"<<"log"<<"linear"<<"quad";
    addParam(Parameter("siman_coolmethod",siman_coolmethod[0],
                       siman_coolmethod,
                       "The cooling method. Available values: exp,log,linear,quad"));
    hasInitialized = false;
}
void    SimanMethod::setPoint(Data &x,double &y)
{
    hasInitialized = true;
    xpoint =x ;
    ypoint = y;
    bestx = x;
    besty = y;
}

void    SimanMethod::getPoint(Data &x,double &y)
{
    x = bestx;
    y = besty;
}

bool    SimanMethod::terminated()
{
    if(T0<=eps)
    {
        return true;
    }
 //   printf("Temperature: %20.10lg Value: %20.10lg\n",T0,besty);
    return false;
}

void    SimanMethod::updateTemp()
{
    const double alpha = 0.8;

    if(coolingMethod=="exp")
    {
        T0 =T0 * pow(alpha,k);
        k=k+1;
    }
    else
        if(coolingMethod=="log")
        {
            T0 = T0/(1+alpha * log(1.0+k));
        }
        else
            if(coolingMethod == "linear")
            {
                T0 = T0/(1.0+alpha * k);
            }
            else
                if(coolingMethod=="quad")
                {
                    T0 = T0/(1.0+alpha * k * k);
                }
}

void    SimanMethod::step()
{
    int i;
    for(i=1;i<=neps;i++)
    {
        Data y = myProblem->getSample();
        double fy = myProblem->statFunmin(y);
        if(fy<ypoint)
        {
            xpoint = y;
            ypoint = fy;
            if(ypoint<besty)
            {
                bestx = xpoint;
                besty = ypoint;
            }
        }
        else
        {
            double r = fabs(myProblem->randomDouble());
            double ratio = exp(-(fy-ypoint)/T0);
            double xmin = ratio<1?ratio:1;
            if(r<xmin)
            {
                xpoint = y;
                ypoint = fy;
                if(ypoint<besty)
                {
                    bestx = xpoint;
                    besty = ypoint;
                }
            }
        }
    }
    updateTemp();
}

void    SimanMethod::init()
{
    k=1;
    if(!hasInitialized)
    {
    xpoint = myProblem->getSample();
    ypoint = myProblem->statFunmin(xpoint);
    }
    else hasInitialized = false;
    bestx = xpoint;
    besty = ypoint;
    T0=getParam("siman_t0").getValue().toDouble();
    neps = getParam("siman_neps").getValue().toInt();
    eps  = getParam("siman_eps").getValue().toDouble();
    coolingMethod= getParam("siman_coolmethod").getValue();
}

void    SimanMethod::done()
{
    if(getParam("opt_localsearch").getValue()!="none")
    besty = localSearch(bestx);

}

SimanMethod::~SimanMethod()
{

}
