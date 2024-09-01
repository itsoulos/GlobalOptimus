#include "paramwidget.h"

ParamWidget::ParamWidget(ParameterList &list,QWidget *parent)
    : QWidget{parent}
{
    useJsonValues = false;
    paramList = list;
    createTable();
}

ParamWidget::ParamWidget(QJsonObject &x,QWidget *parent)
    :QWidget(parent)
{
    useJsonValues = true;
    jsonObject = x;
    createTable();
}

QSize   ParamWidget::getIconSize()
{
    return QSize(wd.height()/15,wd.height()/15);
}

void    ParamWidget::createTable()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    wd = screen->geometry();


    mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mainTable = new QTableWidget;
    mainLayout->addWidget(mainTable);
    QStringList m_TableHeader;
    m_TableHeader<<tr("Name")<<tr("Value")<<tr("Add/Update")<<tr("Remove");
    mainTable->setColumnCount(4);

    mainTable->setShowGrid(true);
    mainTable->setSelectionMode(QAbstractItemView::SingleSelection);
    mainTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mainTable->setHorizontalHeaderLabels(m_TableHeader);
    mainTable->horizontalHeader()->setStretchLastSection(true);
     mainTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    updateTable();
   /* mainTable->setHorizontalHeaderLabels(m_TableHeader);
    mainTable->verticalHeader()->setVisible(false);
    mainTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mainTable->setSelectionMode(QAbstractItemView::SingleSelection);
    mainTable->setShowGrid(true);
    mainTable->horizontalHeader()->setStretchLastSection(true);
    mainTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);*/
}

QPushButton *ParamWidget::addAddButton(int index)
{
    QPushButton *updateButton=new QPushButton();
    updateButton->setIcon(QIcon(QPixmap(":/images/XOPTIMUS/update.png")));
    updateButton->setIconSize(getIconSize());
    updateButton->setProperty("position",QVariant(index));
    connect(updateButton,SIGNAL(clicked(bool)),this,SLOT(updateSlot()));
    return updateButton;
}

QPushButton *ParamWidget::addDelButton(int index)
{
    QPushButton *deleteButton=new QPushButton();
    deleteButton->setIcon(QIcon(QPixmap(":/images/XOPTIMUS/minus.png")));
    deleteButton->setIconSize(getIconSize());
    deleteButton->setProperty("position",QVariant(index));
    connect(deleteButton,SIGNAL(clicked(bool)),this,SLOT(deleteSlot()));
    return deleteButton;
}

QWidget *ParamWidget::addEntryWidget(int index)
{
    if(useJsonValues)
    {
        QLineEdit *paramValue=new QLineEdit();
        QString name = jsonObject.keys().at(index);
        paramValue->setText(jsonObject[name].toString());
        paramValue->setAlignment(Qt::AlignCenter);
        return paramValue;
    }
    return NULL;
}
void    ParamWidget::updateTable()
{
    while(mainTable->rowCount()!=0)
        mainTable->removeRow(0);

    if(useJsonValues)
    {
        mainTable->setRowCount(jsonObject.size()+1);

        for(int i=0;i<jsonObject.size();i++)
        {
            QLineEdit *paramName=new QLineEdit();
            paramName->setAlignment(Qt::AlignCenter);
            paramName->setText(jsonObject.keys().at(i));
            mainTable->setCellWidget(i,0,paramName);
            QLineEdit *paramValue=(QLineEdit *)addEntryWidget(i);
            mainTable->setCellWidget(i,1,paramValue);
            QPushButton *updateButton=addAddButton(i);
            mainTable->setCellWidget(i,2,updateButton);
            QPushButton *deleteButton=addDelButton(i);
            mainTable->setCellWidget(i,3,deleteButton);
        }
    }
    else
    {

    }
    int lastRow = mainTable->rowCount()-1;
    QLineEdit *addName=new QLineEdit();
    mainTable->setCellWidget(lastRow,0,addName);
    QLineEdit *addValue=new QLineEdit();
    mainTable->setCellWidget(lastRow,1,addValue);
    QPushButton *newAddButton=new QPushButton();
    newAddButton->setIcon(QIcon(QPixmap(":/images/XOPTIMUS/add.png")));
    newAddButton->setIconSize(getIconSize());
    mainTable->setCellWidget(lastRow,2,newAddButton);
    connect(newAddButton,SIGNAL(clicked(bool)),this,SLOT(insertSlot()));
    mainTable->resizeRowsToContents();
}

void    ParamWidget::updateSlot()
{
    QObject *s=QObject::sender();
    QVariant v=((QPushButton *)s)->property("position");
    int position=v.toInt();
    QString name=((QLineEdit *)mainTable->cellWidget(position,0))->text();
    if(useJsonValues)
    {
        QString value=((QLineEdit *)mainTable->cellWidget(position,1))->text();
        jsonObject[name]=value;
    }
    else
    {

    }
    updateTable();
}

void    ParamWidget::deleteSlot()
{
    QObject *s=QObject::sender();
    QVariant v=((QPushButton *)s)->property("position");
    int position=v.toInt();
    QString name=((QLineEdit *)mainTable->cellWidget(position,0))->text();
    if(useJsonValues)
    {
        jsonObject.remove(name);
    }
    else
    {

    }
    updateTable();
}
void    ParamWidget::insertSlot()
{
    int lastRow = mainTable->rowCount()-1;
    QString name=((QLineEdit *)mainTable->cellWidget(lastRow,0))->text();
    QString value=((QLineEdit *)mainTable->cellWidget(lastRow,1))->text();
    if(useJsonValues)
    {
        jsonObject[name]=value;
    }
    else
    {

    }
    updateTable();
}
ParamWidget::~ParamWidget()
{

}
