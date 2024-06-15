#ifndef SELECTMETHODDIALOG_H
#define SELECTMETHODDIALOG_H

# include <QDialog>
# include <QRadioButton>
# include <QGroupBox>
# include <QPushButton>
# include <QLabel>
# include <QVBoxLayout>
# include <QDesktopWidget>
# include <QGridLayout>
# include <QHBoxLayout>
class SelectMethodDialog :public QDialog
{
Q_OBJECT
private:
    QPushButton *okButton,*cancelButton;
    QGroupBox *radioBox;
    int width,height;
    QStringList methodList;
    QString selectedMethod;
public:
    SelectMethodDialog(QStringList list,QWidget *parent =0);
    QString getSelectedMethod() const;
public slots:
    void okSlot();
    void cancelSlot();
};

#endif // SELECTMETHODDIALOG_H
