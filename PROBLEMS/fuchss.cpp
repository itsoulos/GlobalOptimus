#include <PROBLEMS/fuchss.h>
FuchsS::FuchsS()
{
    setDimension(1);
    left.resize(1);
    right.resize(1);
    left[0]=0.01;
    right[0]=100;
}

double FuchsS::e1(double omega)
{
    const double einf1=8.16;
    const double omegal1=50.09;
    const double omegat2=33.29;
    const double omegat1=44.88;
      double omegatilde = omega/omegat2;
    return einf1-(omegatilde*omegatilde-(omegal1/omegat2)*(omegal1/omegat2))/
                         (omegatilde*omegatilde-(omegat1/omegat2)/(omegat1/omegat2));

}

double FuchsS::e2(double omega)
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

double FuchsS::q1(double qtilde,double omega,double d)
{    const double hbar= 6.5821*1e-16;

    const double c = 3*1.0e+8;
    double omegat2 = 33.29;
    double omegatilde = omega/omegat2;
    return sqrt(qtilde *qtilde+omegatilde*d*d*((omegat2/1e+3)/(hbar))/((omegat2/1e+3)/(hbar))*e1(omega)/(c*c));
}

double FuchsS::q2(double qtilde,double omega,double d)
{
    const double hbar= 6.5821*1e-16;
    const double c = 3*1.0e+8;
    double omegat2 = 33.29;
    double omegatilde = omega/omegat2;
    return sqrt(qtilde *qtilde+omegatilde*d*d*((omegat2/1e+3)/(hbar))/((omegat2/1e+3)/(hbar))*e2(omega)/(c*c));
}

double coth(double x)
{
    return (exp(2.0*x)+1)/(exp(2.0*x)-1);
}
double FuchsS::funmin(Data &x)
{
    double dv=0.0;
    double q=1.5;
    double omega = x[0];
    double d=50.0*1e-10;
    const double qtilde = 10.3;
    dv = e2(omega)*q1(qtilde,omega,d)/(e1(omega)*q2(qtilde,omega,d))+coth(q2(qtilde,omega,d)/2.0);
    printf("Omega = %lf Dv = %lf \n",omega,dv);
    return dv*dv ;
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
    t["omega"]=x[0];

    return t;
}

FuchsS::~FuchsS()
{

}
