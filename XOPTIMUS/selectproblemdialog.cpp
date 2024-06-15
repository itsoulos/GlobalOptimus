#include "selectproblemdialog.h"
# include <QDebug>
# include <QScreen>
# include <QGuiApplication>
SelectProblemDialog::SelectProblemDialog(QString sname,
                                         QStringList list,QWidget *parent)
    :QDialog(parent)
{
	QScreen *screen = QGuiApplication::primaryScreen();
	QRect wd = screen->geometry();
    width = wd.width()/2;
    height =3*wd.height()/4;
    problemName = sname;
    problemList = list;
    this->setWindowTitle(tr("Problem Selection"));
    this->setFixedSize(width,height);
    QWidget *w1=new QWidget(this);
    w1->setGeometry(0,0,width,height);
    QVBoxLayout *l1=new QVBoxLayout();
    w1->setLayout(l1);
    radioBox = new QGroupBox("Function list");
    l1->addWidget(radioBox);
    QGridLayout *vbox = new QGridLayout;
    for(int i=0;i<problemList.size();i++)
    {
        QString x = problemList[i];
        QRadioButton *bt = new QRadioButton(x);
        vbox->addWidget(bt,i/3,i%3);
        if(x == problemName)
            bt->setChecked(true);
    }
    radioBox->setLayout(vbox);
    QHBoxLayout *buttonLayout=new QHBoxLayout();
    l1->addLayout(buttonLayout);
    okButton=new QPushButton();
    okButton->setText(tr("OK"));
    connect(okButton,SIGNAL(clicked(bool)),this,SLOT(okSlot()));
    buttonLayout->addWidget(okButton);
    cancelButton=new QPushButton();
    cancelButton->setText(tr("CANCEL"));
    connect(cancelButton,SIGNAL(clicked(bool)),this,SLOT(cancelSlot()));
    buttonLayout->addWidget(cancelButton);
}

QString     SelectProblemDialog::getSelectedName() const
{
    return problemName;
}

void SelectProblemDialog::okSlot()
{
        foreach (QRadioButton *button,
                 radioBox->findChildren<QRadioButton*>()) {
            if (button->isChecked()) {

                problemName = button->text();
                break;
            }
        }
        accept();
}

void SelectProblemDialog::cancelSlot()
{
    problemName = "";
    reject();
}
