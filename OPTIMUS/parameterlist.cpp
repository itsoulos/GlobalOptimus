#include "parameterlist.h"

ParameterList::ParameterList()
{
    //nothing here
}

void        ParameterList::addParam(Parameter p)
{
    mparams<<p;
}

void   ParameterList::setParamValuesFromJson(QJsonObject &x)
{
    QStringList names= x.keys();
    for(int i=0;i<names.size();i++)
    {
        setParam(names[i],x[names[i]].toString());
    }
}

QJsonObject ParameterList::getParams()
{
    QJsonObject params;
    for(int i=0;i<mparams.size();i++)
        params[mparams[i].getName()]=mparams[i].getValue();
    return params;
}

void        ParameterList::setParam(QString name,QString value,QString help)
{
    for(int i=0;i<mparams.size();i++)
        if(mparams[i].getName()==name)
        {
            mparams[i].setValue(value);
            if(help.size()!=0)
                mparams[i].setHelp(help);
        }
}

Parameter   ParameterList::getParam(QString name)
{
    for(int i=0;i<mparams.size();i++)
        if(mparams[i].getName()==name)
            return mparams[i];
    Parameter pt(name,"","");
    return pt;
}

QStringList ParameterList::getParameterNames() const
{
    QStringList list;
    for(int i=0;i<mparams.size();i++)
        list<<mparams[i].getName();
    return list;
}

QVector<Parameter> ParameterList::getParamVector()
{
    return mparams;
}

bool ParameterList::contains(QString name) const
{
    for(int i=0;i<mparams.size();i++)
        if(mparams[i].getName()==name)
            return true;
    return false;
}

void    ParameterList::deleteParam(int index)
{
    mparams.remove(index);
}

Parameter ParameterList::getParam(int index)
{
    return mparams[index];
}

int ParameterList::countParameters() const
{
    return mparams.size();
}

ParameterList::~ParameterList()
{
    //nothing here
}
