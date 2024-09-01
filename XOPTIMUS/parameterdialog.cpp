#include "parameterdialog.h"
# include <QScreen>
# include <QLabel>
# include <QHBoxLayout>
ParameterDialog::ParameterDialog(
        QJsonObject params,
        QString name,QWidget *parent)
    :QDialog(parent)
{

	QScreen *screen = QGuiApplication::primaryScreen();
	QRect wd = screen->geometry();
    WIDTH = 3*wd.width()/4;
    HEIGHT =3*wd.height()/4;
    problemName=name;
    obj = params;
    this->setWindowTitle("Problem settings for function "+name);
    this->setFixedSize(WIDTH,HEIGHT);
    QWidget *w1=new QWidget(this);
    w1->setGeometry(0,0,WIDTH,HEIGHT);
    QVBoxLayout *l1=new QVBoxLayout();
    w1->setLayout(l1);
    paramWidget = new ParamWidget(params);
    l1->addWidget(paramWidget);

    QHBoxLayout *buttonLayout=new QHBoxLayout();
    l1->addLayout(buttonLayout);


    ok=new QPushButton();
    ok->setText(tr("OK"));
    connect(ok,SIGNAL(clicked(bool)),this,SLOT(okSlot()));
    buttonLayout->addWidget(ok);
    cancel=new QPushButton();
    cancel->setText(tr("CANCEL"));
    connect(cancel,SIGNAL(clicked(bool)),this,SLOT(cancelSlot()));
    buttonLayout->addWidget(cancel);
}

QJsonObject ParameterDialog::getParams() const
{
    return obj;
}



void    ParameterDialog::okSlot()
{
    QJsonDocument doc(obj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    accept();
}


void    ParameterDialog::cancelSlot()
{
    QString s="{}";
    QJsonDocument doc = QJsonDocument::fromJson(s.toUtf8());
    obj=doc.object();
    reject();
}


ParameterDialog::~ParameterDialog()
{

}
