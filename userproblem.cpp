#include "userproblem.h"
#include <cmath>

static inline double absd(double x) { return (x < 0.0) ? -x : x; }

UserProblem::UserProblem(int n) : Problem(n)
{
    Data l(n, -10.0);
    Data r(n,  10.0);
    setLeftMargin(l);
    setRightMargin(r);
}

double UserProblem::funmin(Data &x)
{
    double s = 0.0;
    for (int i = 0; i < (int)x.size(); ++i)
    {
        double xi = x[i];
        double gi = xi * std::sin(xi) + 0.1 * xi;
        s += absd(gi);
    }
    return s;
}

Data UserProblem::gradient(Data &x)
{
    Data g;
    g.resize(x.size());
    const double eps = 1e-12;

    for (int i = 0; i < (int)x.size(); ++i)
    {
        double xi = x[i];
        double gi = xi * std::sin(xi) + 0.1 * xi;

        if (absd(gi) < eps) { g[i] = 0.0; continue; }

        double sign = (gi > 0.0) ? 1.0 : -1.0;
        double dgi  = std::sin(xi) + xi * std::cos(xi) + 0.1;
        g[i] = sign * dgi;
    }
    return g;
}

UserProblem::~UserProblem() {}

