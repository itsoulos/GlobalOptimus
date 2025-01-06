#ifndef DIRACPROBLEM_H
#define DIRACPROBLEM_H
# include <OPTIMUS/problem.h>

typedef double(*MFUNCTION)(double,void*);

class DiracProblem : public Problem
{
private:
    int nodes;
    int points;
    double xmm;
    double xpm;
    double xnm;
    int     range;
    double  hbc,anor,aa,az,an,c,b,cha,rm;
    Data uf,wf,vf;
    Data ug,wg,vg;
    Data gyf,gfdyf;
    Data gyg,gfdyg;
    Data ww,gv,grid;
    double al;
    double lastEnergy;
    double          rho(double r,double c,double b,double cha);
    void            gauleg(double X1,double X2,Data &X,Data &W,int N);
    double          Simpsons_Rule_Sum_LR( double a, double h, int n,MFUNCTION f,void *param);
    double          potl(double r,double c,double b,double cha);
    double          pot(double r,double c,double b,double cha);
    void            selectMaterial(int index);
    void            nets(double &ann,double &fdann, double r,Data u,Data w,Data v);
    void            funs(double &y,double &fdy,double r,Data u,Data w,Data v,double al);
public:
    DiracProblem();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
    virtual QJsonObject done(Data &x);
    ~DiracProblem();
};

#endif // DIRACPROBLEM_H
