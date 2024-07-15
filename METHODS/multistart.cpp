#include "multistart.h"
Multistart::Multistart()
{
    addParam(Parameter("ms_samples",100,10,10000,"Number of samples"));
    addParam(Parameter("ms_maxiters",100,1,10000,"Maximum number of iterations"));
    QStringList ms_termination;
    ms_termination<<"doublebox"<<"similarity"<<"maxiters";
    }

void    Multistart::init()
{
    minima.clear();
    iteration = 0;
    maxiters = getParam("ms_maxiters").getValue().toInt();
    nsamples = getParam("ms_samples").getValue().toInt();
}

void    Multistart::step()
{
    ++iteration;
    double y;
    Matrix xpoint;
    Data ypoint;
    sampleFromProblem(nsamples,xpoint,ypoint);
    for(int i=0;i<nsamples;i++)
    {
        Data x = xpoint[i];
        y = ypoint[i];
        y = localSearch(x);
        minima.addPoint(x,y);
    }

}

bool    Multistart::terminated()
{
    double besty,worsty;
    minima.getBestWorstValues(besty,worsty);
    QString termination = terminationMethod;

    if(termination == "doublebox" && doubleBox.terminate(besty)) return true;
    if(termination == "similarity" && similarity.terminate(besty)) return true;
    if(iteration>=maxiters) return true;
    return false;
}

void    Multistart::showDebug()
{
    double besty,worsty;
    minima.getBestWorstValues(besty,worsty);
    methodLogger->printMessage(QString::asprintf("MULTISTART. ITERATION=%4d BEST VALUE=%20.10lg",
                                                 iteration,besty));
}

void    Multistart::done()
{
    minima.sort();
    minima.getPoint(0,bestx,besty);
    double minx0=1e+100,maxx0=-1e+100;
    for(int i=0;i<minima.getSize();i++)
    {
        Data x;
        double y;
         minima.getPoint(i,x,y);
       // if(y<1e-1)
        {
             if(x[0]<minx0) minx0=x[0];
             if(x[0]>maxx0) maxx0=x[0];
         //printf("MINIMA X=[ %lf, %lf] Y=%lf\n",x[0]/33.29,x[1]/33.29,y);
        }
    }
    printf("MIN[0]=%lf MAX[0]=%lf \n",
           minx0/33.29,maxx0/33.29);
}

void    Multistart::getBest(Data &x,double &y)
{
    x = bestx;
    y = besty;
}

Multistart::~Multistart()
{
    //nothing here
}
