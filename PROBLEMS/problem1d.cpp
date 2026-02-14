#include "problem1d.h"

problem1d::problem1d()
{
    int k  = modelSize;
    model = NULL;
    setModelSize(k);
}

double problem1d::sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

double problem1d::sigmoid_d(double s) {
    return s * (1.0 - s);
}

double problem1d::sigmoid_dd(double s) {
    return sigmoid_d(s) * (1.0 - 2.0 * s);
}

Eval problem1d::forward(NeuralNet& net, double z, double t)
{
    Eval e;
    e.a1.resize(NeuralNet::H);
    e.a2.resize(NeuralNet::H);
    e.dz1.resize(NeuralNet::H);
    e.dt1.resize(NeuralNet::H);
    e.dz2.resize(NeuralNet::H);
    e.dt2.resize(NeuralNet::H);
    e.dzz2.resize(NeuralNet::H);

    /* Layer 1 */
    for (int i=0;i<NeuralNet::H;i++) {
        double s = net.b1[i] + net.W1[i][0]*z + net.W1[i][1]*t;
        e.a1[i] = sigmoid(s);
        e.dz1[i] = net.W1[i][0];
        e.dt1[i] = net.W1[i][1];
    }

    /* Layer 2 */
    for (int i=0;i<NeuralNet::H;i++) {
        double s = net.b2[i];
        e.dz2[i]=e.dt2[i]=e.dzz2[i]=0.0;

        for (int j=0;j<NeuralNet::H;j++) {
            s += net.W2[i][j]*e.a1[j];
            double da = sigmoid_d(e.a1[j]);
            double dda = sigmoid_dd(e.a1[j]);
            e.dz2[i] += net.W2[i][j]*da*e.dz1[j];
            e.dt2[i] += net.W2[i][j]*da*e.dt1[j];
            e.dzz2[i]+= net.W2[i][j]*dda*e.dz1[j]*e.dz1[j];
        }
        e.a2[i] = sigmoid(s);
    }

    /* Output */
    e.u=e.v=e.ut=e.vt=e.uzz=e.vzz=0.0;
    for (int i=0;i<NeuralNet::H;i++) {
        double da = sigmoid_d(e.a2[i]);
        double dda = sigmoid_dd(e.a2[i]);

        e.u   += net.W3[0][i]*e.a2[i];
        e.v   += net.W3[1][i]*e.a2[i];

        e.ut  += net.W3[0][i]*da*e.dt2[i];
        e.vt  += net.W3[1][i]*da*e.dt2[i];

        e.uzz += net.W3[0][i]*(dda*e.dz2[i]*e.dz2[i] + da*e.dzz2[i]);
        e.vzz += net.W3[1][i]*(dda*e.dz2[i]*e.dz2[i] + da*e.dzz2[i]);
    }
    return e;
}


/* PDE (14.1) */
double problem1d::loss_pde(NeuralNet& net,double z,double t)
{
    auto e = forward(net,z,t);
    double r2 = e.u*e.u + e.v*e.v;

    double R1 = e.ut + Dp*e.vzz + my_beta*e.v + my_gamma*r2*e.v;
    double R2 = e.vt - Dp*e.uzz - my_beta*e.u - my_gamma*r2*e.u;

    return R1*R1 + R2*R2;
}

/* Initial condition (14.2) */
double problem1d::loss_ic(NeuralNet& net,double z)
{
    auto e = forward(net,z,0.0);
    double u0 = exp(-(z-Lz/2)*(z-Lz/2));
    double v0 = 0.0;
    return (e.u-u0)*(e.u-u0) + e.v*e.v;
}

/* Boundary condition (14.3) */
double problem1d::loss_bc(NeuralNet& net,double z,double t,int side)
{
    auto e = forward(net,z,t);
    double s = (side==0 ? -1.0 : 1.0);

    double R1 = e.ut + s*Omega*e.uzz + my_beta*e.v + my_gamma*(e.u*e.u+e.v*e.v)*e.v;
    double R2 = e.vt - s*Omega*e.uzz - my_beta*e.u - my_gamma*(e.u*e.u+e.v*e.v)*e.u;

    return R1*R1 + R2*R2;
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
    net.setParams(x);
    double ic_loss  = 0.0;
    double loss_bc1 = 0.0;
    double loss_bc2 = 0.0;
    double loss = 0.0;
    for(int k=0;k<npoints;k++)
    {
        double z = zpoint[k];
        ic_loss += loss_ic(net,z);
    }
    for(int l=0;l<npoints;l++)
    {
        double t = tpoint[l];
        loss_bc1 += loss_bc(net,0.0,t,0);
        loss_bc2 += loss_bc(net,Lz,t,1);
    }
    for (int k=0;k<npoints;k++) {
        double z = zpoint[k];
        for(int l=0;l<npoints;l++)
        {

            double t = tpoint[l];
            loss += loss_pde(net,z,t);
        }
    }
    loss+=ic_loss;
    loss+=loss_bc1;
    loss+=loss_bc2;
    return loss;

    /*
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
    return realPartFunmin()+imagePartFunmin()+penalty;*/
}

/** finite difference gradient **/
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
    zpoint.resize(npoints);
    tpoint.resize(npoints);
    for(int i=0;i<npoints;i++)
    {
        zpoint[i]=Lz* i*1.0/(npoints-1.0);
        tpoint[i]=T * i*1.0/(npoints-1.0);
    }
    setDimension(net.netSize);
    Data xl,xr;
    xl.resize(net.netSize);
    xr.resize(net.netSize);
    for(int i=0;i<=net.netSize;i++)
    {
        xl[i]=-20.0;
        xr[i]= 20.0;
    }
    setLeftMargin(xl);
    setRightMargin(xr);
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
