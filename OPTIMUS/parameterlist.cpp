#include "parameterlist.h"

ParameterList::ParameterList()
{
    //nothing here
}

void        ParameterList::addParam(Parameter p)
{
    params[p.getName()]=p.getValue();
    paramsHelp[p.getName()]=p.getHelp();
    paramsType[p.getName()]=p.getType();
}

QJsonObject ParameterList::getParams()
{
    return params;
}

void        ParameterList::setParam(QString name,QString value,QString help)
{
    params[name]=value;
    if(help.size()!=0)
        paramsHelp[name]=help;
}

Parameter   ParameterList::getParam(QString name)
{
    if(!params.contains(name))
    {
        Parameter pt(name,"","");
        return pt;
    }
    Parameter pt;
    pt.setName(name);
    pt.setValue(params[name].toString());
    pt.setHelp(paramsHelp[name].toString());
    return pt;
}

QStringList ParameterList::getParameterNames() const
{
    return params.keys();
}

ParameterList::~ParameterList()
{
    //nothing here
}
