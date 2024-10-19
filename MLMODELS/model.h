#ifndef MODEL_H
#define MODEL_H
# include <MLMODELS/dataset.h>
# include <OPTIMUS/parameterlist.h>
class Model : public ParameterList
{
protected:
    Dataset *trainDataset;
    Dataset *testDataset;
    bool    noRemoveData=false;
    Matrix xall;
public:
    Model();
    void        disableRemoveData();
    void        setTrainSet(Dataset *tr);
    virtual     double  getOutput(Data &x)=0;
    double      getTrainError();
    double      getTestError(Dataset *test) ;
    double      getClassTestError(Dataset *test) ;
    double      getDistance(Data &x1,Data &x2);

    void        loadTrainSet();
    void        loadTestSet();
    double      getTestError();
    double      getClassTestError();
    Dataset     *getTrainDataset();
    virtual ~Model();
};

#endif // MODEL_H
