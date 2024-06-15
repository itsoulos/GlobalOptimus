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

class ParameterDialog : public QDialog
{
    Q_OBJECT
private:
    QTableWidget *table;
    QPushButton *ok,*cancel;
    QPushButton *loadParameterButton;
    int WIDTH,HEIGHT;

     QJsonObject obj;
     void   updateTable();
     QString problemName;
public:
    ParameterDialog(QJsonObject params,QString name,QWidget *parent=0);
    QJsonObject getParams() const;
    ~ParameterDialog();
public slots:
    void    okSlot();
    void    cancelSlot();
    void    insertSlot();
    void    updateSlot();
    void    deleteSlot();
    void    loadParameterSlot();
};

#endif // PARAMETERDIALOG_H
