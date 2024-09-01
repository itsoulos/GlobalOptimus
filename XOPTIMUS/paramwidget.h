#ifndef PARAMWIDGET_H
#define PARAMWIDGET_H

#include <QObject>
#include <QWidget>
# include <QTableWidget>
# include <QHeaderView>
# include <QLabel>
# include <QVBoxLayout>
# include <QPushButton>
# include <QLineEdit>
# include <QScreen>
# include <QGuiApplication>
# include <QDoubleSpinBox>
# include <QSpinBox>
# include <QComboBox>
# include <QFileSelector>
# include <OPTIMUS/parameterlist.h>
/**
 * @brief The ParamWidget class is responsible to display
 *        parameters for the methods and for the problems.
 */
class ParamWidget : public QWidget
{
    Q_OBJECT
private:
    QRect           wd;
    QVBoxLayout     *mainLayout;
    QTableWidget    *mainTable;
    ParameterList   paramList;
    QJsonObject     jsonObject;
    bool            useJsonValues=false;
    void            createTable();
    void            updateTable();
    QPushButton     *addAddButton(int index);
    QPushButton     *addDelButton(int index);
    QWidget         *addEntryWidget(int index);
    QSize           getIconSize();
public:
    explicit ParamWidget(ParameterList &list,QWidget *parent = nullptr);
    explicit ParamWidget(QJsonObject   &x,   QWidget *parent = nullptr);
    QJsonObject getJson();
    ParameterList getParamList();
    ~ParamWidget();
public slots:
    void updateSlot();
    void deleteSlot();
    void insertSlot();
};

#endif // PARAMWIDGET_H
