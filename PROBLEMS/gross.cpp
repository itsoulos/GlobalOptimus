#include <PROBLEMS/gross.h>
# include <QFile>
# include <QTextStream>
Gross::Gross()
{
    mlp = NULL;
    int dim = 2*(patternDimension+2)*nodes;
    setDimension(dim);
    left.resize(dim);
    right.resize(dim);
    for(int i=0;i<dim;i++)
    {
        left[i]=-20.0;
        right[i]=20.0;
    }

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
    gamma =0.0;//9.1865;
    bool allZero = true;

    for(int i=0;i<npoints;i++)
    {
          xx[0]=x0+i*(x1-x0)/(npoints-1.0);
        double dder = mlp->getDerivative2(xx,0);
        double dval = mlp->getOutput(xx);
        double vextvalue = vext(xx[0])*dval;
        double gammavalue = gamma *pow(dval,3.0);
        double leftPart =(-dder+vextvalue+gammavalue);
        double rightPart = en *dval;
        if(fabs(dval)>1e-1) allZero = false;
        sum+=pow(leftPart-rightPart,2.0);
    }
    xx[0]=x0;
    double dval0=mlp->getOutput(xx);
    double penalty1 = 100.0 *pow(dval0-0.0,2.0);
    xx[0]=x1;
    double dval1=mlp->getOutput(xx);
    double penalty2 = 100.0 *pow(dval1-0.0,2.0);
    if(allZero) sum+=1000.0;
    return sum+penalty1+penalty2;
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
        double dval = mlp->getOutput(xx);
        st<<xx[0]<<"\t"<<dval<<"\n";
    }
    fp.close();
    double en = x[x.size()-1];
    printf("EN = %lf \n",en);
    return t;
}

Gross::~Gross()
{
    delete mlp;
}
