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
# include <QComboBox>
# include <QTextEdit>
class SelectProblemDialog : public QDialog
{
Q_OBJECT
private:
    QPushButton *okButton,*cancelButton;
    int width,height;
    QString problemName;
    QStringList problemList;
    QComboBox *problemCombo;
    QTextEdit *problemEdit;
public:
    SelectProblemDialog(QString sname,QStringList list,QWidget *parent=0);
    QString getSelectedName() const;
public slots:
    void okSlot();
    void cancelSlot();
    void comboSelectItem(QString text);
};

#endif // SELECTPROBLEMDIALOG_H
