#ifndef EDITLOGGER_H
#define EDITLOGGER_H
# include <OPTIMUS/logger.h>
# include <QTextEdit>
class EditLogger : public Logger
{
private:
    QTextEdit *edit;
public:
    EditLogger(QTextEdit *e);
    virtual void printMessage(QString message);
    virtual ~EditLogger();
};

#endif // EDITLOGGER_H
