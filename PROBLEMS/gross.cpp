#include <PROBLEMS/gross.h>
# include <QFile>
# include <QTextStream>
double param ;
Gross::Gross()
{
    mlp = NULL;
    rbf = NULL;
    int dim = (patternDimension+2)*nodes;
   // dim = nodes * patternDimension+nodes+nodes;
    setDimension(dim);
    left.resize(dim);
    right.resize(dim);
    for(int i=0;i<dim;i++)
    {
        left[i]=-1000.0;
        right[i]=1000.0;
    }

}
double Gross::neural(Data &x)
{
	return mlp->getOutput(x);
	return 1.0/mlp->getOutput(x);
}

double Gross::neuralDeriv(Data &x)
{
	return mlp->getDerivative1(x,0);
	return -1.0/(pow(mlp->getOutput(x),2.0))*mlp->getDerivative1(x,0);
}

double Gross::neuralDeriv2(Data &x)
{
	return mlp->getDerivative2(x,0);
	double d0=mlp->getOutput(x);
	double d1=mlp->getDerivative1(x,0);
	double d2=mlp->getDerivative2(x,0);
	return 2*d0*d1*1.0/pow(d0,4)*d1+d2*(-1.0/(d0*d0));
}

double Gross::model(Data &x)
{
  //  return x[0]*(x[0]-1)*rbf->getOutput(x);
  
    return x[0]*(1-x[0])*neural(x);
}

double Gross::modelDeriv2(Data &x)
{
   /* return 2.0 * rbf->getOutput(x)+
           (4.0*x[0]-2.0)*rbf->getDerivative(x,0)+
           (x[0]*x[0]-x[0])*rbf->getSecondDerivative(x,0);
*/

    return -2.0 * neural(x)+
            (2.0-4.0*x[0])*neuralDeriv(x)+
            (x[0]-x[0]*x[0])*neuralDeriv2(x);
}
double Gross::vext(double x)
{
    return 0.0;
}
double Gross::funmin(Data &x)
{
    if(mlp==NULL)
        mlp=new MlpProblem();
    if(rbf==NULL)
        rbf= new RbfProblem();
   // rbf->setDimension(x.size());
   // rbf->setParameters(x);
    mlp->setDimension(1);//x.size());
    const double n=1.0;
    param = x[0];
    double en = 500.0;// n *n* M_PI*M_PI;
    Data w;
    w.resize(x.size());
    for(int i=0;i<w.size();i++) w[i]=x[i];
    mlp->setWeights(w);
    Data xx;
    xx.resize(1);
    double sum = 0.0;
    const double x0=0.0;
    const double x1=1.0;
    gamma =436.7686;
    int countZero = 0;

    mlp->resetViolationPercent(10.0);
    for(int i=1;i<npoints-1;i++)
    {
          xx[0]=x0+i*(x1-x0)/(npoints-1.0);
        double dder = modelDeriv2(xx);
        double dval = model(xx);
        double vextvalue = vext(xx[0])*dval;
        double gammavalue = gamma *dval*dval*dval;
        double leftPart =(-dder+vextvalue+gammavalue);
        double rightPart = en *dval;

	double div = leftPart/rightPart;
        sum+=pow(div-1.0,2.0);
    }
    return sum;//+100.0 * mlp->getViolationPercent();

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
        st<<xx[0]<<"\t"<<neural(xx)<<"\t"<<dval<<"\n";
    }
    fp.close();

    for(int i=0;i<x.size();i++) printf("%lf ",x[i]);
    printf("\n");
    return t;
}

Gross::~Gross()
{
    delete rbf;
    delete mlp;
}
