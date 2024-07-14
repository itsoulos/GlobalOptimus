#include "editlogger.h"

EditLogger::EditLogger(QTextEdit *e)
{
    edit = e;
}
void    EditLogger::printMessage(QString message)
{
    edit->append(message);
    edit->repaint();
}

EditLogger::~EditLogger()
{

}
