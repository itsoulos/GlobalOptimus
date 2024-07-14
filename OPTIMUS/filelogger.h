#ifndef FILELOGGER_H
#define FILELOGGER_H
# include <QFile>
# include <QTextStream>
# include <OPTIMUS/logger.h>
class FileLogger : public Logger
{
private:
    QString filename;
    bool appendFlag;
    QFile fp;
    QTextStream stream;
public:
    FileLogger(QString n,bool aflag=true);
    virtual void printMessage(QString message);
    virtual ~FileLogger();
};

#endif // FILELOGGER_H
