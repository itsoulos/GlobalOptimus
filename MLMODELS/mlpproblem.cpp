#include "mlpproblem.h"

MlpProblem::MlpProblem()
    :Problem(1)
{
      addParam(Parameter("mlp_nodes","1","Number of weights"));
      addParam(Parameter("mlp_leftmargin","-10.0","Initial left margin"));
      addParam(Parameter("mlp_rightmargin","10.0","Initial right margin"));
      addParam(Parameter("mlp_initmethod","smallvalues","Possible values: smallvalues,xavier"));
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
          double a = -0.01;
          double b = 0.01;
          if(a<leftMargin) a= leftMargin;
          if(b>rightMargin) b= rightMargin;
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
    QString trainName = pt["mlp_trainfile"].toString();
    QString testName =  pt["mlp_testfile"].toString();
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
    trainDataset=new Dataset(trainName);
    testDataset = new Dataset(testName);
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
    return getTrainError();

}

/** edo epistrefoume tin paragogo tis
 *  synartisis funmin(x) os pros x**/
Data    MlpProblem::gradient(Data &x)
{
    Data g;
    weight = x;
    g.resize(weight.size());
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
        for(int j=1;j<=d;j++)
        {
            int pos=(d+2)*i-(d+1)+j-1;
            arg+=weight[pos]*x[j-1];
        }
        arg+=weight[(d+2)*i-1];
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
