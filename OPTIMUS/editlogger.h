#ifndef EDITLOGGER_H
#define EDITLOGGER_H
# include <OPTIMUS/logger.h>
# include <QTextEdit>
/**
 * @brief The EditLogger class implements logging in EditBox
 *  of QT
 */
class EditLogger : public Logger
{
private:
    QTextEdit *edit;
public:
    /**
     * @brief EditLogger the default constructor of the class
     * @param e, the textedit where the messages will be displayed.
     */
    EditLogger(QTextEdit *e);
    /**
     * @brief printMessage shows a  new message in the provided textedit
     * @param message, the message that will be displayed.
     */
    virtual void printMessage(QString message);
    virtual ~EditLogger();
};

#endif // EDITLOGGER_H
