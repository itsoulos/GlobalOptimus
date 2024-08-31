#ifndef FILELOGGER_H
#define FILELOGGER_H
# include <QFile>
# include <QTextStream>
# include <OPTIMUS/logger.h>
/**
 * @brief The FileLogger class implements loggin in files
 * for the used Optimizers
 */
class FileLogger : public Logger
{
private:
    QString filename;
    bool appendFlag;
    QFile fp;
    QTextStream stream;
public:
    /**
     * @brief FileLogger the default constructor of the FileLogger
     * @param n, the name of the file where loggin will show
     * @param aflag, when is true the messages will be appended, else
     *        the file will be erased before first output.
     */
    FileLogger(QString n,bool aflag=true);
    /**
     * @brief printMessage prints a message to the file
     * @param message, the message that will be printed.
     */
    virtual void printMessage(QString message);
    virtual ~FileLogger();
};

#endif // FILELOGGER_H
