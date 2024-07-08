#ifndef GRADIENTDESCENT_H
#define GRADIENTDESCENT_H
# include <OPTIMUS/optimizer.h>
# include <LINESEARCH/linesearch.h>
# include <LINESEARCH/fibonaccisearch.h>
# include <LINESEARCH/goldensearch.h>
# include <LINESEARCH/armijosearch.h>

class GradientDescent : public Optimizer
{
public:
    int maxiters;
    int iteration;
    double eps;
    double rate;
    Data xpoint;
    double ypoint;
    bool hasInitialized;
    LineSearch *lt;
    QString lineSearchMethod;//available values: none, armijo, golden, fibonacci
public:
    GradientDescent();
    virtual void init();
    virtual void step();
    virtual bool terminated();
    void    updaterate();
    void    updatepoint();
    virtual void showDebug();
    void    setPoint(Data &x,double &y);
    void    getPoint(Data &x,double &y);
    virtual ~GradientDescent();
};

#endif // GRADIENTDESCENT_H
