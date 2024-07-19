#include "logger.h"
# include <QDebug>
Logger::Logger()
{
    //default constructor
}

void    Logger::printMessage(QString message)
{
    qDebug().noquote()<<message;
}

Logger::~Logger()
{
    //nothing here
}
