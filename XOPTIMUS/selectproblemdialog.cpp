#include "selectproblemdialog.h"
# include <QDebug>
# include <QScreen>
# include <QGuiApplication>
# include <OPTIMUS/problem.h>
# include <XOPTIMUS/problemloader.h>
SelectProblemDialog::SelectProblemDialog(QString sname,
                                         QStringList list,QWidget *parent)
    :QDialog(parent)
{
	QScreen *screen = QGuiApplication::primaryScreen();
	QRect wd = screen->geometry();
    width = 3*wd.width()/4;
    height =3*wd.height()/4;
    problemName = sname;
    problemList = list;
    this->setWindowTitle(tr("Problem Selection"));

    this->setFixedSize(width,height);
    QWidget *w1=new QWidget(this);
    w1->setGeometry(0,0,width,height);
    QVBoxLayout *l1=new QVBoxLayout();
    w1->setLayout(l1);
    problemCombo = new QComboBox;
    for(int i=0;i<problemList.size();i++)
    {
        problemCombo->addItem(problemList[i]);
    }
    l1->addWidget(problemCombo);
    problemEdit =new QTextEdit;
    l1->addWidget(problemEdit);
    problemEdit->setReadOnly(true);
    connect(problemCombo,SIGNAL(currentTextChanged(QString)),this,
            SLOT(comboSelectItem(QString)));

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
    comboSelectItem(problemList[0]);
}

void    SelectProblemDialog::comboSelectItem(QString name)
{
    //show info about the selected problem
    if(name=="rbfproblem" || name == "mlpproblem") return;
    Problem *myProblem;
    myProblem = ProblemLoader::getProblemFromName(name);
    QString Info = ProblemLoader::getProblemReport(name,myProblem);
    problemEdit->setText(Info);
    delete myProblem;
}
QString     SelectProblemDialog::getSelectedName() const
{
    return problemName;
}

void SelectProblemDialog::okSlot()
{
    problemName=problemCombo->currentText();
    accept();
}

void SelectProblemDialog::cancelSlot()
{
    problemName = "";
    reject();
}
