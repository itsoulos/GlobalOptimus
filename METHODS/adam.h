#ifndef ADAM_H
#define ADAM_H

# include <OPTIMUS/optimizer.h>
# include <LINESEARCH/goldensearch.h>
# include <LINESEARCH/fibonaccisearch.h>
# include <LINESEARCH/armijosearch.h>
# include <math.h>
class Adam : public Optimizer
{
private:
    Data xpoint;
    double ypoint;
    bool haveInitialized;
    double learningRate;
    int     iterations;
    Data M_0,U_0,rM,lM;
    double b1,b2;
    QString linesearch;
    LineSearch *lt;
public:
    Adam();
    virtual void init();
    virtual void step();
    virtual bool terminated();
    void    updateRate();
    void setPoint(Data &x,double &y);
    void getPoint(Data &x,double &y);
    virtual ~Adam();
};

#endif // ADAM_H
