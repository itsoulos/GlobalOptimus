#include "ellipsoidal.h"
#include <cmath>

Ellipsoidal::Ellipsoidal()
    : Problem(1)
{
}

double Ellipsoidal::funmin(Data &x)
{
    double sum = 0.0;
    for (int i = 0; i < getDimension(); i++)
    {
        // Κλιμάκωση με τετραγωνικούς όρους που αυξάνονται με το i
        sum += pow(1000.0, i / (getDimension() - 1.0)) * x[i] * x[i];
    }
    return sum;
}

Data Ellipsoidal::gradient(Data &x)
{
    Data g;
    g.resize(getDimension());
    for (int i = 0; i < getDimension(); i++)
    {
        // Παράγωγος ως προς x_i: 2 * (1000^(i/(n-1))) * x_i
        g[i] = 2.0 * pow(1000.0, i / (getDimension() - 1.0)) * x[i];
    }
    return g;
}

void Ellipsoidal::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    Data l, r;
    l.resize(n);
    r.resize(n);
    for (int i = 0; i < n; i++)
    {
        l[i] = -5.12;  // Τυπικό εύρος για παρόμοιες συναρτήσεις
        r[i] = 5.12;
    }
    setLeftMargin(l);
    setRightMargin(r);
} 
