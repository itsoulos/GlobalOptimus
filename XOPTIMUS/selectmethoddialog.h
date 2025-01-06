#ifndef SELECTMETHODDIALOG_H
#define SELECTMETHODDIALOG_H

# include <QDialog>
# include <QRadioButton>
# include <QGroupBox>
# include <QPushButton>
# include <QLabel>
# include <QVBoxLayout>
# include <QGridLayout>
# include <QHBoxLayout>
# include <QComboBox>
# include <XOPTIMUS/paramwidget.h>
# include <OPTIMUS/methodloader.h>
class SelectMethodDialog :public QDialog
{
Q_OBJECT
private:
    QPushButton *okButton,*cancelButton;
    int width,height;
    QStringList methodList;
    QString selectedMethod;
    QComboBox *methodCombo;
    MethodLoader *methodLoader;
    ParamWidget *paramWidget;
public:
    SelectMethodDialog(QStringList list,MethodLoader *loader,QWidget *parent =0);
    QString getSelectedMethod() const;
public slots:
    void onMethodSelected(const QString &text);
    void okSlot();
    void cancelSlot();
};

#endif // SELECTMETHODDIALOG_H
