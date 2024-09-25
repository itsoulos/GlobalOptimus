#include "shubert.h"

shubert::shubert()
    : Problem(3)
{
    Data l, r;
    l.resize(dimension);
    r.resize(dimension);
    for (int i = 0; i < dimension; i++)
    {
        l[i] = -10.0;
        r[i] =  10.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}

double shubert::funmin( Data &x){
    double x1=x[0];
        double x2=x[1];
    double sum1 = 0, sum2 = 0;

    for (int j = 1; j <= 5; j++) {
        sum1 += j * cos((j + 1) * x1 + j);
        sum2 += j * cos((j + 1) * x2 + j);
    }

    return sum1 * sum2;
}






Data shubert::gradient(Data &x){
    Data g;
    g.resize(2);
double x1 = x[0];
double x2 = x[1];
double sum1 = 0.0;
double sum2 = 0.0;
double t1 = 0.0;
double t2 = 0.0;


for (int j = 1; j <= 5; j++) {
    sum1 += j * cos((j + 1) * x1 + j);
    sum2 += j * cos((j + 1) * x2 + j);
}

for (int j = 1; j <= 5; j++) {
    t1 += -j * (j + 1) * sin((j + 1) * x1 + j);
}
g[0] = t1 * sum2;

for (int j = 1; j <= 5; j++) {
    t2 += -j * (j + 1) * sin((j + 1) * x2 + j);
}
g[1] = sum1 * t2;
return g;
}


