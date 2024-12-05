#include "griewank2.h"

Griewank2::Griewank2()
    :Problem(2)
{

}

double  Griewank2::funmin(Data &x)
{
    int dimension=x.size();
       double sum=0.0;
       double product=1.0;
       for(int i=0;i<dimension;i++)
       {
         sum+=x[i]*x[i]/200.0;
         product*=cos(x[i]/sqrt(i+1.0));
       }
       return sum+1.0-product;
}

Data    Griewank2::gradient(Data &x)
{
    Data g;
    g.resize(dimension);

        double a,b;
        for(int i=0;i<dimension;i++)
        {
           double product=1.0;
           for(int j=0;j<dimension;j++)
           {
             product*=cos(x[j]/sqrt(j+1.0));
            }
            a=sin(x[i]/sqrt(i+1.0));
            b=sqrt(i+1.0)*cos(x[i]/sqrt(i+1.0));
            g[i]=x[i]/100.0+a*product/b;
        }
    return g;
}
void Griewank2::init(QJsonObject &params) {
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    Data l, r;
    l.resize(n);
    r.resize(n);

    for (int i = 0; i < n; i++) {
        l[i] = -dimension;
        r[i] = dimension;
    }

    setLeftMargin(l);
    setRightMargin(r);
}
