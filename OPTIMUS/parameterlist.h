#ifndef PARAMETERLIST_H
#define PARAMETERLIST_H
# include <OPTIMUS/parameter.h>
# include <QJsonObject>
# include <QStringList>
class ParameterList
{
protected:
    /**
    * @brief params, these are the method parameters
    */
    QJsonObject       params;
    /**
    * @brief paramsHelp, the help info for every parameter
    */
    QJsonObject       paramsHelp;
    /**
     * @brief paramsType, the type of each parametet
     */
    QJsonObject       paramsType;
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
    ~ParameterList();
};

#endif // PARAMETERLIST_H
