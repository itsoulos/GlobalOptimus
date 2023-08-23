#include "parameter.h"
Parameter::Parameter(QString n,QString v,QString h)
{
    setName(n);
    setValue(v);
    setHelp(h);
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
    value = v;
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

Parameter::~Parameter()
{
    //nothing here
}
