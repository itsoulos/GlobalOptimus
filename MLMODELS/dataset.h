#ifndef DATASET_H
#define DATASET_H
# include <OPTIMUS/problem.h>
typedef vector<Data>   Matrix;

class Dataset
{
private:
    // xpoint = > ta protypa
    Matrix xpoint;

    //oi monadikes katigories sta protypa
    Data patternClass;
    //sto pararadeigma tou wine sto patternClass mpainoune mesa treis times 0, 0.5, 1.0
    /** **/
    //
    // ypoint = > exodos gia kathe protypo
    Data   ypoint;
    /**
     * @brief makePatternClass, dimiourgei tis monadikes katigories pou yparxoun
     * mesa sto dataset.
     */
    void makePatternClass();
public:
    Dataset(Problem *p,int N);
    /**
     * @brief Dataset, i prokathorismeni synartisi dimioyrgias
     */
    Dataset(); // => default synartisi dimiourgias
    /**
     * @brief Dataset, diabazei ta protypa apo to arxeio filename
     * @param filaname
     */
    Dataset(QString filaname);

    /**
     * @brief setData, orizei ta protypa apo pinaka
     * @param x,  o pinakas ton protypon
     * @param y, oi exodoi gia kathe protypo
     */
    void    setData(Matrix &x,Data &y);
    void    addPoint(Data &x,double y);

    /**
     * @brief saveData Apothikeyei ta protypa kai tis exodous se arxeio.
     * @param filename
     */
    void    saveData(char *filename);

    /**
     * @brief getXPoint, epistrefei to protypo se mia dedomeni thesi
     * @param pos, i thesi toy protypou
     * @return
     */
    Data    getXPoint(int pos) const;

    /**
     * @brief getYPoint, epistrefei tin exodo enos protypou
     * @param pos, i thesi tou protypou
     * @return
     */
    double  getYPoint(int pos) const;

    /**
     * @brief maxx, epistrefei tin megisti timi gia to feature stin thesi pos
     * @param pos, i thesi tou feature
     * @return
     */
    double  maxx(int pos) const;

    /**
     * @brief minx, epistrefei tin elaxisti timi gia to feature stin thesi pos
     * @param pos, i thesi tou feature
     * @return
     */
    double  minx(int pos) const;

    /**
     * @brief miny, Epistrefei tin elaxisti timi ton exodon
     * @return
     */
    double  miny() const;

    /**
     * @brief maxy, Epistrefei tin megisti timi ton exodon
     * @return
     */
    double  maxy() const;

    /**
     * @brief meanx, Epistrefei tin mesi timi tou feature stin thesi pos
     * @param pos
     * @return
     */
    double  meanx(int pos) const;

    /**
     * @brief stdx, epistrefei to standard deviation gia to feature stin
     * thesi pos
     * @param pos
     * @return
     */
    double  stdx(int pos) const;

    /**
     * @brief meany, epistrefei ton meso oro ton exodon gia tis y times.
     * @return
     */
    double  meany() const;

    /**
     * @brief stdy, epistrefei to standard deviation ton exodon gia tis y times.
     * @return
     */
    double  stdy() const;

    /**
     * @brief count, epistrefei to plithos ton protypon sto dataset
     * @return
     */
    int     count() const;

    /**
     * @brief dimension, epistrefei tin diastasi ton protypon (arithmos features)
     * @return
     */
    int     dimension() const;

    /**
     * @brief normalizeMinMax, kanonikopoiei to dataset
     */
    void    normalizeMinMax();

    /**
     * @brief getPatternClass, epistrefei ton pinaka ton monadikon katigorion
     * @return
     */
    Data    getPatternClass();

    /**
     * @brief getClass, epistrefei tin katigoria gia to protypo stin thesi pos
     * @param pos, i thesi tou protypou
     * @return
     */
    double  getClass(int pos) const;

    /**
     * @brief getClass, epistrefei tin katigoria stin opoia prepei na mpei i timi value
     * @param value
     * @return
     */
    double  getClass(double value)const;
    vector<Data> getAllXpoint();
    Data         getAllYpoint();
};

#endif // DATASET_H
