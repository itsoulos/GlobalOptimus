#ifndef PARAMETER_H
#define PARAMETER_H
# include <QString>
# include <QStringList>
/**
 *  Parameter types
 */
# define PARAM_STRING   1
# define PARAM_INTEGER  2
# define PARAM_DOUBLE   3
# define PARAM_LIST     4
# define PARAM_FILE     5

/**
 * @brief The Parameter class implements a command line parameter
 * using name, value and help.
 */
class Parameter
{
private:
    QString name;
    QString value;
    QString help;
    int     type=PARAM_STRING;
    int     low_int;
    int     upper_int;
    double  low_double;
    double  upper_double;
    QStringList string_values;
public:
    /**
     * @brief Parameter
     * @param n
     * @param v
     * @param h
     */
    Parameter(QString n,QString v,QString h);
    /**
     * @brief Parameter
     * @param n
     * @param v
     * @param low
     * @param upper
     * @param h
     */
    Parameter(QString n,int v,int low,int upper,QString h);
    /**
     * @brief Parameter
     * @param n
     * @param v
     * @param low
     * @param upper
     * @param h
     */
    Parameter(QString n,double v,double low,double upper,QString h);
    /**
     * @brief Parameter
     * @param n
     * @param h
     */
    Parameter(QString n,QString h);
    /**
     * @brief Parameter
     * @param n
     * @param v
     * @param list
     * @param h
     */
    Parameter(QString n,QString v,QStringList list,QString h);
    /**
     * @brief setName
     * @param n
     */
    void    setName(QString n);
    /**
     * @brief getName
     * @return
     */
    QString getName() const;
    /**
     * @brief setValue
     * @param v
     */
    void    setValue(QString v);
    /**
     * @brief getValue
     * @return
     */
    QString getValue() const;
    /**
     * @brief setHelp
     * @param h
     */
    void    setHelp(QString h);
    /**
     * @brief getHelp
     * @return
     */
    QString getHelp() const;
    /**
     * @brief Parameter
     */

    /**
     * @brief getType
     * @return
     */
    int getType() const;


    /**
     * @brief setType alters the type of a parameter.
     * @param t
     */
    void    setType(int t);
    /**
     * @brief getLowIntValue
     * @return
     */
    int getLowIntValue() const;
    /**
     * @brief getUpperIntValue
     * @return
     */
    int getUpperIntValue() const;
    /**
     * @brief setLowIntValue
     * @param v
     */
    void setLowIntValue(int v);
    /**
     * @brief setUpperIntValue
     * @param v
     */
    void setUpperIntValue(int v);
    /**
     * @brief getUpperDoubleValue
     * @return
     */
    double getUpperDoubleValue() const;
    /**
     * @brief getLowDoubleValue
     * @return
     */
    double getLowDoubleValue()   const;
    /**
     * @brief setUpperDoubleValue
     * @param v
     */
    void  setUpperDoubleValue(double v);
    /**
     * @brief setLowDoubleValue
     * @param v
     */
    void  setLowDoubleValue(double v);
    /**
     * @brief setStringValues changes the string array of values
     * @param list the new list.
     */
    void setStringValues(QStringList &list);
    /**
     * @brief getStringValues
     * @return the string array of values
     */
    QStringList getStringValues() const;
    /**
     * @brief Parameter, the default constructor of the class.
     */
    Parameter();
    /**
     */
    ~Parameter();
};

#endif // PARAMETER_H
