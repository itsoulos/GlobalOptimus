#include "userproblem.h"
# include <stdio.h>
UserProblem::UserProblem()
    :Problem(1)
{
}

double  UserProblem::funmin(Data &x)
{
    printf("This is a simple test function.\n");
    return 0.0;
}

Data UserProblem::gradient(Data &x)
{
    Data g;
    g.resize(x.size());
    return g;
}

void    UserProblem::init(QJsonObject &params)
{

}

QJsonObject UserProblem::done(Data &x)
{
    QJsonObject t;
    return t;
}

UserProblem::~UserProblem()
{

}
