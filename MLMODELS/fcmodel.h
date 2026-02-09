#ifndef FCMODEL_H
#define FCMODEL_H
# include <MLMODELS/model.h>
# include <GE/cprogram.h>
# include <GE/population.h>
# include <OPTIMUS/parameterlist.h>
# include <OPTIMUS/methodloader.h>
# include <GE/fcprogram.h>
class FcModel :public Model
{
private:
    Model           *createModel,*evaluateModel;
    MethodLoader    *createLoader,*evaluateLoader;
    Optimizer       *createMethod,*evaluateMethod;
    Population      *pop;
    FcProgram       *program;
    double   avgPrec,avgRec,avgF1;
    bool isTestRunning=false;
public:
    FcModel();
    virtual     void        trainModel();
    virtual     double  getOutput(Data &x);
    virtual     void  testModel(double &trainError,double &testError,
                           double &classError,double &classErrorPerClass);
    virtual void    getPrecisionRecall(double &precision,
                        double &recall,double &fscore);

    virtual ~FcModel();
};

#endif // FCMODEL_H
