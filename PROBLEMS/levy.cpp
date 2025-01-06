#include <PROBLEMS/levy.h>
# define MARGIN_EPS 1e-5
# define LAMBDA     1e+5
Levy::Levy()
    :Problem(2)
{
    left.resize(2);
    right.resize(2);
    left[0]=0;
    left[1]=0;
    right[0]=1;
    right[1]=1;
}

int Levy::geteq()
{
    return 0;
}

int Levy::getineq()
{
    return 1;
}

double  Levy::funmin(Data &x)
{

    int fes=isFeasible(x);

    vector<double> temp_eq,temp_ineq;
    temp_eq.resize(geteq());
    temp_ineq.resize(getineq());
    feq(x,temp_eq);
    fineq(x,temp_ineq);
    double v1=0.0,v2=0.0,v3=0.0;

    for(int i=0;i<temp_eq.size();i++)
    {
        v2=v2+pow(temp_eq[i],2.0);
    }

    for(int i=0;i<temp_ineq.size();i++)
    {
        if(temp_ineq[i]>0) v3=v3+pow(temp_ineq[i],2.0);
    }
    v1=-x[0]-x[1];
    double l=qMax(LAMBDA,1000 * fabs(v1));


    return (v1+l*(v2+v3))+0.0;//100*(1.0-fes);
}

Data    Levy::gradient(Data &x)
{
    Data g;
    g.resize(dimension);
    for(int i=0;i<getdimension();i++)
    {
        double eps=pow(1e-18,1.0/3.0)*qMax(1.0,fabs(x[i]));
        x[i]+=eps;
        double v1=funmin(x);
        x[i]-=2.0 *eps;
        double v2=funmin(x);
        g[i]=(v1-v2)/(2.0 * eps);
        x[i]+=eps;
    }
    return g;
}

void	Levy::feq(vector<double> &x,vector<double> &eq)
{

}

void	Levy::fineq(vector<double> &x,vector<double> &ineq)
{
    double a=2.0;
    double b=0.25;
    ineq[0]=-(((x[0]-1)*(x[0]-1)+(x[1]-1))*(1/(2.0*a*a)-1.0/(2*b*b))+(x[0]-1)*(x[1]-1)*(1.0/(a*a)-1.0/(b*b))-1.0);
}

int     Levy::isFeasible(vector<double> &x)
{
    int neq=geteq();
    int nineq=getineq();
    double lmargin[getdimension()];
    double rmargin[getdimension()];
    getleftmargin(lmargin);
    getrightmargin(rmargin);

    for(int i=0;i<x.size();i++)
    {
        //if(x[i]-rmargin[i]>MARGIN_EPS || lmargin[i]-x[i]>MARGIN_EPS) return 0;
        if(x[i]>rmargin[i] || lmargin[i]>x[i]) return 0;
    }
    if(neq)
    {
        vector<double> k;
        k.resize(neq);
        feq(x,k);
        double eq_max=-1;
        double sum=0.0;
        for(int i=0;i<k.size();i++)
        {
            if(fabs(k[i])>eq_max) eq_max=fabs(k[i]);
            sum+=k[i]*k[i];
        }
        if(sum>1e-5) return 0;
        //if(eq_max>MARGIN_EPS*10) return 0;
    }
    vector<double> t;
    t.resize(nineq);
    fineq(x,t);
    if(t.size()) for(int i=0;i<t.size();i++) if(t[i]>MARGIN_EPS) {return 0;}
    return 1;
}

QJsonObject Levy::done(Data &x)
{
    funmin(x);
    int fes=isFeasible(x);
    printf("Problem is Feasible?%s\n",(fes==1)?"YES":"NO");
    return QJsonObject();
}

Levy::~Levy()
{

}
