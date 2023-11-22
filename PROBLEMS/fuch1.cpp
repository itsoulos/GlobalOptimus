#include <PROBLEMS/fuch1.h>

Fuch1::Fuch1()
{
    trainSet = NULL;
    mlp = NULL;
    int dim = (patternDimension+2)*nodes;//for omega
    setDimension(dim);
    left.resize(dim);
    right.resize(dim);
    //original [0.0,1.5]
    const double omegat2=33.29;
    for(int i=0;i<dim-1;i++)
    {
        left[i]=-100.0;
        right[i]=100.0;
    }
    //left[dim-1]=0.0*omegat2;
    //right[dim-1]=1.5*omegat2;
}
static double coth2(double x)
{
    return (exp(2.0*x)+1)/(exp(2.0*x)-1);
}

static double tanh2(double x)
{
    return (exp(2*x)-1)/(exp(2*x)+1);
}

void    Fuch1::makeDataset(double omega)
{
    if(trainSet==NULL)
    trainSet = new Dataset();
    Matrix xdata;
    Data ydata;
    xdata.resize(npoints);
    ydata.resize(npoints);
    const double q00=0.0;
    const double q01=3.0;
    const double d=50.0*1e-10;

    double q;
    for(int i=0;i<npoints;i++)
    {
        q = q00+i*(q01-q00)/npoints;
        xdata[i].resize(1);
        xdata[i][0]=q;
        ydata[i]=coth2(q2(q,omega,d)/2.0);
        if(isnan(ydata[i]) || isinf(ydata[i]))
        {
            if(i==0)
                ydata[i]=0.0;
            else
            {
                xdata[i]=xdata[i-1];
                ydata[i]=ydata[i-1];
            }
        }
    }
    trainSet->setData(xdata,ydata);
}
double Fuch1::e1(double omega)
{
    const double einf1=8.16;
    const double omegal1=50.09;
    const double omegat2=33.29;
    const double omegat1=44.88;
    double omegatilde = omega/omegat2;
    return einf1-(omegatilde*omegatilde-(omegal1/omegat2)*(omegal1/omegat2))/
                       (omegatilde*omegatilde-(omegat1/omegat2)/(omegat1/omegat2));
}

double Fuch1::e2(double omega)
{
    const double einf2=10.89;
    const double omegal1=50.09;
    const double omegal2=36.25;
    const double omegat2=33.29;
    const double omegat1=44.88;
    double omegatilde = omega/omegat2;
    return einf2-(omegatilde*omegatilde-(omegal2/omegat2)*(omegal2/omegat2))/
                       (omegatilde*omegatilde-(omegat2/omegat2)/(omegat2/omegat2));
}

double Fuch1::q1(double qtilde,double omega,double d)
{
    const double hbar= 6.5821*1e-16;

    const double c = 3*1.0e+8;
    double omegat2 = 33.29;
    double omegatilde = omega/omegat2;
    return sqrt(qtilde *qtilde-omegatilde*omegatilde*d*d*((omegat2/1e+3)/(hbar))/((omegat2/1e+3)/(hbar))*e1(omega)/(c*c));
}

double Fuch1::q2(double qtilde,double omega,double d)
{
    const double hbar= 6.5821*1e-16;
    const double c = 3*1.0e+8;
    double omegat2 = 33.29;
    double omegatilde = omega/omegat2;
    return sqrt(qtilde *qtilde-omegatilde*omegatilde*d*d*((omegat2/1e+3)/(hbar))/((omegat2/1e+3)/(hbar))*e2(omega)/(c*c));
}



double Fuch1::funmin(Data &x)
{
    if(mlp==NULL)
        mlp=new MlpProblem();
    mlp->setDimension(x.size());
    mlp->setWeights(x);
    Data xx;
    xx.resize(1);
    const double q00=0.0;
    const double q01=3.0;
    const double d=50.0*1e-10;

    double q;
    double sum = 0.0;
    for(int i=0;i<npoints;i++)
    {
        q = q00+i*(q01-q00)/npoints;
        xx[0]=q;
        double omega = mlp->getOutput(xx);

        double dv = e2(omega)*q1(q,omega,d)/(e1(omega)*q2(q,omega,d))+tanh2(q2(q,omega,d)/2.0);
        if(isnan(dv)) dv = 100.0;
       // printf("Omega = %lf Dv=%lf  \n",omega,dv);
        sum+=dv * dv;
    }
    return sum;
     /* double omega = x[0];
    if(mlp==NULL)
        mlp = new MlpProblem();
    makeDataset(omega);
    Data w;
    w.resize(x.size()-1);
    for(int i=0;i<x.size()-1;i++)
        w[i]=x[i];
    mlp->setDimension(w.size());
    mlp->setWeights(w);
    mlp->setTrainSet(trainSet);
    double dv = mlp->getTrainError();

    return dv;*/
   /* double dv=0.0;
    double omega = x[0];
    double d=50.0*1e-10;
    const double qtilde = 0.6;
    dv = e2(omega)*q1(qtilde,omega,d)/(e1(omega)*q2(qtilde,omega,d))+coth2(q2(qtilde,omega,d)/2.0);
    dv = dv * dv;
    return dv ;*/
}

Data    Fuch1::gradient(Data &x)
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

void    Fuch1::init(QJsonObject &params)
{

}

QJsonObject Fuch1::done(Data &x)
{
    funmin(x);
    Data xx;
    xx.resize(1);
    const double q00=0.0;
    const double q01=3.0;
    const double d=50.0*1e-10;

    double q;
    for(int i=0;i<npoints;i++)
    {
        q = q00+i*(q01-q00)/npoints;
        xx[0]=q;
        double omega = mlp->getOutput(xx);

        printf("%lf %lf\n",q,fabs(omega/33.29));
        //if(isnan(omega)) continue;
        //printf("Omega = %lf Dv=%lf  \n",omega,dv);

    }
    QJsonObject t;
    const double omegat2=33.29;
    t["omega"]=x[x.size()-1]/omegat2;
    return t;
}

Fuch1::~Fuch1()
{
    delete mlp;
}
