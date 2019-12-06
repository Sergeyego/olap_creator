#ifndef CUBEWIDGET_H
#define CUBEWIDGET_H

#include <QWidget>
#include "olapmodel.h"
#include <QCalendarWidget>
#include "axiswidget.h"

namespace Ui {
class CubeWidget;
}

class CubeWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit CubeWidget(QString head, QStringList axes, QString qu, int dec, QWidget *parent = 0);
    ~CubeWidget();
    
private:
    Ui::CubeWidget *ui;
    OlapModel *olapmodel;
    QString query;
    AxisWidget *axisX;
    AxisWidget *axisY;


private slots:
    void updQuery();
    void saveXls();

};

#endif // CUBEWIDGET_H
