#ifndef PARAMETER_H
#define PARAMETER_H
# include <QString>

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
public:
    Parameter(QString n,QString v,QString h);
    void    setName(QString n);
    QString getName() const;
    void    setValue(QString v);
    QString getValue() const;
    void    setHelp(QString h);
    QString getHelp() const;
    Parameter();
    ~Parameter();
};

#endif // PARAMETER_H
