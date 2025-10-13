#ifndef MODEL_H
#define MODEL_H
# include <MLMODELS/dataset.h>
# include <OPTIMUS/parameterlist.h>
# include <OPTIMUS/optimizer.h>
#include <random>
using namespace std;

/**
 * @brief The Model class implements an abstract class for Machine Learning models.
 */
class Model : public ParameterList
{
protected:
    /**
     * @brief trainDataset, provides the train dataset
     */
    Dataset *trainDataset;
    /**
     * @brief testDataset, provides the test dataset
     */
    Dataset *testDataset;
    bool    noRemoveData=false;
    /**
     * @brief xall, all the training data
     */
    Matrix xall;
    /**
     * @brief method, the used optimization method.
     */
    Optimizer *method;
    /**
     * @brief modelSeed, the seed number used in the model initialization.
     */
    int     modelSeed;

    mt19937 generator;
    uniform_int_distribution<> intDistrib;
    uniform_real_distribution<> doubleDistrib;
public:
    /**
     * @brief Model, the default constructor
     */
    Model();
    void        disableRemoveData();
    /**
     * @brief setTrainSet, alters the training dataset
     * @param tr
     */
    void        setTrainSet(Dataset *tr);
    /**
     * @brief setTestSet, alters the testing dataset
     * @param tt
     */
    void        setTestSet(Dataset *tt);
    /**
     * @brief getOutput
     * @param x, the input pattern
     * @return the output of the model for any given pattern
     */
    virtual     double  getOutput(Data &x)=0;

    virtual     double getOutput(double *x);
    /**
     * @brief getTrainError
     * @return the training error of the model
     */
    double      getTrainError();
    /**
     * @brief getTestError
     * @param test
     * @return  the test error for a provided dataset
     */
    double      getTestError(Dataset *test) ;
    /**
     * @brief getClassTestError
     * @param test
     * @return  the class test error for a provided dataset
     */
    double      getClassTestError(Dataset *test) ;
    /**
     * @brief getDistance
     * @param x1
     * @param x2
     * @return  the euclidean distance between vectors x1 and x2.
     */
    double      getDistance(Data &x1,Data &x2);
    /**
     * @brief initModel, initialize the parameters of the model.
     */
    virtual     void initModel();
    /**
     * @brief trainModel, trains the model to estimate the best set of parameters.
     */
    virtual     void trainModel();
    /**
     * @brief setOptimizer, changes the used optimization method.
     * @param p
     */
    void        setOptimizer(Optimizer *p);
    /**
     * @brief loadTrainSet, loads the training set from a disk file.
     */
    void        loadTrainSet();
    /**
     * @brief loadTestSet, loads the test set from a disk file.
     */
    void        loadTestSet();
    /**
     * @brief getTestError
     * @return  the test error of the model
     */
    double      getTestError();
    /**
     * @brief getClassTestError
     * @return the class test error of the model
     */
    double      getClassTestError();
    /**
     * @brief getTrainDataset
     * @return  the used train dataset
     */
    Dataset     *getTrainDataset();
    /**
     * @brief testModel, perform test for the model
     * @param trainError
     * @param testError
     * @param classError
     */
    virtual     void  testModel(double &trainError,double &testError,double &classError);
    /**
     * @brief setModelSeed, alters the used seed for the random number generator
     * @param seed
     */
    void    setModelSeed(int seed);
    /**
     * @brief getModelSeed
     * @return  the used seed for the random number generator
     */
    int     getModelSeed() const;
    /**
     * @brief ~Model the used destructor
     */

    void	printConfusionMatrix(vector<double> &T,
                                 vector<double> &O,
                                 vector<double> &precision,
                                 vector<double> &recall);
    void    getPrecisionRecall(double &precision,double &recall,double &fscore);
    ParameterList getParameterList();
    virtual ~Model();
};

#endif // MODEL_H
