#include "mlpproblem.h"
# include <QDebug>
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
          double a = -10;
          double b = 10;
          for(int i=0;i<dimension;i++)
          {
              xx[i]=a+(b-a)*randomDouble();
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

void    MlpProblem::init(QJsonObject &pt)
{
    QString trainName = pt["model_trainfile"].toString();
    QString testName =  pt["model_testfile"].toString();
    setParam("model_trainfile",trainName);
    setParam("model_testfile",testName);
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
    double arg=0.0;
    double per=0.0;
    int nodes = weight.size()/(x.size()+2);
    int d = x.size();
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

QJsonObject MlpProblem::done(Data &x)
{
    double tr=funmin(x);
    QJsonObject xx;
    double tt=getTestError(testDataset);
    double tc=getClassTestError(testDataset);
    xx["trainError"]=tr;
    xx["testError"]=tt;
    xx["classError"]=tc;
    return xx;

}
MlpProblem::~MlpProblem()
{
}
