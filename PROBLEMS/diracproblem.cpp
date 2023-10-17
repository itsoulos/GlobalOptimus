#include <PROBLEMS/diracproblem.h>
DiracProblem::DiracProblem()
{
    xpm=938.27;
    xnm=939.57;
    xmm=105.65;
    hbc=197.325;
}

double  DiracProblem::rho(double r,double c,double b,double cha)
{
  return cha/(1 + exp( (r-c)/b) );
}

void    DiracProblem::gauleg(double X1,double X2,Data &X,Data &W,int N)
{
  X.resize(N);
  W.resize(N);
  const double eps=3.0e-14;
  int M=(N+1)/2;
  double XM=0.5*(X2+X1);
  double XL=0.5*(X2-X1);
  double Z,P1,P2,P3,PP,Z1;
  for(int I=1;I<=M;I++)
  {
      Z=cos(M_PI*(I-0.25)/(N+0.5));
  label1:
      P1=1.0;
      P2=0.0;
      for(int J=1;J<=N;J++)
      {
          P3=P2;
          P2=P1;
          P1=((2.0*J-1.0)*Z*P2-(J-1.0)*P3)/J;
      }
      PP=N*(Z*P1-P2)/(Z*Z-1.0);
      Z1=Z;
      Z=Z1-P1/PP;
      if(fabs(Z-Z1)>eps) goto label1;
      X[I-1]=XM-XL*Z;
      X[N+1-I-1]=XM+XL*Z;
      W[I-1]=2.0*XL/((1.0-Z*Z)*PP*PP);
      W[N+1-I-1]=W[I-1];
  }
}

double  DiracProblem::Simpsons_Rule_Sum_LR( double a, double h, int n,MFUNCTION f,void *param)
{
  return 0.0;
}

double  DiracProblem::potl(double r,double c,double b,double cha)
{
  const double esq=1.44;
  const double cc=1.781;
  const int rmax=20;
  const double zero=0.0;
  const int nop=points;
  const double rcw = 386.11;
  Data xx,ww;
  double fact,sum=0.0,rp,ad,ff;
  fact = -2*acos(-1.0)*esq;
  gauleg(zero,rmax,xx,ww,nop);
  sum = 0.0;
  for(int i=1;i<=nop;i++)
  {
      rp=xx[i-1];
      ad=fabs(r-rp);
      ff=ad*(log(cc*ad/rcw)-1)-(r+rp)*(log(cc*(r+rp)/rcw)-1);
      sum = sum + ww[i-1]*xx[i-1]*rho(xx[i-1],c,b,cha)*ff;
  }
  return fact*sum/r;
}

double  DiracProblem::pot(double r,double c,double b,double cha)
{
  const double esq=1.44;
  const int rmax=25;
  const double zero=0.0;
  const int nop=points;
  Data xx,ww;
  double fact = -4*acos(-1.0)*esq;
  double rr,sum=0.0,rest;
  if(r>rmax)
  {
      rr = rmax;
      gauleg(zero,rr,xx,ww,nop);
      sum = 0;
      for(int i=1;i<=nop;i++)
          sum = sum + ww[i-1]*pow(xx[i-1],2)*rho(xx[i-1],c,b,cha);
      rest = 0.;
  }
  else
  {
      rr = r;
      gauleg(zero,rr,xx,ww,nop);
      sum = 0;
      for(int i=1;i<=nop;i++)
          sum = sum + ww[i-1]*pow(xx[i-1],2)*rho(xx[i-1],c,b,cha);
      gauleg(rr,rmax,xx,ww,nop);
      rest = 0;
      for(int i=1;i<=nop;i++)
          rest = rest + ww[i-1]*xx[i-1]*rho(xx[i-1],c,b,cha);
  }
  return fact*(sum/r + rest);
}

void    DiracProblem::selectMaterial(int index)
{
  setDimension(6*nodes+1);
  left.resize(dimension);
  right.resize(dimension);
  for(int i=0;i<dimension-1;i++)
  {
      left[i]=-10.0;
      right[i]= 10.0;
  }
  left[dimension-1]=0.15;
  right[dimension-1]=0.35;


  //tables from materials;
  QVector<double> AA;
  AA<<28<<48<<64<<98<<124<<156<<186<<208<<238;
  QVector<double> AZ;
  AZ<<14<<22<<30<<42<< 50<< 64<< 74<< 82<<92;
  QVector<double> C;
  C<<3.14<<3.843<<4.285<<5.107<<5.49<<5.93<<6.58<<6.61<<6.874;
  QVector<double> B;
  B<<0.537<<0.588<<0.584<<0.569<<0.534<<0.576<<0.48<<0.545<<0.556;
  QVector<int> Range;
  Range<<70<<60<<60<<50<<50<<40<<40<<40<<40;

  aa=AA[index];
  az=AZ[index];
  c=C[index];
  b=B[index];
  range=Range[index];
  int nop=points;
  gyf.resize(nop);gfdyf.resize(nop);
  gyg.resize(nop);gfdyg.resize(nop);
  ww.resize(nop);gv.resize(nop);

  uf.resize(nodes);
  wf.resize(nodes);
  vf.resize(nodes);
  ug.resize(nodes);
  wg.resize(nodes);
  vg.resize(nodes);
  double mycharge=0.0;
  mycharge=3.0*az/(4.0*M_PI*c*c*c*(1.0+pow((M_PI*b/c),2)));
  cha = mycharge;
  rm=(az*xpm+an*xnm)*xmm/(az*xpm+an*xnm+xmm) ;

  an=aa-az;
  double x1 = 0;
  double x2 = range;

  gauleg(x1,x2,grid,ww,points);
  gv.resize(points);
  for(int j=1;j<=points;j++)
  {
      double r=grid[j-1];
      gv[j-1]=pot(r,c,b,cha);
      gv[j-1]=gv[j-1]+2/137.037/3./M_PI*(potl(r,c,b,cha)-5./6*gv[j-1]);
  }
}

void    DiracProblem::nets(double &ann,double &fdann, double r,Data u,Data w,Data v)
{
    ann=0.0;
    fdann=0.0;
    for(int i=1;i<=nodes;i++)
    {
        double z=w[i-1]*r+u[i-1];
        double sig=1.0/(1.0+exp(-z));
        double fdsig=sig*(1.0-sig);
        ann=ann+v[i-1]*sig;
        fdann=fdann+w[i-1]*v[i-1]*fdsig;
    }
}

void    DiracProblem::funs(double &y,double &fdy,double r,Data u,Data w,Data v,double al)
{
    double ann,fdann;
    nets(ann,fdann,r,u,w,v);
    double b=r*exp(-al*r);
    double fdb=exp(-al*r)-al*b;
    y=b*ann;
    fdy=fdb*ann+b*fdann;
}

double  DiracProblem::funmin(Data &x)
{
    const int nop=points;

    for(int i=0;i<nodes;i++)
    {
        uf[i]=x[i];
        wf[i]=x[nodes+i];
        vf[i]=x[2*nodes+i];
        ug[i]=x[3*nodes+i];
        wg[i]=x[4*nodes+i];
        vg[i]=x[5*nodes+i];
    }
    al=x[dimension-1];
    anor = 0.0;
    double abnor = 0.0;
    double ener = 0.0;
    double y,fdy;
    for(int j=1;j<=nop;j++)
    {
        double r=grid[j-1];
        funs(y,fdy,r,uf,wf,vf,al);
        gyf[j-1]=y;
        gfdyf[j-1]=fdy;
        funs(y,fdy,r,ug,wg,vg,al);
        gyg[j-1]=y;
        gfdyg[j-1]=fdy;

        double sos =  pow(gyg[j-1],2) + pow(gyf[j-1],2);
        double dos =  pow(gyg[j-1],2) - pow(gyf[j-1],2);

        anor = anor + ww[j-1]*sos;
        abnor = abnor +ww[j-1]*dos;
        ener = ener + ww[j-1]*gv[j-1]*dos;
    }
    ener = (ener+ rm*anor)/abnor;
    lastEnergy=ener-xmm;
    double funmin_0 = 0;
    for(int j=1;j<=nop;j++)
    {
        double r = grid[j-1];
        double xffp =  gfdyf[j-1]+gyf[j-1]/r-(rm - ener + gv[j-1])*gyg[j-1]/hbc;
        double xggp =  gfdyg[j-1]-gyg[j-1]/r-(rm + ener - gv[j-1])*gyf[j-1]/hbc;
        funmin_0 = funmin_0 + pow(xffp,2) + pow(xggp,2);
    }
    funmin_0 = funmin_0/anor;
    return funmin_0;
}

Data    DiracProblem::gradient(Data &x)
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

void    DiracProblem::init(QJsonObject &params)
{
    if(params.contains("nodes"))
        nodes = params["nodes"].toString().toInt();
    else nodes=10;
    if(params.contains("points"))
        points=params["points"].toString().toInt();
    else points= 10;
    int material = 1;
    if(params.contains("material"))
        material = params["material"].toString().toInt();
    selectMaterial(material);
}

QJsonObject DiracProblem::done(Data &x)
{
    QJsonObject ret;
    ret["energy"]=lastEnergy;
    return ret;
}
DiracProblem::~DiracProblem()
{

}
