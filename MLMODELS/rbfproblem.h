#ifndef RBFPROBLEM_H
#define RBFPROBLEM_H

# include <MLMODELS/dataset.h>
# include <MLMODELS/model.h>
# include <OPTIMUS/problem.h>
# include <math.h>
#ifdef OPTIMUS_ARMADILLO
#   include <armadillo>
#   include <adept.h>
#endif

/**
 * @brief The RbfProblem class implements the Radial Basis Function (RBF) model.
 */
class RbfProblem :public Problem, public Model
{
private:
    Data weight;
    vector<Data> centers;
    Data variances;
    /**
     * @brief gaussianDerivative
     * @param x
     * @param m
     * @param v
     * @param pos
     * @return the derivative of the gaussian function at feature pos.
     */
    double      gaussianDerivative(Data &x,Data &m,double v,int pos);
    /**
     * @brief gaussianSecondDerivative
     * @param x
     * @param m
     * @param v
     * @param pos
     * @return the second derivative of the gaussian function at feature pos.
     */
    double      gaussianSecondDerivative(Data &x,Data &m,double v,int pos);
    /**
     * @brief getWeightDerivative Returns the derivative of weights.
     * @param index
     * @param x
     * @param g
     */
    void        getWeightDerivative(int index,Data &x,double &g);
    /**
     * @brief getVarianceDerivative Returns the derivative of variances.
     * @param index
     * @param x
     * @param g
     */
    void        getVarianceDerivative(int index,Data &x,double &g);
    /**
     * @brief getCenterDerivative Returns the derivative of centers.
     * @param index
     * @param x
     * @param g
     */
    void        getCenterDerivative(int index,Data &x,Data &g);
    Matrix trainA;
    Matrix RealOutput;
    Matrix trA;
    Matrix matrixE;
    Matrix matrixC;
    Matrix matrixD;

    bool error_flag=false;
public:
    /**
     * @brief RbfProblem the constructor of the RBF model.
     */
    RbfProblem();
    /**
     * @brief getDerivative
     * @param x
     * @param pos
     * @return the derivative of training error for parameter pos.
     */
    double  getDerivative(Data &x,int pos);
    /**
     * @brief getSecondDerivative
     * @param x
     * @param pos
     * @return the second derivative of training error for parameter pos.
     */
    double  getSecondDerivative(Data &x,int pos);
    /**
     * @brief setParameters Sets the parameters of the model.
     * @param x
     */
    void    setParameters(Data &x);
    /**
     * @brief getParameters Returns the parameters of the model.
     * @param x
     */
    void    getParameters(Data &x);
    /**
     * @brief initModel Initialize the RBF model.
     */
    virtual void initModel();
    /**
     * @brief funmin
     * @param x
     * @return the training error.
     */
    virtual double  funmin(Data &x);
    /**
     * @brief gradient
     * @param x
     * @return the gradient of training error.
     */
    virtual Data    gradient(Data &x);
    /**
     * @brief getOutput
     * @param x
     * @return the output of RBF for pattern x.
     */
    double  getOutput(Data &x);
    double  getOutput(double *x);
    /**
     * @brief runKmeans Implements the K-means algorithm.
     * @param point
     * @param K
     * @param centers
     * @param variances
     */
    void    runKmeans(vector<Data> &point, int K,vector<Data> &centers,
                                Data &variances,vector<int> &t);
    /**
     * @brief gaussian
     * @param x
     * @param center
     * @param variance
     * @return the gaussian value for pattern x.
     */
    double  gaussian(Data &x,Data &center,double variance);
    double  gaussian(double *x,Data &center,double variance);

    /**
     * @brief trainModel Trains the RBF model.
     */
    virtual void trainModel();
    /**
     * @brief init Initialize the RBF model.
     * @param params
     */
    virtual void init(QJsonObject &params);
    /**
     * @brief done
     * @param x
     * @return
     */
    virtual QJsonObject done(Data &x);

    /** Matrix operations **/
    /**
     * @brief matrix_transpose Creates the transpose of a matrix.
     * @param x
     * @param xx
     */
    void    matrix_transpose(Matrix &x,Matrix &xx);
    /**
     * @brief matrix_mult Multiples matrixes.
     * @param x
     * @param y
     * @param z
     */
    void    matrix_mult(Matrix &x,Matrix &y,Matrix &z);
    /**
     * @brief matrix_inverse Creates the inverse of a matrix.
     * @param x
     * @param error_flag
     * @param c
     */
    void    matrix_inverse(Matrix &x,bool &error_flag,Matrix &c);
    /**
     * @brief matrix_pseudoinverse Creates the pseudo inverse of a matrix.
     * @param x
     * @param error_flag
     */
    void    matrix_pseudoinverse(Matrix &x,bool &error_flag);
    /**
     * @brief originalTrain Executes the original train process of RBF using the two phase method.
     */
    void    originalTrain();
    /**
     * @brief apostasi
     * @param x
     * @param y
     * @return the Euclidena distance between x and y.
     */
    double  apostasi(Data &x,Data &y);

    /**
     * @brief getSample
     * @return a new weight vector.
     */
    virtual Data getSample();
#ifdef OPTIMUS_ARMADILLO
    adept::adouble aneuronOutput( vector<adept::adouble> &x, vector<double> &patt, unsigned pattDim, unsigned offset);
    adept::adouble afunmin( vector<adept::adouble> &x, vector<double> &x1 );
#endif
    virtual ~RbfProblem();
};

#endif // RBFPROBLEM_H
