 
#include "buche_rastrigin.h"
#include <cmath>

BucheRastrigin::BucheRastrigin()
    : Problem(1)
{
}

double BucheRastrigin::funmin(Data &x)
{
    double sum = 0.0;
    for (int i = 0; i < getDimension(); i++)
    {
        double z = x[i] * x[i] - 10.0 * cos(2.0 * M_PI * x[i]) + 10.0; // Βασικός όρος Rastrigin
        sum += z * (1.0 + 0.1 * sin(10.0 * M_PI * x[i])); // Επιπλέον αναταράξεις
    }
    return sum;
}

Data BucheRastrigin::gradient(Data &x)
{
    Data g;
    g.resize(getDimension());
    for (int i = 0; i < getDimension(); i++)
    {
        double term1 = 2.0 * x[i] + 20.0 * M_PI * sin(2.0 * M_PI * x[i]); // Κλασικό gradient Rastrigin
        double term2 = 0.1 * 10.0 * M_PI * cos(10.0 * M_PI * x[i]); // Παράγωγος του επιπλέον όρου
        g[i] = term1 * (1.0 + 0.1 * sin(10.0 * M_PI * x[i])) + term2 * (x[i] * x[i] - 10.0 * cos(2.0 * M_PI * x[i]) + 10.0);
    }
    return g;
}

void BucheRastrigin::init(QJsonObject &params)
{
    int n = params["opt_dimension"].toString().toInt();
    setDimension(n);
    Data l, r;
    l.resize(n);
    r.resize(n);
    for (int i = 0; i < n; i++)
    {
        l[i] = -5.12;  // Τυπικό εύρος για τη συνάρτηση Rastrigin
        r[i] = 5.12;
    }
    setLeftMargin(l);
    setRightMargin(r);
}
