 
#include "linear_slope.h"
#include <cmath>

LinearSlope::LinearSlope()
    : Problem(1)
{
}

double LinearSlope::funmin(Data &x)
{
    double sum = 0.0;
    for (int i = 0; i < getDimension(); i++)
    {
        sum += (i + 1) * x[i];  // Γραμμικός όρος με διαφορετική κλίση για κάθε διάσταση
    }
    return sum;
}

Data LinearSlope::gradient(Data &x)
{
    Data g;
    g.resize(getDimension());
    for (int i = 0; i < getDimension(); i++)
    {
        g[i] = (i + 1);  // Η παράγωγος του γραμμικού όρου είναι ο συντελεστής (i + 1)
    }
    return g;
}

void LinearSlope::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    Data l, r;
    l.resize(n);
    r.resize(n);
    for (int i = 0; i < n; i++)
    {
        l[i] = -5.0;  // Τυπικό εύρος για τη συνάρτηση Linear Slope
        r[i] = 5.0;
    }
    setLeftMargin(l);
    setRightMargin(r);
}
