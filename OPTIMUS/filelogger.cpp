#include "filelogger.h"

FileLogger::FileLogger(QString n,bool aflag)
{
    filename = n;
    appendFlag = aflag;
    fp.setFileName(filename);
    if(appendFlag)
        fp.open(QIODevice::Append|QIODevice::Text);
    else
        fp.open(QIODevice::WriteOnly|QIODevice::Text);
    stream.setDevice(&fp);
}

void    FileLogger::printMessage(QString message)
{
    stream<<message;
}

FileLogger::~FileLogger()
{
    fp.close();
}
