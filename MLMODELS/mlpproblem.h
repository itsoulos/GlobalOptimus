#ifndef MLPPROBLEM_H
#define MLPPROBLEM_H

# include <MLMODELS/dataset.h>
# include <MLMODELS/model.h>
# include <OPTIMUS/problem.h>

/**
 * @brief The MlpProblem class implements the neural network (MLP) model.
 */
class MlpProblem : public Problem, public Model
{
private:
	int violcount,sigcount;
	double viollimit;
    bool usebound_flag=false;
    Data weight;
public:
    /**
     * @brief MlpProblem The main constructor of MLP.
     */
    MlpProblem();
    /**
     * @brief setWeights Sets the weights of MLP.
     * @param w
     */
    void    setWeights(Data &w);
    /**
     * @brief funmin
     * @param x
     * @return  train error for weights x.
     */
    double  funmin(Data &x);
    /**
     * @brief gradient
     * @param x
     * @return the gradient of training error.
     */
    Data    gradient(Data &x);
    /**
     * @brief sig
     * @param x
     * @return the sigmoid function for x.
     */
    double  sig(double x);
    /**
     * @brief sigder
     * @param x
     * @return the derivative of sigmoid function.
     */
    double  sigder(double x);
    /**
     * @brief getOutput
     * @param x
     * @return the output of mlp for pattern x.
     */
    double  getOutput(Data &x);
    double  getOutput(double *x);
    double  getOutput(Data &w,double *x);

    /**
     * @brief getDerivative
     * @param x
     * @return the derivative for pattern x.
     */
    Data    getDerivative(Data &x);
    Data    getDerivative(double *x);
    Data    getDerivative(Data &w,double *x);

    /**
     * @brief getDerivative1
     * @param xpoint
     * @param pos
     * @return the derivative for pattern x estimated for input pos.
     */
    double  getDerivative1(vector<double> xpoint,int pos);
    /**
     * @brief getDerivative2
     * @param xpoint
     * @param pos
     * @return the second derivative for pattern x estimated for input pos.
     */
    double	getDerivative2(vector<double> xpoint,int pos);
    /**
     * @brief init Initialize the MLP model.
     * @param params
     */
    virtual void init(QJsonObject &params);
    /**
     * @brief done
     * @param x
     * @return it is called when MLP finishes with the training.
     */
    virtual QJsonObject done(Data &x);
    /**
     * @brief initWeights Initializes the weights of the MLP.
     */
    void    initWeights();
    /**
     * @brief getSample
     * @return a new weight vector.
     */
    virtual Data getSample();
    /**
     * @brief resetViolationPercent
     * @param limit
     */
    void    resetViolationPercent(double limit);
    /**
     * @brief getViolationPercent
     * @return
     */
    double  getViolationPercent();
    /**
     * @brief getViolationPercentInBounds
     * @param limit
     * @param lb
     * @param rb
     * @return
     */
    double  getViolationPercentInBounds(double limit,Data &lb,Data &rb);
    /**
     * @brief initModel Initializes MLP.
     */
    virtual void initModel();
    /**
     * @brief findBoundsWithSiman
     * @param x0
     * @param xl
     * @param xr
     */
    void    findBoundsWithSiman(Data &x0,Data &xl,Data &xr);
    /**
     * @brief getSampleNoViolate
     * @return
     */
    Data    getSampleNoViolate();
    /**
     * @brief enableBound
     */
    void    enableBound();
    /**
     * @brief disableBound
     */
    void    disableBound();
    virtual ~MlpProblem();
};
#endif // MLPPROBLEM_H
