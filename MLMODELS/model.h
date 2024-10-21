#ifndef MODEL_H
#define MODEL_H
# include <MLMODELS/dataset.h>
# include <OPTIMUS/parameterlist.h>
# include <OPTIMUS/optimizer.h>
class Model : public ParameterList
{
protected:
    Dataset *trainDataset;
    Dataset *testDataset;
    bool    noRemoveData=false;
    Matrix xall;
    Optimizer *method;
public:
    Model();
    void        disableRemoveData();
    void        setTrainSet(Dataset *tr);
    void        setTestSet(Dataset *tt);
    virtual     double  getOutput(Data &x)=0;
    double      getTrainError();
    double      getTestError(Dataset *test) ;
    double      getClassTestError(Dataset *test) ;
    double      getDistance(Data &x1,Data &x2);
    virtual     void initModel();
    virtual     void trainModel();
    void        setOptimizer(Optimizer *p);
    void        loadTrainSet();
    void        loadTestSet();
    double      getTestError();
    double      getClassTestError();
    Dataset     *getTrainDataset();
    virtual     void  testModel(double &trainError,double &testError,double &classError);
    virtual ~Model();
};

#endif // MODEL_H
