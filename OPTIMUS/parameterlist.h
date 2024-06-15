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
public:
    ParameterList();
    void        addParam(Parameter p);
    void        setParam(QString name,QString value,QString help="");
    Parameter   getParam(QString name);
    QStringList getParameterNames() const;
    QJsonObject getParams() ;
    ~ParameterList();
};

#endif // PARAMETERLIST_H
