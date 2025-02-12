#include "problem.h"

Problem::Problem(int n)
    : hasKnownOptimum(false),
      besty(std::numeric_limits<double>::infinity()),
      functionCalls(0) {
        setDimension(n);
}

Problem::Problem() : hasKnownOptimum(false), besty(std::numeric_limits<double>::infinity()), functionCalls(0) {
    setDimension(1);
}

void    Problem::resetFunctionCalls()
{
    functionCalls=0;
    besty=1e+100;
}

int Problem::getdimension()
{
    return dimension;
}
int     Problem::getDimension()
{
    return dimension;
}

void    Problem::getleftmargin(double *x)
{
    for(int i=0;i<dimension;i++)
        x[i]=left[i];
}
void    Problem::getrightmargin(double *x)
{
    for(int i=0;i<dimension;i++)
        x[i]=right[i];
}

void    Problem::setDimension(int n)
{
    dimension = n;
    left.resize(dimension);
    right.resize(dimension);
    functionCalls = 0;
    bestx.resize(n);
    bestx = getSample();

}
bool    Problem::isPointIn(Data &x)
{
    for(int i=0;i<(int)x.size();i++)
    {
        if(x[i]<left[i] || x[i]>right[i]) return false;
    }
    return true;
}

double    Problem::statFunmin(Data &x)
{
    double y = funmin(x);

    if(y<besty || functionCalls ==0)
    {

        besty = y;
        bestx = x;

    }
    ++functionCalls;
    return y;
}

Data    Problem::getBestx()
{

    return bestx;
}

double  Problem::getBesty() const
{
    return besty;
}

int     Problem::getFunctionCalls() const
{
    return functionCalls;
}

int Problem::getDimension() const
{
    return dimension;
}

Data Problem::getSample()
{
    Data x;
    x.resize(dimension);
    double r;
    for (int i = 0; i < dimension; i++)
    {
        r = ((double)rand()/(double)RAND_MAX);
        x[i] = left[i] + (right[i] - left[i]) * r;
    }
    return x;
}
void Problem::setLeftMargin(Data &x)
{
    left = x;
}

void Problem::setRightMargin(Data &x)
{
    right = x;
}
Data Problem::getLeftMargin() const
{
    return left;
}

Data Problem::getRightMargin() const
{
    return right;
}

double Problem::grms(Data &x)
{
    Data g = gradient(x);
    int i;
    double s = 0.0;
    for (i = 0; i < (int)x.size(); i++)
        s = s + g[i] * g[i];
    return sqrt(s / x.size());
}

void    Problem::init(QJsonObject &params)
{
    //nothing here
}

double  Problem::randomDouble()
{
    return rand()*1.0/RAND_MAX;
}

void    Problem::randomize(int seed)
{
    srand(seed);
}

bool     Problem::getGradientCriterion(Data &x1,Data &x2)
{
    Data g1,g2;
    g1.resize(x1.size());
    g2.resize(x2.size());
    g1=gradient(x1);
    g2=gradient(x2);
    double s=0.0;
    for(int i=0;i<x1.size();i++)
        s+=(x1[i]-x2[i])*(g1[i]-g2[i]);
    return s>=0;
}

void    Problem::granal(Data &x,Data &g)
{

}
QJsonObject Problem::done(Data &x)
{
    return QJsonObject();
}

void Problem::setKnownOptimum(double value, const Data& location) {
    knownOptimumValue = value;
    knownOptimumLocation = location;
    hasKnownOptimum = true;
}

bool Problem::hasOptimum() const {
    return hasKnownOptimum;
}

double Problem::getKnownOptimumValue() const {
    return knownOptimumValue;
}

Data Problem::getKnownOptimumLocation() const {
    return knownOptimumLocation;
}
Problem::~Problem()
{
}
