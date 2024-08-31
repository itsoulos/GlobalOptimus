#ifndef PARAMWIDGET_H
#define PARAMWIDGET_H

#include <QObject>
#include <QWidget>
# include <QTableWidget>
# include <QLabel>
# include <QVBoxLayout>
# include <OPTIMUS/parameterlist.h>
/**
 * @brief The ParamWidget class is responsible to display
 *        parameters for the methods and for the problems.
 */
class ParamWidget : public QWidget
{
    Q_OBJECT
private:
    QVBoxLayout *mainLayout;
    QTableWidget *mainTable;
public:
    explicit ParamWidget(QWidget *parent = nullptr);
    ~ParamWidget();
signals:

};

#endif // PARAMWIDGET_H
