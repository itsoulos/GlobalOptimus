#include "gkls250.h"
# include "gkls.h"
Gkls250::Gkls250()
    :Problem(2)
{
    Data l, r;
    l.resize(2);
    r.resize(2);
    for (int i = 0; i < 2; i++)
    {
        l[i] = -1.0;
        r[i] =  1.0;
    }
    setLeftMargin(l);
    setRightMargin(r);

    GKLS_dim = 2;
    GKLS_num_minima = 50;
    GKLS_domain_alloc();
    GKLS_global_dist=2.0/3.0;
    GKLS_global_radius=0.5*GKLS_global_dist;
    GKLS_global_value=GKLS_GLOBAL_MIN_VALUE;
        GKLS_parameters_check();
    int func_num=2;
        GKLS_arg_generate(func_num);
}

double Gkls250::funmin(Data &x)
{
    double *tempx=new double[x.size()];
    for(int i=0;i<x.size();i++) tempx[i]=x[i];
    double f=GKLS_D_func(tempx);
    for(int i=0;i<x.size();i++) x[i]=tempx[i];
    delete[] tempx;
    return f;
}

Data Gkls250::gradient(Data &x)
{
    Data g;
    g.resize(dimension);
    double *tempx=new double[x.size()];
    double *tempg=new double[x.size()];
    for(int i=0;i<x.size();i++) tempx[i]=x[i];
    GKLS_D_gradient(tempx,tempg);
    for(int i=0;i<x.size();i++) x[i]=tempx[i];
    for(int i=0;i<x.size();i++) g[i]=tempg[i];
    delete[] tempx;
    delete[] tempg;
    return g;
}
