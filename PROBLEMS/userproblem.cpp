#include "userproblem.h"
#include <stdio.h>
#include <cmath>

UserProblem::UserProblem()
    : Problem(1)  // Αρχική διάσταση 5
{
}

double UserProblem::funmin(Data &x)
{
}

Data UserProblem::gradient(Data &x)
{
    Data g;
    g.resize(x.size());
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
