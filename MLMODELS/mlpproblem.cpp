#include "mlpproblem.h"
#include "mlpproblem.h"

  MlpProblem::MlpProblem()
    :Problem(1)
{

}
void    MlpProblem::init(QJsonObject &params)
{
    QString trainName = params["mlp_trainfile"].toString();
    QString testName =  params["mlp_testfile"].toString();
    int nodes        = 1;
    if(params.contains("mlp_nodes"))
        nodes = params["mlp_nodes"].toString().toInt();
    double leftMargin = -10.0;
    double rightMargin = 10.0;
    if(params.contains("mlp_leftmargin"))
        leftMargin=params["mlp_leftmargin"].toString().toDouble();
    if(params.contains("mlp_rightmargin"))
       rightMargin = params["mlp_rightmargin"].toString().toDouble();
    trainDataset=new Dataset(trainName);
    testDataset = new Dataset(testName);
    int k = (trainDataset->dimension()+2)*nodes;
    setDimension(k);
    left.resize(k);
    right.resize(k);
    /** edo bazo ta oria ton
     *  parametron tou neuronikou diktyou **/
    for(int i=0;i<k;i++)
    {
        left[i]=leftMargin;
        right[i]=rightMargin;
    }
    weight.resize(k);
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
    int nodes = weight.size()/(trainDataset->dimension()+2);
    int d = trainDataset->dimension();
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
