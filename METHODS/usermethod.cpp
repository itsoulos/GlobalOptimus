#include <METHODS/usermethod.h>

UserMethod::UserMethod()
{
    addParam(Parameter("userParam","1","Example user parameter"));
}

void    UserMethod::init()
{
    paramValue = getParam("userParam").getValue().toInt();
}

void    UserMethod::step()
{

}

bool    UserMethod::terminated()
{
    return true;
}

void    UserMethod::done()
{

}

UserMethod::~UserMethod()
{

}
