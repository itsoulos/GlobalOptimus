#ifndef PARAMWIDGET_H
#define PARAMWIDGET_H

#include <QObject>
#include <QWidget>
# include <QTableWidget>
# include <QLabel>
# include <QVBoxLayout>
class ParamWidget : public QWidget
{
    Q_OBJECT
private:
    QVBoxLayout *mainLayout;
    QTableWidget *mainTable;
public:
    explicit ParamWidget(QWidget *parent = nullptr);

signals:

};

#endif // PARAMWIDGET_H
