#ifndef MODEL_H
#define MODEL_H
# include <AIMODELS/dataset.h>

class Model
{
protected:
    Dataset *trainDataset;
    Dataset *testDataset;
public:
    Model();
    virtual     double  getOutput(Data &x)=0;
    double      getTrainError();
    double      getTestError(Dataset *test) ;
    double      getClassTestError(Dataset *test) ;
    ~Model();
};

#endif // MODEL_H
