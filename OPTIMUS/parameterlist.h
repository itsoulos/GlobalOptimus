#ifndef PARAMETERLIST_H
#define PARAMETERLIST_H
# include <OPTIMUS/parameter.h>
# include <QJsonObject>
# include <QStringList>
class ParameterList
{
protected:
    QVector<Parameter> mparams;
public:
    /**
     * @brief ParameterList, the default constructor
     */
    ParameterList();
    /**
     * @brief addParam, adds the parameter p to the list
     * @param p, the new parameter.
     */
    void        addParam(Parameter p);
    /**
     * @brief setParam, changes the values of a parameter
     * @param name, the name of the parameter
     * @param value, the new value
     * @param help, the new help string
     */
    void        setParam(QString name,QString value,QString help="");
    /**
     * @brief getParam, return the parameter located in value name
     * @param name, the name of the required parameter
     * @return  the parameter located in value name
     */
    Parameter   getParam(QString name);
    Parameter   getParam(int index);
    void        deleteParam(int index);
    /**
     * @brief getParameterNames
     * @return  the list of all parameter names
     */
    QStringList getParameterNames() const;
    /**
     * @brief getParams
     * @return  in json format the params
     */
    QJsonObject getParams() ;
    /**
     * @brief getParamVector
     * @return  the list of parameters in vector format.
     */
    QVector<Parameter> getParamVector();

    /**
     * @brief contains
     * @param name
     * @return  true if the parameter list contains a param
     *     with name.
     */
    bool contains(QString name)const;
    /**
     * @brief setParamValuesFromJson alters all parameter values from the json
     * @param x, the json object contains the new values
     */
    void   setParamValuesFromJson(QJsonObject &x);
    int     countParameters() const;
    ~ParameterList();
};

#endif // PARAMETERLIST_H
