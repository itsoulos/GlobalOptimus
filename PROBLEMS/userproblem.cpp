#include "userproblem.h"
#include <stdio.h>
#include <cmath>


/** ylopoio tin Sphere function **/
//f(x)=sum(i=1)(d)(x^2)
UserProblem::UserProblem()
    : Problem(1)  // Αρχική διάσταση 5
{
}

/** edo grafo to f(x) **/
double UserProblem::funmin(Data &x)
{
    double sum = 0.0;
    /** x.size() == d **/
    for(int i=0;i<x.size();i++)
    {
        sum+=x[i]*x[i];
    }
    return sum;
}

/** auti einai i paragogos **/
Data UserProblem::gradient(Data &x)
{
    /** dino mnimi sto g oso einai i diastasi **/
    Data g;
    g.resize(x.size());
    for(int i=0;i<x.size();i++)
        g[i]=2.0 * x[i];
    return g;
}

void UserProblem::init(QJsonObject &params)
{
    // Ορισμός διάστασης (μπορεί να αλλάξει από UI)
    int d = params["opt_dimension"].toString().toInt();  // Προκαθορισμένη τιμή
    //auto allazei tin diastasi tou problimatos se d
    setDimension(d);

    //Bazo oria stin function
    Data l, r;
    l.resize(d);
    r.resize(d);
    for(int i=0;i<d;i++)
    {
        l[i]=-5.12;
        r[i]= 5.12;
    }
    //auto einai to left margin
    setLeftMargin(l);
    //auto einai to right margin
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
