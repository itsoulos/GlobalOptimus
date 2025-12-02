#include "mlpproblem.h"
# include <QDebug>
using Interval = vector<double>; // size = 2*dim
using Point = vector<double>;
MlpProblem::MlpProblem()
    :Problem(1)
{
      addParam(Parameter("mlp_nodes",1,1,100,"Number of weights"));
      addParam(Parameter("mlp_leftmargin",-10.0,-10000.0,10000.0,"Initial left margin"));
      addParam(Parameter("mlp_rightmargin",10.0,-10000.0,10000.0,"Initial right margin"));
      QStringList methods;
      methods<<"smallvalues"<<"xavier"<<"random";
      addParam(Parameter("mlp_initmethod",methods[0],methods,"Possible values: smallvalues,xavier"));
      QStringList boolValues;
      boolValues<<"no"<<"yes";
      addParam(Parameter("mlp_usebound",boolValues[0],boolValues,"Use bound function for weights (yes|no)"));
      addParam(Parameter("mlp_boundlimit",10.0,1.0,100.0,"Bound limit for weights"));
      addParam(Parameter("mlp_balanceclass",boolValues[1],boolValues,"Enable or disabled balanced classes during train (yes|no)"));
      addParam(Parameter("mlp_outputfile","","The output file for mlp"));
      addParam(Parameter("mlp_usesimanbound",boolValues[0],boolValues,"Discover bounds using siman"));
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
          for(int i=0;i<dimension;i++)
          {
              double a = -1;
              double b = 1;
              if(left[i]>a) a =left[i];
              if(right[i]<b) b = right[i];
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
              xx= Problem::getSample();
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


}

void    MlpProblem::initModel()
{
    initWeights();
      QString siman = getParam("mlp_usesimanbound").getValue();
	if(siman == "yes")
	{
    		enableBound();
    		Data xl = left;
    		Data xr = right;
    		findBoundsWithSiman(xl,xr);
    		left  = xl;
    		right = xr;
    		setLeftMargin(xl);
    		setRightMargin(xr);
    		disableBound();
	}
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
    usebound_flag = getParam("mlp_usebound").getValue()=="no"?false:true;
    viollimit  = getParam("mlp_boundlimit").getValue().toDouble();
    useFitnessPerClass=getParam("mlp_balanceclass").getValue()=="no"?false:true;
    loadTrainSet();
    loadTestSet();
    initWeights();
}


/** edo ypologizoume to train error pou einai
 *  kai i timi pou elaxistopoioume **/
double MlpProblem::funmin(Data &x)
{
    //thread safe
    resetViolationPercent(viollimit);

    double error = 0.0;
    int xallsize  = xall.size();

    if(useFitnessPerClass)// && method!=NULL && !method->isLocalOptimizerRunning())
    {
        return getClassErrorPerClass(x,trainDataset);
    }
    for(int i=0;i<xallsize;i++)
    {
        Data xx = xall[i];

        double yy = trainDataset->getYPoint(i);

        double per = getOutput(x,xx.data());
        if(fabs(per)>=1e+10) return 1e+100;
        if(isnan(per))
        {
            /*hasDebug = true;
            per = getOutput(x,xx.data());
            hasDebug = false;*/
            return 1e+100;
        }
        if(isinf(per))
        {
            return 1e+100;
        }
        error+= (per-yy)*(per-yy);
    }

    if(usebound_flag || useFitnessPerClass)
    {
        double tt = getViolationPercent()/100.0;
        double lambda = 10;
        return error*(1.0+lambda * tt*tt);
    }

    return error;

}

/** edo epistrefoume tin paragogo tis
 *  synartisis funmin(x) os pros x**/
Data    MlpProblem::gradient(Data &x)
{
    Data g;
    g.resize(x.size());
    if(trainDataset==NULL) return g;
    if(usebound_flag || useFitnessPerClass)
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
        Data gtemp = getDerivative(x,xx.data());
        double per=getOutput(x,xx.data())-trainDataset->getYPoint(i);
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

double  MlpProblem::getOutput(Data &w,double *x)
{

    double arg=0.0;
    double per=0.0;
    int d = trainDataset->dimension();
    int nodes = w.size()/(d+2);
    for(int i=1;i<=nodes;i++)
    {
        arg=0.0;
        sigcount++;
        for(int j=1;j<=d;j++)
        {
            int pos=(d+2)*i-(d+1)+j-1;
            arg+=w[pos]*x[j-1];

        }
        arg+=w[(d+2)*i-1];
        if(fabs(arg)>=viollimit)
        {

            violcount++;
        }
        per+=w[(d+2)*i-(d+1)-1]*sig(arg);

    }
    return per;
}

double MlpProblem::getTestError(Data &x,Dataset *test)
{
    Matrix xtest = test->getAllXpoint();
    double error = 0.0;
    int xallsize  = xtest.size();
    for(int i=0;i<xallsize;i++)
    {
        Data xx = xtest[i];

        double yy = test->getYPoint(i);

        double per = getOutput(x,xx.data());
        if(fabs(per)>=1e+10) return 1e+100;
        if(isnan(per)) return 1e+100;
        if(isinf(per)) return 1e+100;
        error+= (per-yy)*(per-yy);
    }
    return error;
}

double MlpProblem::getClassTestError(Data &x,Dataset *test)
{
    Matrix xtest = test->getAllXpoint();
    double error = 0.0;
    int xallsize  = xtest.size();
    for(int i=0;i<xallsize;i++)
    {
        Data xx = xtest[i];

        double realClass = test->getClass(i);
        double per = getOutput(x,xx.data());
        double estClass = test->getClass(per);
        /** an apexei i pragmatiki klasi apo tin ypologismeni
         *  pano apo 10^-5, tote exoume sfalma **/
        error+= (fabs(estClass - realClass)>1e-5);
    }
    return error*100.0/test->count();
}
Data    MlpProblem::getDerivative(Data &w,double *x)
{

    double arg;
    double f,f2;
    int nodes = w.size()/(trainDataset->dimension()+2);
    int d = trainDataset->dimension();
    Data G;
    G.resize(w.size());

    for(int i=1;i<=nodes;i++)
    {
        arg = 0.0;
        for(int j=1;j<=d;j++)
        {
            arg+=w[(d+2)*i-(d+1)+j-1]*x[j-1];
        }
        arg+=w[(d+2)*i-1];
        f=sig(arg);
        f2=f*(1.0-f);
        G[(d+2)*i-1]=w[(d+2)*i-(d+1)-1]*f2;
        G[(d+2)*i-(d+1)-1]=f;
        for(int k=1;k<=d;k++)
        {
            G[(d+2)*i-(d+1)+k-1]=
                x[k-1]*f2*w[(d+2)*i-(d+1)-1];
        }
    }
    return G;
}

void    MlpProblem::enableBound()
{
    usebound_flag=true;
}
double  MlpProblem::getDerivative1(Data &weight,vector<double> xpoint,int pos)
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
double	MlpProblem::getDerivative2(Data &weight,vector<double> xpoint,int pos)
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
    int dim;
    Data left,right;
    double T0;
    int neps;
    int seed;

    double rand01(mt19937 &r);
    double evaluate_interval(const Interval &I, int dim, int samples, mt19937 &rng);
    Interval neighbor(const Interval &I,
                      double step,
                      const Point &GLOBAL_L,
                      const Point &GLOBAL_U,
                      mt19937 &rng);
    Point GLOBAL_L,GLOBAL_U;

public:
    SimanBounds(MlpProblem *p,Data &xl,Data &xr,int s=123);
    void    Solve();
    void    getBounds(Data &xl,Data &xr);
    ~SimanBounds();
};


SimanBounds::SimanBounds(MlpProblem *p,Data &xl,Data &xr,int s)
{

    problem = p;
    seed  = s;
    dim = p->getDimension();
    GLOBAL_L.resize(dim);
    GLOBAL_U.resize(dim);
    for(int i=0;i<dim;i++)
    {
        GLOBAL_L[i]=xl[i];
        GLOBAL_U[i]=xr[i];
    }
    left = xl;
    right = xr;
    neps=100;
    T0=3.0;
}

// υπολογισμός “ποιότητας” διαστήματος
double SimanBounds::evaluate_interval(const Interval &I, int dim, int samples, mt19937 &rng)
{
    double best = 1e100;
    double avg = 0.0;
    for(int k=0;k<samples;k++) {
        Point x(dim);
        for(int i=0;i<dim;i++) {
            double L = I[2*i];
            double U = I[2*i+1];
            uniform_real_distribution<double> ud(L,U);
            x[i] = ud(rng);
        }
        avg+=problem->statFunmin(x);
        best = min(best, problem->statFunmin(x));
    }
    return avg/samples;
}

Interval SimanBounds::neighbor(const Interval &I,
                  double step,
                  const Point &GLOBAL_L,
                  const Point &GLOBAL_U,
                  mt19937 &rng)
{
    normal_distribution<double> N(0,step);
    Interval J = I;

    // mutation
    for (double &v : J) 
    {
	    int direction = (rand()%2==1)?1:-1;
	    v = v +direction * N(rng)*v;
	    //v += N(rng);
    }


    /*
    // impose bounds
    for(int i=0;i<GLOBAL_L.size();i++){
        double &L = J[2*i];
        double &U = J[2*i+1];
        L = max(L, GLOBAL_L[i]);
        U = min(U, GLOBAL_U[i]);
        if(L > U) std::swap(L,U);
    }*/

    return J;
}

double SimanBounds::rand01(mt19937 &r) {
    return uniform_real_distribution<double>(0.0,1.0)(r);
}

void    SimanBounds::Solve()
{
    mt19937 rng(seed);

    double T = T0;
    double T_end=1e-3;
    double alpha=0.95;
    Interval I(2*dim);
    for(int i=0;i<dim;i++){
        double a = left[i];
        double b = right[i];
        double width = (b-a);
        double mid = a+width/2;
        double p = 0.1;

        I[2*i]   = mid-p *rand01(rng) * width;
        I[2*i+1] = mid+p *rand01(rng) * width;
    }
    double score = evaluate_interval(I,dim,25,rng);
    Interval bestI=I;
    double bestScore=score;
    while(T>T_end){
        for(int it=0;it<neps;it++){
            Interval J = neighbor(I, 0.05, GLOBAL_L, GLOBAL_U, rng);
            double s2  = evaluate_interval(J,dim,25,rng);

            double dE = s2-score;

            if(dE<0 || rand01(rng)<exp(-dE/T)){
                I=J;
                score=s2;
                if(score<bestScore){
                    bestScore=score;
                    bestI=I;
                }
            }
        }
        T*=alpha;
        printf("SIMAN. T=%lf BEST=%lf \n",T,bestScore);
    }
    cout<<"Best interval found: \n";
    for(int i=0;i<dim;i++){
        left[i]=bestI[2*i];
        right[i]=bestI[2*i+1];
        cout<<"param "<<i<<" in ["<<bestI[2*i]<<","<<bestI[2*i+1]<<"]\n";
    }
    cout<<"score="<<bestScore<<"\n";
}

void    SimanBounds::getBounds(Data &xl,Data &xr)
{
    xl = left;
    xr = right;
}

SimanBounds::~SimanBounds()
{

}

int run = 1;
void    MlpProblem::findBoundsWithSiman(Data &xl,Data &xr)
{
    Data xl_old = xl;
    Data xr_old = xr;
    SimanBounds b(this,xl,xr,123+run++);

    b.Solve();
    b.getBounds(xl,xr);
    for(int i=0;i<xl.size();i++)
    {
        printf("Bounds[%d]=[%lf=>%lf,%lf=>%lf]\n",i,xl_old[i],xl[i],xr_old[i],xr[i]);
    }
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
            double a = lb[j];
            double b = rb[j];

            sample[j]=a+randomDouble()*(b-a);
        }
        resetViolationPercent(limit);
        double v=funmin(sample);
        //printf("V[%d]=%lf\n",i,v);
        sum = sum+v*(1.0+pow(getViolationPercent()/100.0,2.0));
    }
    sum/=nsamples;
  //  printf("Fitness sum = %.10lg\n",sum);
    return sum;
}

double MlpProblem::getClassErrorPerClass(Data &x, Dataset *t)
{
    int count = t->count();
    Data dclass = t->getPatternClass();
    vector<int> samplesPerClass;
    vector<int> failPerClass;
    samplesPerClass.resize(dclass.size());
    failPerClass.resize(dclass.size());
    for(int i=0;i<(int)dclass.size();i++)
    {
        samplesPerClass[i]=0;
        failPerClass[i]=0;
    }
    for(int i=0;i<count;i++)
    {
        double yy = t->getYPoint(i);
        double xclass = t->getClass(yy);
        double oclass = t->getClass(getOutput(x,t->getXPoint(i).data()));
        bool isError = (fabs(xclass-oclass)>1e-5);
        for(int j=0;j<(int)dclass.size();j++)
        {
            if(fabs(xclass-dclass[j])<1e-5)
            {
                samplesPerClass[j]++;
                if(isError)
                    failPerClass[j]++;
                break;
            }
        }
    }
    double sum=0.0;
    for(int i=0;i<(int)dclass.size();i++)
    {
        double e=failPerClass[i]*100.0/samplesPerClass[i];
        sum=sum+e;
    }
    sum=sum/dclass.size();
    return sum;
}

QJsonObject MlpProblem::done(Data &x)
{
    usebound_flag =false;
    double tr=funmin(x);
    QJsonObject xx;
    double tt=getTestError(x,testDataset);
    double tc=getClassTestError(x,testDataset);
    double ttc=getClassErrorPerClass(x,testDataset);
    xx["trainError"]=tr;
    xx["testError"]=tt;
    xx["classError"]=tc;
    xx["classErrorPerClass"]=ttc;
    double precision=0.0,recall=0.0,f1score=0.0;
    getPrecisionRecall(precision,recall,f1score);
    xx["precision"]=precision;
    xx["recall"]=recall;
    xx["f1score"]=f1score;

    QString output=getParam("mlp_outputfile").getValue();
    if(!output.isEmpty())
    {
        QFile fp(output);
        if(fp.open(QIODevice::WriteOnly))
        {
            QTextStream st(&fp);
            int count = testDataset->count();
            for(int i=0;i<count;i++)
            {
                double realValue = testDataset->getYPoint(i);
                double estValue  = getOutput(x,testDataset->getXPoint(i).data());
                double estClaSS  = testDataset->getClass(estValue);
                st<<realValue<<"  "<<estValue<<" "<<estClaSS<<"\n";
            }
            fp.close();
        }
    }
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
