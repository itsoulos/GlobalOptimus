#include "selectmethoddialog.h"
# include <QScreen>
# include <QGuiApplication>

SelectMethodDialog::SelectMethodDialog(QStringList list,
                                       MethodLoader *loader,
                                       QWidget *parent )
    :QDialog(parent)
{
    methodLoader = loader;
	QScreen *screen = QGuiApplication::primaryScreen();
	QRect wd = screen->geometry();
    width = 4*wd.width()/5;
    height =4*wd.height()/5;
    methodList = list;
    this->setWindowTitle(tr("Optimizer Selection"));
    this->setFixedSize(width,height);
    QWidget *w1=new QWidget(this);
    w1->setGeometry(0,0,width,height);
    QVBoxLayout *l1=new QVBoxLayout();
    w1->setLayout(l1);
    QLabel *title = new QLabel;
    title->setText("<h3 align=center>Select a method from the list</h3>");
    title->setFixedSize(QSize(width,height/10));
    title->setAlignment(Qt::AlignTop);
    l1->addWidget(title);

    methodCombo = new QComboBox;
    methodCombo->addItems(methodList);
    l1->addWidget(methodCombo,0,Qt::AlignCenter);
    methodCombo->setFixedSize(QSize(95*width/100,height/15));
    connect(methodCombo,SIGNAL(currentTextChanged(QString)),this,
            SLOT(onMethodSelected(QString)));

    ParameterList paramList=methodLoader->getSelectedMethod(methodList[0])->getParameterList();
    paramWidget=new ParamWidget(paramList);
    l1->addWidget(paramWidget,0,Qt::AlignCenter);
    paramWidget->setFixedSize(QSize(95*width/100,7*height/10));

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

void    SelectMethodDialog::onMethodSelected(const QString &text)
{
    ParameterList paramList=methodLoader->getSelectedMethod(text)->getParameterList();
    paramWidget->setParamList(paramList);
}
QString SelectMethodDialog::getSelectedMethod() const
{
    return selectedMethod;
}

void SelectMethodDialog::okSlot()
{
    selectedMethod = methodCombo->currentText();
    QJsonObject x = paramWidget->getJson();
    methodLoader->getSelectedMethod(selectedMethod)->setParams(x);
    accept();
}

void SelectMethodDialog::cancelSlot()
{
    selectedMethod="";
    reject();
}
