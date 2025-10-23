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
    else
    {
        Parameter param = paramList.getParam(index);
        int type = param.getType();
        switch(type)
        {
        case PARAM_STRING:
        {
            QLineEdit *paramValue=new QLineEdit();
            paramValue->setText(param.getValue());
            paramValue->setAlignment(Qt::AlignCenter);
            return paramValue;
            break;
        }
        case PARAM_DOUBLE:
        {
            QDoubleSpinBox *spin = new QDoubleSpinBox;
            spin->setLocale(QLocale::c());

            spin->setDecimals(10);
            spin->setMinimum(param.getLowDoubleValue());
            spin->setMaximum(param.getUpperDoubleValue());
            //spin->setSingleStep(0.00001);
            spin->setSingleStep(0.01*(param.getUpperDoubleValue()-param.getLowDoubleValue()));
            spin->setValue(param.getValue().toDouble());
            return spin;
        }
            break;
        case PARAM_FILE:
        {
            QPushButton *paramButton = new QPushButton;
            paramButton->setText(param.getValue().length()==0?"LOAD FILE":param.getValue());
            connect(paramButton,SIGNAL(clicked()),this,SLOT(loadFileSlot()));
            return paramButton;

            QLineEdit *paramValue=new QLineEdit();
            paramValue->setText(param.getValue());
            paramValue->setAlignment(Qt::AlignCenter);
            return paramValue;
            break;
        }
        case PARAM_INTEGER:
        {
            QSpinBox *spin = new QSpinBox;
            spin->setMinimum(param.getLowIntValue());
            spin->setMaximum(param.getUpperIntValue());
            spin->setValue(param.getValue().toInt());
            return spin;
            break;
        }
        case PARAM_LIST:
        {
            QComboBox *combo = new QComboBox;
            QStringList list = param.getStringValues();
            combo->addItems(list);
            combo->setCurrentText(param.getValue());
            return combo;
            break;
        }
        }
    }
    return NULL;
}

void    ParamWidget::loadFileSlot()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), ".", tr("Data Files (*.train *.test *.data *.csv *.arff)"));;
    if(fileName.length()!=0)
    {
        QPushButton *bt = (QPushButton *)sender();
        bt->setText(fileName);
    }
}
void    ParamWidget::updateTable()
{
    while(mainTable->rowCount()!=0)
        mainTable->removeRow(0);
    int minHeight = 105*getIconSize().height()/100;;
    if(useJsonValues)
    {
        mainTable->setRowCount(jsonObject.size()+1);

        for(int i=0;i<jsonObject.size();i++)
        {
            mainTable->setRowHeight(i,minHeight);
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
        int n = paramList.countParameters();
        QStringList names = paramList.getParameterNames();
        mainTable->setRowCount(n+1);
        for(int i=0;i<n;i++)
        {
            mainTable->setRowHeight(i,minHeight);
            QLineEdit *paramName=new QLineEdit();
            paramName->setAlignment(Qt::AlignCenter);
            paramName->setText(names[i]);
            mainTable->setCellWidget(i,0,paramName);
            QWidget *paramValue=addEntryWidget(i);
            mainTable->setCellWidget(i,1,paramValue);
            QPushButton *updateButton=addAddButton(i);
            mainTable->setCellWidget(i,2,updateButton);
            QPushButton *deleteButton=addDelButton(i);
            mainTable->setCellWidget(i,3,deleteButton);

        }
    }

        int lastRow = mainTable->rowCount()-1;
        mainTable->setRowHeight(lastRow,minHeight);
        QLineEdit *addName=new QLineEdit();
        mainTable->setCellWidget(lastRow,0,addName);
        QLineEdit *addValue=new QLineEdit();
        mainTable->setCellWidget(lastRow,1,addValue);
        QPushButton *newAddButton=new QPushButton();
        newAddButton->setIcon(QIcon(QPixmap(":/images/XOPTIMUS/add.png")));
        newAddButton->setIconSize(getIconSize());
        mainTable->setCellWidget(lastRow,2,newAddButton);
        connect(newAddButton,SIGNAL(clicked(bool)),this,SLOT(insertSlot()));

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
        Parameter param = paramList.getParam(position);
        int type = param.getType();
        switch(type)
        {
        case PARAM_DOUBLE:
        {
            QDoubleSpinBox *spin = (QDoubleSpinBox *)mainTable->cellWidget(position,1);
            param.setValue(QString::number(spin->value()));
            break;
        }
        case PARAM_FILE:
        {
            QString value=((QPushButton *)mainTable->cellWidget(position,1))->text();
            param.setValue(value);
            break;
        }
        case PARAM_INTEGER:
        {
            QSpinBox *spin = (QSpinBox *)mainTable->cellWidget(position,1);
            param.setValue(QString::number(spin->value()));
            break;
        }
        case PARAM_LIST:
        {
            QComboBox *combo = (QComboBox *)mainTable->cellWidget(position,1);
            QString paramText = combo->currentText();
            param.setValue(combo->currentText());
            break;
        }
        case PARAM_STRING:
        {
            QString value=((QLineEdit *)mainTable->cellWidget(position,1))->text();
            param.setValue(value);
            break;
        }
        }
        paramList.setParam(param.getName(),param.getValue());
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
       paramList.deleteParam(position);
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
        Parameter p(name,value,"");
        paramList.addParam(p);
    }
    updateTable();
}

QJsonObject ParamWidget::getJson()
{
    return jsonObject;
}

ParameterList   ParamWidget::getParamList()
{
    return paramList;
}

void    ParamWidget::setParamList(ParameterList &list)
{
    paramList = list;
    updateTable();
}
ParamWidget::~ParamWidget()
{

}
