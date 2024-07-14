#ifndef LOGGER_H
#define LOGGER_H
# include <QString>
# include <QStringList>
# include <QFile>
# include <QTextStream>

class Logger
{
public:
    Logger();
    virtual void printMessage(QString message);
    virtual ~Logger();
};

#endif // LOGGER_H
