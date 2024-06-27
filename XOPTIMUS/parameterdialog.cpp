#include "parameterdialog.h"
# include <QScreen>
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
    //this->setFixedSize(800,200);
    QWidget *w1=new QWidget(this);
    w1->setGeometry(0,0,1000,400);
    QVBoxLayout *l1=new QVBoxLayout();
    w1->setLayout(l1);

    table=new QTableWidget();
    l1->addWidget(table);
    //table->setFixedWidth(95 *WIDTH/100);
    QStringList m_TableHeader;

    m_TableHeader<<tr("Name")<<tr("Value")<<tr("Add/Update")<<tr("Remove");
    table->setColumnCount(4);
    //table->setFixedHeight(100);
    table->setRowHeight(2, 50);
    table->setHorizontalHeaderLabels(m_TableHeader);
    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setShowGrid(true);
    table->horizontalHeader()->setStretchLastSection(true);


    updateTable();
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QHBoxLayout *buttonLayout=new QHBoxLayout();
    l1->addLayout(buttonLayout);

    loadParameterButton=new QPushButton();
    loadParameterButton->setText(tr("LOAD PARAMETERS"));
    connect(loadParameterButton,SIGNAL(clicked(bool)),this,SLOT(loadParameterSlot()));
    buttonLayout->addWidget(loadParameterButton);
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

void    ParameterDialog::updateTable()
{
    while(table->rowCount()!=0)
        table->removeRow(0);
    table->setRowCount(obj.size()+1);
    for(int i=0;i<obj.size();i++)
    {
        QLineEdit *paramName=new QLineEdit();
        paramName->setText(obj.keys().at(i));
        table->setCellWidget(i,0,paramName);
        QLineEdit *paramValue=new QLineEdit();
        paramValue->setText(obj[paramName->text()].toString());
        table->setCellWidget(i,1,paramValue);
        QPushButton *updateButton=new QPushButton();
        updateButton->setIcon(QIcon(QPixmap(":/images/XOPTIMUS/update.png")));
        //updateButton->setIconSize(QSize(WIDTH/8,HEIGHT/14));
        updateButton->setProperty("position",QVariant(i));
        connect(updateButton,SIGNAL(clicked(bool)),this,SLOT(updateSlot()));
        table->setCellWidget(i,2,updateButton);
        QPushButton *deleteButton=new QPushButton();
        deleteButton->setIcon(QIcon(QPixmap(":/images/XOPTIMUS/minus.png")));
        deleteButton->setIconSize(updateButton->iconSize());
        deleteButton->setProperty("position",QVariant(i));
        connect(deleteButton,SIGNAL(clicked(bool)),this,SLOT(deleteSlot()));
        table->setCellWidget(i,3,deleteButton);
    }
    QLineEdit *addName=new QLineEdit();
    table->setCellWidget(obj.size(),0,addName);
    QLineEdit *addValue=new QLineEdit();
    table->setCellWidget(obj.size(),1,addValue);
    QPushButton *addAddButton=new QPushButton();
    addAddButton->setIcon(QIcon(QPixmap(":/images/XOPTIMUS/add.png")));
    //addAddButton->setIconSize(QSize(WIDTH/8,HEIGHT/14));
    table->setCellWidget(obj.size(),2,addAddButton);
    connect(addAddButton,SIGNAL(clicked(bool)),this,SLOT(insertSlot()));

    table->resizeColumnsToContents();
    table->resizeRowsToContents();
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

void    ParameterDialog::insertSlot()
{
    QString name=((QLineEdit *)table->cellWidget(obj.size(),0))->text();
    QString value=((QLineEdit *)table->cellWidget(obj.size(),1))->text();
    obj[name]=value;
    updateTable();
}

void    ParameterDialog::updateSlot()
{
    QObject *s=QObject::sender();
    QVariant v=((QPushButton *)s)->property("position");
    int position=v.toInt();
    QString name=((QLineEdit *)table->cellWidget(position,0))->text();
    QString value=((QLineEdit *)table->cellWidget(position,1))->text();
    obj[name]=value;
    updateTable();
}

void    ParameterDialog::deleteSlot()
{
    QObject *s=QObject::sender();
    QVariant v=((QPushButton *)s)->property("position");
    int position=v.toInt();
    QString name=((QLineEdit *)table->cellWidget(position,0))->text();
    obj.remove(name);
    updateTable();
}

void    ParameterDialog::loadParameterSlot()
{
//load parameter settings from a json file
    QStringList files = QFileDialog::getOpenFileNames(
                            this,
                            "Select a json file to open",
                            ".",
                            "Text file (*)");
    if(files.size()!=0)
    {
        QFile f(files[0]);
        if (!f.exists())
        {
            qDebug()<<"Unable to open file not found\n";
        }
        else
        {
            f.open(QFile::ReadOnly | QFile::Text);
            QTextStream ts(&f);
            QString parameterJson=ts.readAll();

             QJsonDocument doc = QJsonDocument::fromJson(parameterJson.toUtf8());

             QJsonObject object=doc.object();
             QJsonObject::iterator it;
             for(it=obj.begin();it!=obj.end();it++)
             {
                 QString key=it.key();
                 obj.remove(key);
             }
                 for (it = object.begin(); it != object.end(); it++) {

                     QString key = it.key();

                     QString value = it.value().toString();
                     obj[key]=value;
                     updateTable();
                 }
        }
    }

}

ParameterDialog::~ParameterDialog()
{

}
