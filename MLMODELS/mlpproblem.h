#ifndef MLPPROBLEM_H
#define MLPPROBLEM_H

# include <MLMODELS/dataset.h>
# include <MLMODELS/model.h>
# include <OPTIMUS/problem.h>

class MlpProblem : public Problem, public Model
{
private:
    /** weight=> einai oi parametroi tou neuronikou, to dianysma w **/
    Data weight;
public:
    MlpProblem();
    /**
     * @brief setWeights => allazei ton pinaka weight se w.
     * @param w
     */
    void    setWeights(Data &w);
    /**
     * @brief funmin => einai to sfalma ekpaideysis gia ta weight = x
     * @param x
     * @return
     */
    double  funmin(Data &x);
    /**
     * @brief gradient => einai i paragogos tis synartisis sfalmatos
     * os pros tis parametrous x.
     * @param x
     * @return
     */
    Data    gradient(Data &x);
    /**
     * @brief sig, einai i sigmoidis synartisi gia eisodo x.
     * @param x
     * @return
     */
    double  sig(double x);
    /**
     * @brief sigder, einai i paragogos tis sigmoeidous gia eisodo x.
     * @param x
     * @return
     */
    double  sigder(double x);
    /**
     * @brief getOutput=> einai i timi tou neuronikou gia to protypo x
     * gia weight = x.
     * @param x
     * @return
     */
    double  getOutput(Data &x);
    /**
     * @brief getDerivative=> einai i paragogos gia to protypo x.
     * @param x
     * @return
     */
    Data    getDerivative(Data &x);
    double  getDerivative1(vector<double> xpoint,int pos);
    double	getDerivative2(vector<double> xpoint,int pos);
    virtual void init(QJsonObject &params);
    virtual QJsonObject done(Data &x);
    void    initWeights();
    virtual Data getSample();
    virtual ~MlpProblem();
};
#endif // MLPPROBLEM_H
