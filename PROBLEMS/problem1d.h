#ifndef PROBLEM1D_H
#define PROBLEM1D_H
#include <OPTIMUS/problem.h>
# include <MLMODELS/mlpproblem.h>
# include <MLMODELS/nncmodel.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <random>

using namespace std;


/* =========================
   Neural Network
========================= */
struct NeuralNet {
    static const int H = 16;
    int netSize = 0;
    vector<vector<double>> W1, W2, W3;
    vector<double> b1, b2, b3;

    NeuralNet() {
        int size = 0;
        init(W1, H, 2);
        init(W2, H, H);
        init(W3, 2, H);
        b1.assign(H, 0.0);
        b2.assign(H, 0.0);
        b3.assign(2, 0.0);
        for(int i=0;i<W1.size();i++)
        {
            for(int j=0;j<W1[i].size();j++)
                size++;
        }

        for(int i=0;i<W2.size();i++)
        {
            for(int j=0;j<W2[i].size();j++)
                size++;
        }

        for(int i=0;i<W3.size();i++)
        {
            for(int j=0;j<W3[i].size();j++)
                size++;
        }
        size+=H;
        size+=H;
        size+=2;
        netSize = size;
    }

    void init(vector<vector<double>>& W, int o, int i) {
        random_device rd;
        mt19937 gen(rd());
        normal_distribution<> d(-0.1, 0.1);
        W.assign(o, vector<double>(i));
        for (auto& r : W)
            for (auto& x : r)
                x = d(gen);
    }

    vector<double> getParams()
    {
        vector<double> x;
        x.resize(netSize);
        int icount=0;
        for(unsigned int i=0;i<W1.size();i++)
        {
            for(unsigned int j=0;j<W1[i].size();j++)
                x[icount++]=W1[i][j];
        }

        for(unsigned int i=0;i<W2.size();i++)
        {
            for(unsigned int j=0;j<W2[i].size();j++)
                x[icount++]=W2[i][j];
        }

        for(unsigned int i=0;i<W3.size();i++)
        {
            for(unsigned int j=0;j<W3[i].size();j++)
               x[icount++]=W3[i][j];
        }
        for(unsigned int i=0;i<b1.size();i++)
            x[icount++]=b1[i];
        for(unsigned int i=0;i<b2.size();i++)
            x[icount++]=b2[i];
        for(unsigned int i=0;i<b3.size();i++)
            x[icount++]=b3[i];
    }

    void setParams(vector<double> &x)
    {
        int icount=0;
        for(unsigned int i=0;i<W1.size();i++)
        {
            for(unsigned int j=0;j<W1[i].size();j++)
                W1[i][j]=x[icount++];
        }

        for(unsigned int i=0;i<W2.size();i++)
        {
            for(unsigned int j=0;j<W2[i].size();j++)
                W2[i][j]=x[icount++];
        }

        for(unsigned int i=0;i<W3.size();i++)
        {
            for(unsigned int j=0;j<W3[i].size();j++)
                W3[i][j]=x[icount++];
        }
        for(unsigned int i=0;i<b1.size();i++)
            b1[i]=x[icount++];
        for(unsigned  int i=0;i<b2.size();i++)
            b2[i]=x[icount++];
        for(unsigned int i=0;i<b3.size();i++)
            b3[i]=x[icount++];
    }
};

struct Eval {
    double u,v, ut,vt, uzz,vzz;
    vector<double> a1,a2, dz1,dt1, dz2,dt2,dzz2;
};

# define USE_MLP
class problem1d : public Problem
{
private:
    double Lt=10.0;
    double h = 0.01;
    double beta=3.14;
    double gamma = 1.0; //accepted values: 0,1,-1
    const int npoints = 100;
    double t0,t1;
    double z0,z1;

    const double Dp    = 1.0;
    const double my_beta  = 1.0;
    const double my_gamma = 1.0;
    const double Omega = 1.0;
    const double Lz = 20.0;
    const double T  = 10.0;
    vector<double> zpoint;
    vector<double> tpoint;
    NeuralNet net;
#ifdef USE_MLP
    MlpProblem *model=NULL;
    int modelSize=10;
    Data params;
#else
    NNCModel *model=NULL;
    int    modelSize=100;
#endif
public:
    problem1d();

    double sigmoid(double x);
    double sigmoid_d(double s);
    double sigmoid_dd(double s);
    Eval forward(NeuralNet& net, double z, double t);
    /* PDE (14.1) */
    double loss_pde(NeuralNet& net,double z,double t);
    /* Initial condition (14.2) */
    double loss_ic(NeuralNet& net,double z);
    /* Boundary condition (14.3) */
    double loss_bc(NeuralNet& net,double z,double t,int side);

    void    setModelSize(int n);
    double boundaryInT0(double z);
    double boundaryInZ0RealPart(double t);
    double boundaryInZ0ImagePart(double t);
    double boundaryInZ1RealPart(double t);
    double boundaryInZ1ImagePart(double t);
    double realPartFunmin();
    double imagePartFunmin();
    virtual double funmin(Data &x);
    virtual Data gradient(Data &x);
    virtual void init(QJsonObject &params);
    virtual QJsonObject done(Data &x);
    ~problem1d();
};

#endif // PROBLEM1D_H
