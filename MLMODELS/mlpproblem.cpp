#include "mlpproblem.h"
# include <QDebug>
# include <QRandomGenerator>
MlpProblem::MlpProblem()
    :Problem(1)
{
      addParam(Parameter("mlp_nodes",1,1,100,"Number of weights"));
      addParam(Parameter("mlp_leftmargin",-10.0,-10000.0,10000.0,"Initial left margin"));
      addParam(Parameter("mlp_rightmargin",10.0,-10000.0,10000.0,"Initial right margin"));
      QStringList methods;
      methods<<"smallvalues"<<"xavier";
      addParam(Parameter("mlp_initmethod",methods[0],methods,"Possible values: smallvalues,xavier"));
      QStringList boolValues;
      boolValues<<"false"<<"true";
      addParam(Parameter("mlp_usebound",boolValues[0],boolValues,"Use bound function for weights (true|false)"));
      addParam(Parameter("mlp_boundlimit",10.0,1.0,100.0,"Bound limit for weights"));
}

Data    MlpProblem::getSample()
{


      //init weights
      Data xx;
      xx.resize(dimension);


      double leftMargin = getParam("mlp_leftmargin").getValue().toDouble();
      double rightMargin = getParam("mlp_rightmargin").getValue().toDouble();
      QString initmethod = getParam("mlp_initmethod").getValue();

      if(initmethod == "smallvalues")
      {
          double a = -1;
          double b = 1;
          for(int i=0;i<dimension;i++)
          {
              xx[i]= (a+(b-a)*randomDouble());
          }
      }
      else
          if(initmethod=="xavier")
          {
              double a = -1.0/trainDataset->dimension();
              double b = 1.0/trainDataset->dimension();
              if(a<leftMargin) a= leftMargin;
              if(b>rightMargin) b= rightMargin;
              for(int i=0;i<dimension;i++)
              {
                  xx[i]=a+(b-a)*randomDouble();
              }
          }
          else
          {
              return Problem::getSample();
          }
          return xx;
}
void    MlpProblem::initWeights()
{
      int nodes = getParam("mlp_nodes").getValue().toInt();
      int k = (trainDataset->dimension()+2)*nodes;
      setDimension(k);
      left.resize(k);
      right.resize(k);
      double leftMargin = getParam("mlp_leftmargin").getValue().toDouble();
      double rightMargin = getParam("mlp_rightmargin").getValue().toDouble();
      for(int i=0;i<k;i++)
      {
          left[i]=leftMargin;
          right[i]=rightMargin;
      }
      weight.resize(k);

}

void    MlpProblem::initModel()
{
    initWeights();
}
void    MlpProblem::init(QJsonObject &pt)
{
    initParams(pt);
    if(pt.contains("mlp_nodes"))
    {
        setParam("mlp_nodes",pt["mlp_nodes"].toString());
    }
    if(pt.contains("mlp_leftmargin"))
    {
        setParam("mlp_leftmargin",pt["mlp_leftmargin"].toString());
    }
    if(pt.contains("mlp_rightmargin"))
    {
        setParam("mlp_rightmargin",pt["mlp_rightmargin"].toString());
    }
    if(pt.contains("mlp_initmethod"))
    {
        setParam("mlp_initmethod",pt["mlp_initmethod"].toString());
    }
    usebound_flag = getParam("mlp_usebound").getValue()=="false"?false:true;
    viollimit  = getParam("mlp_boundlimit").getValue().toDouble();
    loadTrainSet();
    loadTestSet();
    initWeights();
}

void    MlpProblem::setWeights(Data &w)
{
    weight  =w ;
}

/** edo ypologizoume to train error pou einai
 *  kai i timi pou elaxistopoioume **/
double MlpProblem::funmin(Data &x)
{
    weight  =x ;
    if(usebound_flag)
    {

        resetViolationPercent(viollimit);
        double dv = getTrainError();
        double tt = getViolationPercent();
        return dv*(1.0 + tt * tt);
    }
    return getTrainError();

}

/** edo epistrefoume tin paragogo tis
 *  synartisis funmin(x) os pros x**/
Data    MlpProblem::gradient(Data &x)
{
    Data g;
    weight = x;
    g.resize(weight.size());
    if(trainDataset==NULL) return g;
    if(usebound_flag)
    {
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
    for(int i=0;i<g.size();i++)
        g[i]=0.0;
    for(int i=0;i<trainDataset->count();i++)
    {
        Data xx = trainDataset->getXPoint(i);
        Data gtemp = getDerivative(xx);
        double per=getOutput(xx)-trainDataset->getYPoint(i);
        for(int j=0;j<g.size();j++)	g[j]+=gtemp[j]*per;
    }
    for(int j=0;j<x.size();j++) g[j]*=2.0;
    return g;
}

double  MlpProblem::sig(double x)
{
    return 1.0/(1.0+exp(-x));
}

double  MlpProblem::sigder(double x)
{
    double s = sig(x);
    return s*(1.0-s);
}

void	MlpProblem::resetViolationPercent(double limit)
{
	violcount=0;
	sigcount=0;
	viollimit=limit;
}

double  MlpProblem::getViolationPercent()
{
	return violcount*100.0/sigcount;
}

/** einai i exodos tou neuronikou gia to protypo x**/
double  MlpProblem::getOutput(Data  &x)
{
    return getOutput(x.data());
}

double  MlpProblem::getOutput(double  *x)
{
    double arg=0.0;
    double per=0.0;
    int d = trainDataset->dimension();
    int nodes = weight.size()/(d+2);
    for(int i=1;i<=nodes;i++)
    {
        arg=0.0;
        sigcount++;
        for(int j=1;j<=d;j++)
        {
            int pos=(d+2)*i-(d+1)+j-1;
            arg+=weight[pos]*x[j-1];

        }
        arg+=weight[(d+2)*i-1];
        if(fabs(arg)>=viollimit)
        {

            violcount++;
        }
        per+=weight[(d+2)*i-(d+1)-1]*sig(arg);
    }
    return per;
}

/** einai i paragogos tou neuronikou os
 *  pros to protypo x**/
Data    MlpProblem::getDerivative(Data &x)
{
    return getDerivative(x.data());
}


Data    MlpProblem::getDerivative(double *x)
{

    double arg;
    double f,f2;
    int nodes = weight.size()/(trainDataset->dimension()+2);
    int d = trainDataset->dimension();
    Data G;
    G.resize(weight.size());

    for(int i=1;i<=nodes;i++)
    {
        arg = 0.0;
        for(int j=1;j<=d;j++)
        {
            arg+=weight[(d+2)*i-(d+1)+j-1]*x[j-1];
        }
        arg+=weight[(d+2)*i-1];
        f=sig(arg);
        f2=f*(1.0-f);
        G[(d+2)*i-1]=weight[(d+2)*i-(d+1)-1]*f2;
        G[(d+2)*i-(d+1)-1]=f;
        for(int k=1;k<=d;k++)
        {
            G[(d+2)*i-(d+1)+k-1]=
                x[k-1]*f2*weight[(d+2)*i-(d+1)-1];
        }
    }
    return G;
}
void    MlpProblem::enableBound()
{
    usebound_flag=true;
}
double  MlpProblem::getDerivative1(vector<double> xpoint,int pos)
{
    int nodes=weight.size()/ (xpoint.size() + 2);
    int dimension = xpoint.size();
    double per=0.0;
    for(int i=1;i<=nodes;i++)
    {
        double arg=0.0;
        for(int j=1;j<=dimension;j++)
        {
            int mypos=(dimension+2)*i-(dimension+1)+j;
            arg+=xpoint[j-1]*weight[mypos-1];
        }
        arg+=weight[(dimension+2)*i-1];
        double s=sig(arg);
        per+=weight[(dimension+2)*i-(dimension+1)-1]*s*(1.0-s)*weight[(dimension+2)*i-(dimension+1)+pos-1];
    }
    return per;
}

void    MlpProblem::disableBound()
{
    usebound_flag = false;
}
double	MlpProblem::getDerivative2(vector<double> xpoint,int pos)
{
    int nodes=weight.size()/ (xpoint.size() + 2);
    int dimension = xpoint.size();
        double per=0.0;
        for(int i=1;i<=nodes;i++)
        {
            double arg=0.0;
            for(int j=1;j<=dimension;j++)
            {
                int mypos=(dimension+2)*i-(dimension+1)+j;
                arg+=xpoint[j-1]*weight[mypos-1];
            }
            arg+=weight[(dimension+2)*i-1];
            double s=sig(arg);
            double w1=weight[(dimension+2)*i-(dimension+1)-1];
            double w2=weight[(dimension+2)*i-(dimension+1)+pos-1];
            per+=w1*w2*w2*s*(1.0-s)*(1.0-2*s);
        }
        return per;
}

class SimanBounds
{
private:
    MlpProblem *problem;
    Data left,right;
    double T0;
    int neps;
    int k;
    Data X0;
    void    reduceTemp();
    double  fitness(Data &xl,Data &xr);
    void    randomBounds(Data &xl,Data &xr);
    double  besty;
    QRandomGenerator gen;
public:
    SimanBounds(Data &x0,MlpProblem *p,Data &xl,Data &xr);
    void    Solve();
    void    getBounds(Data &xl,Data &xr);
    ~SimanBounds();
};


SimanBounds::SimanBounds(Data &x0,MlpProblem *p,Data &xl,Data &xr)
{
    X0 = x0;
    problem = p;
    left = xl;
    right = xr;
    k=0;
    T0=1e+8;
    besty = fitness(xl,xr);
    neps = 50;
    gen.seed(1);
}

double  SimanBounds::fitness(Data &xl,Data &xr)
{
    return problem->getViolationPercentInBounds(10.0,xl,xr);
}

void    SimanBounds::reduceTemp()
{
    k=k+1;
    const double alpha = 0.8;
    T0 =T0 * pow(alpha,k);

}


void    SimanBounds::randomBounds(Data &xl,Data &xr)
{
    for(int i=0;i<(int)xl.size();i++)
    {
        double mid = left[i]+(right[i]-left[i])/2.0;
        double l = left[i]+gen.generateDouble()*(mid-left[i]);
        double r = mid+gen.generateDouble()*(right[i]-mid);
        if(l<X0[i])
            l=X0[i]-0.05 * fabs(X0[i]);
        if(r>X0[i])
            r = X0[i]+0.05 *fabs(X0[i]);
        xl[i]=l;
        xr[i]=r;
        //printf("In %lf ->[%lf,%lf]\n",X0[i],l,r);
    }
}
void    SimanBounds::Solve()
{
    Data xl=left,xr=right;
    Data globalLeft = left;
    Data globalRight = right;
    double ypoint = besty;
    while(true)
    {
        Data testLeft = xl;
        Data testRight = xr;
        for(int i=1;i<=neps;i++)
        {
            randomBounds(testLeft,testRight);
            double y = fitness(testLeft,testRight);
            if(y<ypoint)
            {
                ypoint = y;
                xl = testLeft;
                xr = testRight;
                if(ypoint<besty)
                {
                    besty=ypoint;
                    globalLeft = xl;
                    globalRight = xr;
                }
            }
            else
            {
                double r = (rand()*1.0/RAND_MAX);
                double ratio = exp(-(y-ypoint)/T0);
                double xmin = ratio<1?ratio:1;
                if(r<xmin)
                {
                    ypoint = y;
                    xl = testLeft;
                    xr = testRight;
                    if(ypoint<besty)
                    {
                        besty=ypoint;
                        globalLeft = xl;
                        globalRight = xr;
                    }
                }
            }
        }
        reduceTemp();
        if(T0<1e-6) break;
      //  printf("Siman Bounds Generation: %4d Besty: %10.5lf\n",
      //         k,besty);
    }
    left = globalLeft;
    right = globalRight;
}

void    SimanBounds::getBounds(Data &xl,Data &xr)
{
    xl = left;
    xr = right;
}

SimanBounds::~SimanBounds()
{

}

void    MlpProblem::findBoundsWithSiman(Data &x0,Data &xl,Data &xr)
{
    SimanBounds b(x0,this,xl,xr);
    b.Solve();
    b.getBounds(xl,xr);
}

double  MlpProblem::getViolationPercentInBounds(double limit,Data &lb,Data &rb)
{
    const int nsamples =100;
    randomize(1);
    Data sample;
    int n = getDimension();
    sample.resize(n);

    double sum=0.0;
    for(int i=0;i<nsamples;i++)
    {
        for(int j=0;j<n;j++)
        {
            sample[j]=lb[j]+randomDouble()*(rb[j]-lb[j]);
        }
        setWeights(sample);
        resetViolationPercent(limit);
        double v=getTrainError();
        sum = sum+v*(1.0+getViolationPercent()/100.0);
    }
    sum/=nsamples;
  //  printf("Fitness sum = %.10lg\n",sum);
    return sum;
}

QJsonObject MlpProblem::done(Data &x)
{
    double tr=funmin(x);
    QJsonObject xx;
    double tt=getTestError(testDataset);
    double tc=getClassTestError(testDataset);
    xx["trainError"]=tr;
    xx["testError"]=tt;
    xx["classError"]=tc;
    double precision=0.0,recall=0.0,f1score=0.0;
    getPrecisionRecall(precision,recall,f1score);
    xx["precision"]=precision;
    xx["recall"]=recall;
    xx["f1score"]=f1score;
    return xx;

}

Data    MlpProblem::getSampleNoViolate()
{
    Data x;
    const int iters = 100;
    double minViolate = 1e+100;
    Data bestx = x;
    x.resize(getDimension());
    for(int i=1;i<=iters;i++)
    {
        for(int j=0;j<getDimension();j++)
            x[j]=0.01 * (2.0*randomDouble()-1.0);
        resetViolationPercent(10.0);
        double y = getTrainError();
        double p = getViolationPercent();
        p  = y *(1.0+p);
        if(p<minViolate)
        {
            minViolate=p;
            bestx  = x;
        }
    }
    return bestx;
}

MlpProblem::~MlpProblem()
{
}
