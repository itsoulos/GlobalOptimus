#include "potential.h"

Potential::Potential()
    :Problem(1)
{

}
double  Potential::funmin(Data &x)
{
    int natoms = dimension/3;
    double value=0.0;
    for(int i=1;i<=natoms;i++)
    {
        int i31 = 3*i;
        for(int j=i+1;j<=natoms;j++)
        {
            int i32 = 3*j;
            double rij = sqrt( (x[i31-2-1] - x[i32-2-1])*(x[i31-2-1] - x[i32-2-1])+
                              (x[i31-1-1] - x[i32-1-1])*(x[i31-1-1] - x[i32-1-1])+
                              (x[i31-1] - x[i32-1])*(x[i31-1] - x[i32-1]) );
            value=value+dpot(rij);
        }
    }
    return  value;
}

Data    Potential::gradient(Data &x)
{
    int natoms = dimension/3;
    Data g;
    g.resize(dimension);
    for(int i=1;i<=natoms;i++)
    {
        int i31=3*i;
        int idim=3 * i;
        g[idim-1]=0.0;
        g[idim-1-1]=0.0;
        g[idim-2-1]=0.0;
        for(int j=1;j<=natoms;j++)
        {
            int i32=3*j;
            if(j!=i)
            {
                double rij = sqrt( (x[i31-2-1] - x[i32-2-1])*(x[i31-2-1] - x[i32-2-1])+
                                  (x[i31-1-1] - x[i32-1-1])*(x[i31-1-1] - x[i32-1-1])+
                                  (x[i31-1] - x[i32-1])*(x[i31-1] - x[i32-1]) );

                double gp = gpot(rij);
                g[idim-2-1]+=gp* (x[i31-2-1] - x[i32-2-1])/rij;
                g[idim-1-1]+=gp*(x[i31-1-1] - x[i32-1-1])/rij;
                g[idim-1]+=gp*(x[i31-1] - x[i32-1])/rij;
            }
        }
    }
    return g;
}

double  Potential::gpot(double r)
{
    double eps = 1.0, sig = 1.0;
    double eps4 = 4*eps;
    double sl = 0.0;

    double sbyr7 = pow((sig/r),7);
    double sbyr6 = pow((sig/r),6);
    return eps4*sbyr6*(-12.0*sbyr7 + 6.0*(sig/r))+sl/(r*r);
}

double  Potential::dpot(double r)
{
    double  eps=1.0;
    double  sig =1.0;
    double  eps4=4.0 *eps;
    double sbyr6=pow(sig/r,6.0);
    return eps4 * sbyr6*(sbyr6-1.0);
}

void    Potential::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(3*n);
    Data l, r;
    l.resize(dimension);
    r.resize(dimension);
    for (int i = 0; i < dimension; i++)
    {
        l[i] = -2.0;
        r[i] =  2.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}
