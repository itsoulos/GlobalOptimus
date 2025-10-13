#include "parameterdialog.h"
# include <QScreen>
# include <QLabel>
# include <QHBoxLayout>
ParameterDialog::ParameterDialog(
        QJsonObject params,
        QString name,QWidget *parent)
    :QDialog(parent)
{
    obj  =params;
    initView(name,true);
}

ParameterDialog::ParameterDialog(
        ParameterList &params,
        QString name,QWidget *parent)
    :QDialog(parent)
{
    paramList = params;
    initView(name,false);
}
void     ParameterDialog::initView(QString name,bool isJson)
{
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect wd = screen->geometry();
    WIDTH = 7*wd.width()/8;
    HEIGHT =7*wd.height()/8;
    problemName=name;
    this->setWindowTitle("Problem settings for function "+name);
    this->setFixedSize(WIDTH,HEIGHT);
    QWidget *w1=new QWidget(this);
    w1->setGeometry(0,0,WIDTH,HEIGHT);
    QVBoxLayout *l1=new QVBoxLayout();
    w1->setLayout(l1);
    if(isJson)
    {

        paramWidget = new ParamWidget(obj);
    }
    else
    {
        paramWidget = new ParamWidget(paramList);
    }
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

ParameterList ParameterDialog::getParameterList() const
{
    return paramList;
}

void    ParameterDialog::okSlot()
{
    obj = paramWidget->getJson();
    paramList = paramWidget->getParamList();
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
