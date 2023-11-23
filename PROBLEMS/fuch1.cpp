#include <PROBLEMS/fuch1.h>

Fuch1::Fuch1()
{
    trainSet = NULL;
    mlp = NULL;
    mlp1= NULL;
    mlp2= NULL;
    int dim = 2*(patternDimension+2)*nodes;
    setDimension(dim);
    left.resize(dim);
    right.resize(dim);
    for(int i=0;i<dim;i++)
    {
        left[i]=-100.0;
        right[i]=100.0;
    }
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
/*    if(mlp==NULL)
        mlp=new MlpProblem();
    mlp->setDimension(x.size());
    mlp->setWeights(x);*/
    if(mlp1==NULL)
    {
        mlp1= new MlpProblem();
        mlp2= new MlpProblem();
    }
    mlp1->setDimension(x.size()/2);
    mlp2->setDimension(x.size()/2);
    Data w1,w2;
    w1.resize(x.size()/2);
    w2.resize(x.size()/2);
    for(int i=0;i<x.size()/2;i++)
    {
        w1[i]=x[i];
        w2[i]=x[x.size()/2+i];
    }
    mlp1->setWeights(w1);
    mlp2->setWeights(w2);
    Data xx;
    xx.resize(1);
    const double q00=0.1;
    const double q01=3.0;
    const double d=50.0*1e-10;

    double q;
    double sum = 0.0;
    for(int i=0;i<npoints;i++)
    {
        q = q00+i*(q01-q00)/npoints;
        xx[0]=q;
        //double omega = mlp->getOutput(xx);
        double omega1=mlp1->getOutput(xx);
        double omega2=mlp2->getOutput(xx);
        double d1 = e2(omega1)*q1(q,omega1,d)/(e1(omega1)*q2(q,omega1,d))+coth2(q2(q,omega1,d)/2.0);
        if(isnan(d1)) d1=100;
        double d2 = e2(omega2)*q1(q,omega2,d)/(e1(omega2)*q2(q,omega2,d))+tanh2(q2(q,omega2,d)/2.0);
        if(isnan(d2)) d2=100;
        sum+=d1*d1+d2*d2;
        //double dv = e2(omega)*q1(q,omega,d)/(e1(omega)*q2(q,omega,d))+tanh2(q2(q,omega,d)/2.0);
        //if(isnan(dv)) dv = 100.0;
        //sum+=dv * dv;
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
    const double q00=0.1;
    const double q01=3.0;
    const double d=50.0*1e-10;

    double q;
    for(int i=0;i<npoints;i++)
    {
        q = q00+i*(q01-q00)/npoints;
        xx[0]=q;
        double omega1=mlp1->getOutput(xx);
        double omega2=mlp2->getOutput(xx);
        printf("%lf %lf %lf\n",q,fabs(omega1)/33.29,fabs(omega2)/33.29);
        //double omega = mlp->getOutput(xx);

        //printf("%lf %lf\n",q,fabs(omega/33.29));
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
    delete mlp1;
    delete mlp2;
}
