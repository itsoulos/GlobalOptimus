#ifndef PARAMETERDIALOG_H
#define PARAMETERDIALOG_H

#include <QObject>
#include <QWidget>
# include <QDialog>
# include <QPushButton>
# include <QTableView>
# include <QTableWidget>
# include <QJsonArray>
# include <QJsonDocument>
# include <QJsonObject>
# include <QJsonValue>
# include <QHBoxLayout>
# include <QVBoxLayout>
# include <QHeaderView>
# include <QLineEdit>
# include <QIcon>
# include <QPixmap>
#include <QFile>
#include <QFileDialog>
# include <QApplication>
# include <XOPTIMUS/paramwidget.h>
# include <OPTIMUS/parameterlist.h>
class ParameterDialog : public QDialog
{
    Q_OBJECT
private:
    ParamWidget  *paramWidget;
    QPushButton *ok,*cancel;
    int WIDTH,HEIGHT;

     QJsonObject obj;
     ParameterList paramList;
     QString problemName;
public:
    ParameterDialog(QJsonObject params,QString name,QWidget *parent=0);
    ParameterDialog(ParameterList &list,QString name,QWidget *parent=0);
    void     initView(QString name,bool isJson=false);
    QJsonObject getParams() const;
    ParameterList getParameterList() const;
    ~ParameterDialog();
public slots:
    void    okSlot();
    void    cancelSlot();
};

#endif // PARAMETERDIALOG_H
