#include <PROBLEMS/fuchss.h>
FuchsS::FuchsS()
{
    setDimension(2);
    //x[0]=q;
    //x[1]=omega;
    left.resize(2);
    right.resize(2);
    left[0]=0;
    right[0]=3;
    left[1]=0.9;
    right[1]=1.6;
}

double FuchsS::e1(double omega)
{
    const double hbar= 6.5821*1e-16;
    const double einf1=8.16;
    const double omegal1=50.09/hbar;
    const double omegat1=44.88/hbar;

    return einf1 * (omegal1*omegal1-omega*omega)/(omegat1*omegat1-omega*omega);
}

double FuchsS::e2(double omega)
{
    const double hbar= 6.5821*1e-16;

    const double einf2=10.89;
    const double omegal2=36.25/hbar;
    const double omegat2=33.29/hbar;
    return einf2 * (omegal2*omegal2-omega*omega)/(omegat2*omegat2-omega*omega);
}

double FuchsS::q1(double q,double omega,double d)
{    const double hbar= 6.5821*1e-16;

    const double c = 3*1.0e+8;
    double omegat2 = 33.29*1e+3/hbar;
     const double omegat1=44.88*1e+3/hbar;
    double omegatilde = omega/omegat2;
    double qtilde = q * d;
    return sqrt(qtilde *qtilde+omegatilde*d*d*omegat1*e1(omegatilde)/(c*c));
}

double FuchsS::q2(double q,double omega,double d)
{
    const double hbar= 6.5821*1e-16;
    const double c = 3*1.0e+8;
    double omegat2 = 33.29*1e+3/hbar;
    const double omegat1=44.88*1e+3/hbar;
    double omegatilde = omega/omegat2;
    double qtilde = q * d;
    return sqrt(qtilde *qtilde+omegatilde*d*d*omegat1*e2(omegatilde)/(c*c));
}

double coth(double x)
{
    return (exp(2.0*x)+1)/(exp(2.0*x)-1);
}
double FuchsS::funmin(Data &x)
{
    double dv=0.0;
    double q = x[0];
    double omega = x[1];
    double d=150.0;
    const double hbar= 6.5821*1e-16;
    const double c = 3*1.0e+8;
    double omegat2 = 33.29*1e+3/hbar;
    const double omegat1=44.88*1e+3/hbar;
    double omegatilde = omega/omegat2;
    double qtilde = q * d;
    dv = e2(omegatilde)*q1(q,omega,d)/(e1(omegatilde)*q2(q,omega,d))+coth(q2(q,omega,d)/2.0);
    printf("dv = %lf \n",dv);
    return dv ;
}

Data    FuchsS::gradient(Data &x)
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

void FuchsS::init(QJsonObject &params)
{

}

QJsonObject FuchsS::done(Data &x)
{
    QJsonObject t;
    t["q"]=x[0];
    t["omega"]=x[1];
    return t;
}

FuchsS::~FuchsS()
{

}