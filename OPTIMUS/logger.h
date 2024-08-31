#ifndef LOGGER_H
#define LOGGER_H
# include <QString>
# include <QStringList>
# include <QFile>
# include <QTextStream>

/**
 * @brief The Logger class is a virtual class responsible
 * to display log messages for the optimizers.
 */
class Logger
{
public:
    /**
     * @brief Logger the default constructor of the virtual class
     */
    Logger();
    /**
     * @brief printMessage is an abstract method that displays log
     * messages on any platform.
     * @param message
     */
    virtual void printMessage(QString message);
    virtual ~Logger();
};

#endif // LOGGER_H
