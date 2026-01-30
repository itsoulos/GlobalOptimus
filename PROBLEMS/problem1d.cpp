#include "problem1d.h"

problem1d::problem1d()
{
    int k  = modelSize;
    model = NULL;
    setModelSize(k);
}

void    problem1d::setModelSize(int n)
{
    if(model!=NULL) delete model;
    modelSize=n;
    setDimension(modelSize);
#ifdef USE_MLP
    int patternDimension=2;
    Data l,r;
    l.resize((patternDimension+2)*modelSize);
    r.resize(l.size());
    setDimension(l.size());
    for(int i=0;i<modelSize;i++)
    {
        l[i]=-10.0;
        r[i]= 10.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
    model= new MlpProblem();
    model->setParam("mlp_nodes",QString::number(modelSize));
    model->setPatternDimension(2);
#else
    model = new NNCModel();
    model->setParam("nnc_popsize",QString::number(modelSize));
    model->makeProgram(2);
    Data l,r;
    l.resize(modelSize);
    r.resize(modelSize);
    for(int i=0;i<modelSize;i++)
    {
        l[i]=0.0;
        r[i]=255.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
    setDimension(modelSize);
#endif
}

double problem1d::boundaryInT0(double z)
{
    z0= 0.0;
    z1= Lz;
    double x=1.0;
    double Lc = (z1-z0)/2.0;
    double arg =-pow(z-Lc,2.0)+2.0*M_PI*x*(z-Lc);
    return exp(arg);
}

double problem1d::boundaryInZ0RealPart(double t)
{
    double x = 1.0;
    double D = 1.0/(4.0 * M_PI * x);
    double b = M_PI * x;
    double omega = 2.0*M_PI*x;
    Data xx;
    xx.resize(2);
    xx[0]=t;
    xx[1]=0.0;
    double val1,val2;
#ifdef USE_MLP
    val1=model->getDerivative1(params,xx,0);
    val2=-2.0*D*omega*model->getDerivative1(params,xx,1);
#else
    val1=model->getDerivative(xx,0);
    val2=-2.0*D*omega*model->getDerivative(xx,1);
#endif
    return val1+val2;
}

double problem1d::boundaryInZ0ImagePart(double t)
{
    double x = 1.0;
    double D = 1.0/(4.0 * M_PI * x);
    double b = M_PI * x;
    double omega = 2.0*M_PI*x;
    Data xx;
    xx.resize(2);
    xx[0]=t;
    xx[1]=0.0;
    double m ;
#ifdef USE_MLP
    m=model->getOutput(params,xx.data());
#else
    m=model->getOutput(xx);
#endif
    double val1=2.0 * b * m;
    double val2=gamma * m * m * m;
    return val1+val2;
}

double problem1d::boundaryInZ1RealPart(double t)
{
    double x = 1.0;
    double D = 1.0/(4.0 * M_PI * x);
    double b = M_PI * x;
    double omega = 2.0*M_PI*x;
    Data xx;
    xx.resize(2);
    xx[0]=t;
    xx[1]=Lz;
    double val1,val2;
#ifdef USE_MLP
    val1=model->getDerivative1(params,xx,0);
    val2=2.0*D*omega*model->getDerivative1(params,xx,1);
#else
    val1=model->getDerivative(xx,0);
    val2=2.0*D*omega*model->getDerivative(xx,1);
#endif
    return val1+val2;
}
double problem1d::boundaryInZ1ImagePart(double t)
{
    double x = 1.0;
    double D = 1.0/(4.0 * M_PI * x);
    double b = M_PI * x;
    double omega = 2.0*M_PI*x;
    Data xx;
    xx.resize(2);
    xx[0]=t;
    xx[1]=0.0;
    double m ;
#ifdef USE_MLP
    m=model->getOutput(params,xx.data());
#else
    m=model->getOutput(xx);
#endif
    double val1=2.0 * b * m;
    double val2=gamma * m * m * m;
    return val1+val2;
}

double  problem1d::realPartFunmin()
{
    double sum = 0.0;
    t0 = 0.0;
    t1 = Lt;
    Data xx;
    xx.resize(2);
    for(int i=0;i<=npoints;i++)
    {
        double t = t0 + i*(t1-t0)/(npoints * 1.0);
        xx[0]=t;
        xx[1]=0;
        double val;
#ifdef USE_MLP
        val=model->getDerivative1(params,xx,0);
#else
        val=model->getDerivative(xx,0);
#endif
        sum+=val * val;
    }
    return sum;
}

double problem1d::imagePartFunmin()
{
    double x = 1.0;
    double D = 1.0/(4.0 * M_PI * x);
    double b = M_PI * x;
    double omega = 2.0*M_PI*x;
    Data xx;
    xx.resize(2);
    double sum = 0.0;
    t0=0.0;
    t1=Lt;
    z0=0.0;
    z1=Lz;
    for(int i=0;i<=npoints;i++)
    {
        for(int j=0;j<=npoints;j++)
        {
            xx[0]=t0+i*(t1-t0)/(npoints*1.0);
            xx[1]=z0+j*(z1-z0)/(npoints*1.0);
            double val1,m;
#ifdef USE_MLP
            val1 = D * model->getDerivative2(params,xx,1);
            m = model->getOutput(params,xx.data());
#else
            val1=D * model->getDerivative2(xx,1);
            m = model->getOutput(xx);
#endif

            double val2=b * m;
            double val3=gamma * m * (m*m);
            double f = val1+val2+val3;
            /*
            printf("Values(%lf,%lf) are %lf %lf %lf \n",

                   xx[0],xx[1],
                   val1,val2,val3);*/
            sum+= f*f;
        }
    }
    return sum;
}

double  problem1d::funmin(Data &x)
{
#ifdef USE_MLP
    params = x;
#else
    vector<int> g;
    g.resize(x.size());
    for(int i=0;i<(int)g.size();i++)
    {
        g[i]=(int)x[i];
        if(g[i]<0) g[i]=0;
    }
    int k=model->setChromosomeInParser(g);
    if(!k) return 1e+8;
#endif
    double lambda = 100.0;
    z0=0;
    z1=Lz;
    t0=0;
    t1=Lt;
    double sum1=0.0;
    for(int i=0;i<=npoints;i++)
    {
        Data xx;
        xx.resize(2);

        double z = z0+i*(z1-z0)/(npoints * 1.0);
        xx[0]=0;
        xx[1]=z;
        double p1=boundaryInT0(z);
        double at;
#ifdef USE_MLP
        at=model->getOutput(params,xx.data());
#else
        at=model->getOutput(xx);
#endif
        sum1+=pow(at-p1,2.0);
    }

    double sum2 = 0.0;
    double sum3 = 0.0;
    for(int i=0;i<=npoints;i++)
    {
        double t = t0+i*(t1-t0)/(npoints *1.0);
        double p1=boundaryInZ0RealPart(t);
        double p2=boundaryInZ0ImagePart(t);
        sum2+=p1*p1+p2*p2;
        double p3=boundaryInZ1RealPart(t);
        double p4=boundaryInZ1ImagePart(t);
        sum3+=p3*p3+p4*p4;
    }
    printf("penalties: %lf %lf %lf\n",sum1,sum2,sum3);
    double penalty = lambda * (sum1+sum2+sum3);
    return realPartFunmin()+imagePartFunmin()+penalty;
}

Data    problem1d::gradient(Data &x)
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

void    problem1d::init(QJsonObject &params)
{
    if(params.contains("modelSize"))
    {
        int n = params["modelSize"].toString().toInt();
        setModelSize(n);
    }
    if(params.contains("Lz"))
    {
        Lz = params["Lz"].toString().toDouble();
    }
    if(params.contains("npoints"))
    {
        npoints=params["npoints"].toString().toDouble();
    }
    if(params.contains("beta"))
    {
        beta = params["npoints"].toString().toDouble();
    }
    if(params.contains("gamma"))
    {
        gamma = params["gamma"].toString().toDouble();
    }
    if(params.contains("Lt"))
    {
        Lt = params["Lt"].toString().toDouble();
    }

}

QJsonObject problem1d::done(Data &x)
{
    QJsonObject xx ;
    xx["dummy"]=1;
    return xx;
}

problem1d::~problem1d()
{
    if(model!=NULL)
        delete model;
}
