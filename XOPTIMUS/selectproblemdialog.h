#ifndef SELECTPROBLEMDIALOG_H
#define SELECTPROBLEMDIALOG_H

# include <QDialog>
# include <QRadioButton>
# include <QGroupBox>
# include <QPushButton>
# include <QLabel>
# include <QVBoxLayout>
# include <QGridLayout>
# include <QHBoxLayout>
class SelectProblemDialog : public QDialog
{
Q_OBJECT
private:
    QPushButton *okButton,*cancelButton;
    QGroupBox *radioBox;
    int width,height;
    QString problemName;
    QStringList problemList;
public:
    SelectProblemDialog(QString sname,QStringList list,QWidget *parent=0);
    QString getSelectedName() const;
public slots:
    void okSlot();
    void cancelSlot();
};

#endif // SELECTPROBLEMDIALOG_H
