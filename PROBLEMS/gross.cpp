#include <PROBLEMS/gross.h>
# include <QFile>
# include <QTextStream>
Gross::Gross()
{
    mlp = NULL;
    int dim = (patternDimension+2)*nodes;
    setDimension(dim);
    left.resize(dim);
    right.resize(dim);
    for(int i=0;i<dim;i++)
    {
        left[i]=-20.0;
        right[i]=20.0;
    }

}
double Gross::model(Data &x)
{
    return x[0]*(x[0]-1)*mlp->getOutput(x);
}

double Gross::modelDeriv2(Data &x)
{

    return 2.0 * mlp->getOutput(x)+
            (4.0*x[0]-2.0)*mlp->getDerivative1(x,0)+
            (x[0]*x[0]-x[0])*mlp->getDerivative2(x,0);
}
double Gross::vext(double x)
{
    return 0.0;
}
double Gross::funmin(Data &x)
{
    if(mlp==NULL)
        mlp=new MlpProblem();
    mlp->setDimension(x.size());
    const double n=1.0;
    double en = n *n* M_PI*M_PI;
    mlp->setWeights(x);
    Data xx;
    xx.resize(1);
    double sum = 0.0;
    const double x0=0.0;
    const double x1=1.0;
    gamma =0;//9.1865;
    bool allZero = true;

    for(int i=0;i<npoints;i++)
    {
          xx[0]=x0+i*(x1-x0)/(npoints-1.0);
        double dder = modelDeriv2(xx);
        double dval = model(xx);
        double vextvalue = vext(xx[0])*dval;
        double gammavalue = gamma *pow(dval,3.0);
        double leftPart =(-dder+vextvalue+gammavalue);
        double rightPart = en *dval;
       if(fabs(dval)>1e-1) allZero = false;

        sum+=pow(leftPart-rightPart,2.0);
    }
    if(allZero) sum+=1000.0;
    return sum;
}

Data    Gross::gradient(Data &x)
{
    Data g;
    g.resize(x.size());
    for(int i=0;i<getDimension();i++)
    {
        double eps=pow(1e-18,1.0/3.0)*qMax(1.0,fabs(x[i]));
        x[i]+=eps;
        double v1=statFunmin(x);
        x[i]-=2.0 *eps;
        double v2=statFunmin(x);
        g[i]=(v1-v2)/(2.0 * eps);
        x[i]+=eps;
    }
    return g;
}

void    Gross::init(QJsonObject &params)
{

}

QJsonObject Gross::done(Data &x)
{
    funmin(x);
    QJsonObject t;
    QFile fp("grossplot.txt");
    fp.open(QIODevice::WriteOnly);
    QTextStream st(&fp);
    Data xx;
    xx.resize(1);
    const double x0=0.0;
    const double x1=1.0;
    for(int i=0;i<npoints;i++)
    {
        xx[0]=x0+i*(x1-x0)/(npoints-1.0);
        double dval = model(xx);
        st<<xx[0]<<"\t"<<dval<<"\n";
    }
    fp.close();

    return t;
}

Gross::~Gross()
{
    delete mlp;
}
