#include <PROBLEMS/salkin.h>
# define LAMBDA     1e+5
# define MARGIN_EPS 1e-5

Salkin::Salkin()
    :Problem(5)
{
    left.resize(5);
    right.resize(5);
    left[0]=1;left[1]=80;left[2]=30;left[3]=145;left[4]=0;
    right[0]=4;right[1]=88;right[2]=35;right[3]=150;right[4]=2;
}

double  Salkin::funmin(Data &x)
{
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
    v1= -(3*x[0]+x[1]+2*x[2]+x[3]-x[4]);
    double l=qMax(LAMBDA,1000 * fabs(v1));


    return (v1+l*(v2+v3))+0.0;//100*(1.0-fes);
}

Data    Salkin::gradient(Data &x)
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

void	Salkin::feq(vector<double> &x,vector<double> &eq)
{

}

void	Salkin::fineq(vector<double> &x,vector<double> &ineq)
{
    ineq[0]=25*x[0]-40*x[1]+16*x[2]+21*x[3]+x[4]-300.0;
    ineq[1]=x[0]+20*x[1]-50*x[2]+x[3]-x[4]-200.0;
    ineq[2]=60*x[0]+x[1]-x[2]+2*x[3]+x[4]-600.0;
    ineq[3]=-7*x[0]+4*x[1]+15*x[2]-x[3]+65*x[4]-700.0;
}

int     Salkin::isFeasible(vector<double> &x)
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

QJsonObject Salkin::done(Data &x)
{
    funmin(x);
    int fes=isFeasible(x);
    printf("Problem is Feasible?%s\n",(fes==1)?"YES":"NO");
    return QJsonObject();
}

int Salkin::geteq()
{
    return 0;
}

int Salkin::getineq()
{
    return 4;
}

Salkin::~Salkin()
{

}
