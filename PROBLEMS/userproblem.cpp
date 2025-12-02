#include "userproblem.h"
#include <stdio.h>
#include <cmath>

UserProblem::UserProblem()
    : Problem(5)  // Αρχική διάσταση 5
{
}

double UserProblem::funmin(Data &x)
{
    // Trid Function: f(x) = Σ(x_i - 1)^2 - Σ(x_i * x_{i-1})
    double sum1 = 0.0;
    double sum2 = 0.0;

    int d = getDimension();

    // Πρώτο άθροισμα: Σ(x_i - 1)^2
    for(int i = 0; i < d; i++)
    {
        double diff = x[i] - 1.0;
        sum1 += diff * diff;
    }

    // Δεύτερο άθροισμα: Σ(x_i * x_{i-1}) για i=1..d-1
    for(int i = 1; i < d; i++)
    {
        sum2 += x[i] * x[i-1];
    }

    return sum1 - sum2;
}

Data UserProblem::gradient(Data &x)
{
    // Αναλυτική κλίση της Trid function
    // δf/∂x_i = 2(x_i - 1) - x_{i-1} - x_{i+1}
    Data g;
    g.resize(x.size());
    int d = getDimension();

    for(int i = 0; i < d; i++)
    {
        // Βασικός όρος: 2(x_i - 1)
        g[i] = 2.0 * (x[i] - 1.0);
        if(i>0)
            g[i]-=x[i-1];
       
        if(i > 0)
        {
            g[i] -= x[i-1];
        }

       
        if(i < d - 1)
        {
            g[i] -= x[i+1];
        }
    }
    return g;
}

void UserProblem::init(QJsonObject &params)
{
    // Ορισμός διάστασης (μπορεί να αλλάξει από UI)
    int d = params["opt_dimension"].toString().toInt();  // Προκαθορισμένη τιμή
    setDimension(d);

    Data l, r;
    l.resize(d);
    r.resize(d);

    // Ορισμός ορίων: [-d^2, d^2] για κάθε διάσταση
    for(int i = 0; i < d; i++)
    {
        l[i] = -d * d;   // Left margin = -d^2
        r[i] = d * d;    // Right margin = d^2
    }

    setLeftMargin(l);
    setRightMargin(r);
}

QJsonObject UserProblem::done(Data &x)
{
    QJsonObject t;
    return t;
}

UserProblem::~UserProblem()
{
}
