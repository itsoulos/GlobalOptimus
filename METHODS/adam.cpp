#include "adam.h"

Adam::Adam()
{
    QStringList options;
    options<<"none"<<"golden"<<"fibonacci"<<"armijo";
    addParam(Parameter("adam_rate",0.01,0.0,1.0,"Adam learning rate"));
    addParam(Parameter("adam_linesearch",
                       "none",options,
                       "Line search used. Values: none,golden,fibonacci,armijo"));
    addParam(Parameter("adam_maxiters",10000,100,100000,"Maximum number of iterations"));
    addParam(Parameter("adam_b1",0.9,0.0,1.0,"B1 parameter"));
    addParam(Parameter("adam_b2",0.999,0.0,1.0,"B2 parameter"));

    haveInitialized = false;
    lt = NULL;
}

void    Adam::updateRate()
{
    if(linesearch == "golden")
    {
        double lambda = learningRate;
        double a = lambda * 0.9;
        double b = lambda * 1.1;
        if(lt==NULL)
            lt = new GoldenSearch(myProblem);
        ((GoldenSearch *)lt)->setParams(a,b);

        learningRate = lt->getDirection(xpoint);
    }
    else
        if(linesearch == "fibonacci")
        {
            double lambda = learningRate;
            double a = lambda * 0.9;
            double b = lambda * 1.1;
            if(lt==NULL)
                lt = new FibonacciSearch(myProblem);
            ((FibonacciSearch *)lt)->setParams(a,b);
            learningRate = lt->getDirection(xpoint);
        }
        else
            if(linesearch == "armijo")
            {
                double lambda  = 2;
                if(lt == NULL)
                    lt = new ArmijoSearch(myProblem);
                ((ArmijoSearch*)lt)->setLambda(lambda);

                learningRate = lt->getDirection(xpoint);

            }
}

void    Adam::init()
{

    learningRate = getParam("adam_rate").getValue().toDouble();
    linesearch   = getParam("adam_linesearch").getValue();
    iterations   = getParam("adam_maxiters").getValue().toInt();
    b1           = getParam("adam_b1").getValue().toDouble();
    b2           = getParam("adam_b2").getValue().toDouble();
    if(!haveInitialized)
    {
        xpoint = myProblem->getSample();
        ypoint = myProblem->statFunmin(xpoint);
    }
    haveInitialized = false;
}

void    Adam::step()
{
    double f=0.0;
    Data gradient=xpoint;
    bool debug = getParam("opt_debug").getValue()=="yes"?true:false;

    rM = myProblem->getRightMargin();
    lM = myProblem->getLeftMargin();
    M_0.resize(myProblem->getDimension(),0.0);
    U_0.resize(myProblem->getDimension(),0.0);
    for(int i = 0; i < iterations; i++){
        updateRate();
        double a = learningRate * sqrt( 1.0 - pow(b2,i+1) ) / (1.0 - pow(b1,i+1));
        gradient = myProblem->gradient(xpoint);
        for(unsigned j = 0; j < gradient.size(); j++){
            M_0[j] = b1 * M_0[j] + (1-b1) * gradient[j];
            U_0[j] = b2 * U_0[j] + (1-b2) * gradient[j] * gradient[j];
            xpoint[j] -= a * M_0[j] / (sqrt(U_0[j]) + 1e-1) ;
            if(xpoint[j] > rM[j]) xpoint[j] = rM[j];
            if(xpoint[j] < lM[j]) xpoint[j] = lM[j];
        }
        f=myProblem->statFunmin(xpoint);
        if(i % 10 == 0 && debug)
            methodLogger->printMessage(
                QString::asprintf("ADAM ITER: %4d ERROR %10.5lf ",
                                  i,f));

    }
    ypoint = f;

}

bool    Adam::terminated()
{
    return true;
}

void    Adam::setPoint(Data &x,double &y)
{
    xpoint  =x ;
    ypoint  =y;
    haveInitialized = true;
}

void    Adam::getPoint(Data &x,double &y)
{
    x=xpoint;
    y=ypoint;
}

Adam::~Adam()
{
    if(lt!=NULL)
        delete lt;
}
