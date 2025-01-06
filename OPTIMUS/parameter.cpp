#include "parameter.h"
# include <QDebug>
Parameter::Parameter(QString n,QString v,QString h)
{
    setName(n);
    setValue(v);
    setHelp(h);
    type=PARAM_STRING;
}

Parameter::Parameter(QString n,int v,int low,int upper,QString h)
{
    setName(n);
    setHelp(h);
    type=PARAM_INTEGER;
    low_int = low;
    upper_int =upper;
    value = QString::number(v);
}

Parameter::Parameter(QString n,double v,double low,double upper,QString h)
{
    setName(n);
    setHelp(h);
    type=PARAM_DOUBLE;
    low_double = low;
    upper_double = upper;
    value = QString::number(v);
}

Parameter::Parameter(QString n,QString h)
{
    setName(n);
    setHelp(h);
    type=PARAM_FILE;
    value = "";
}

Parameter::Parameter(QString n,QString v,QStringList list,QString h)
{
    setName(n);
    setHelp(h);
    value = v;
    string_values = list;
    type=PARAM_LIST;
}

int Parameter::getType() const
{
    return type;
}

int Parameter::getLowIntValue() const
{
    return low_int;
}

int Parameter::getUpperIntValue() const
{
    return upper_int;
}

double  Parameter::getLowDoubleValue() const
{
    return low_double;
}

double  Parameter::getUpperDoubleValue() const
{
    return upper_double;
}
QStringList Parameter::getStringValues() const
{
    return string_values;
}

void    Parameter::setStringValues(QStringList &list)
{
    string_values = list;
}

void    Parameter::setName(QString n)
{
    name = n;
}

QString Parameter::getName() const
{
    return name;
}

void    Parameter::setValue(QString v)
{
    if(type==PARAM_STRING || type==PARAM_FILE)
        value = v;
    else
    if(type == PARAM_INTEGER)
    {
        int vv = v.toInt();
        if(vv < low_int)
            vv = low_int;
        if(vv > upper_int)
            vv= upper_int;
        value =QString::number(vv);
    }
    else
    if(type == PARAM_DOUBLE)
    {
        double vv = v.toDouble();
        if(vv<low_double)
            vv=low_double;
        if(vv>upper_double)
            vv=upper_double;
        value = QString::number(vv);
    }
    else
    if(type == PARAM_LIST)
    {
        if(!string_values.contains(v))
            value = string_values[0];
        else
            value = v;
    }

}

QString Parameter::getValue() const
{
    return value;
}

void    Parameter::setHelp(QString h)
{
    help =h;
}

QString Parameter::getHelp() const
{
    return help;
}

Parameter::Parameter()
{
    setName("");
    setValue("");
    setHelp("");
}

void    Parameter::setType(int t)
{
    type = t;
}

Parameter::~Parameter()
{
    //nothing here
}
