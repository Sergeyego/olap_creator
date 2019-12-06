#include "cubewidget.h"
#include "ui_cubewidget.h"

CubeWidget::CubeWidget(QString head, QStringList axes, QString qu, int dec, QWidget *parent) :
    QWidget(parent), query(qu),
    ui(new Ui::CubeWidget)
{
    ui->setupUi(this);
    ui->cmdUpd->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));
    ui->cmdSave->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton)));

    QCalendarWidget *begCalendarWidget = new QCalendarWidget(this);
    begCalendarWidget->setFirstDayOfWeek(Qt::Monday);
    ui->dateEditBeg->setCalendarWidget(begCalendarWidget);
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    QCalendarWidget *endCalendarWidget = new QCalendarWidget(this);
    endCalendarWidget->setFirstDayOfWeek(Qt::Monday);
    ui->dateEditEnd->setCalendarWidget(endCalendarWidget);
    ui->dateEditEnd->setDate(QDate::currentDate());

    axisX = new AxisWidget(axes, this);
    axisY = new AxisWidget(axes, this);
    ui->groupBoxX->layout()->addWidget(axisX);
    ui->groupBoxY->layout()->addWidget(axisY);

    this->setWindowTitle(head);
    olapmodel = new OlapModel(axes,dec,this);
    ui->tableView->setModel(olapmodel);
    updQuery();
    connect(ui->cmdUpd,SIGNAL(clicked()),this,SLOT(updQuery()));
    connect(ui->radioButtonSum,SIGNAL(clicked(bool)),olapmodel,SLOT(setTypeSum(bool)));
    connect(ui->radioButtonAvg,SIGNAL(clicked(bool)),olapmodel,SLOT(setTypeAvg(bool)));
    connect(ui->radioButtonMin,SIGNAL(clicked(bool)),olapmodel,SLOT(setTypeMin(bool)));
    connect(ui->radioButtonMax,SIGNAL(clicked(bool)),olapmodel,SLOT(setTypeMax(bool)));
    connect(axisX,SIGNAL(sigUpd(QStringList)),olapmodel,SLOT(setX(QStringList)));
    connect(axisY,SIGNAL(sigUpd(QStringList)),olapmodel,SLOT(setY(QStringList)));
    connect(ui->cmdSave,SIGNAL(clicked()),this,SLOT(saveXls()));
    connect(olapmodel,SIGNAL(sigRefresh()),ui->tableView,SLOT(resizeToContents()));
}

CubeWidget::~CubeWidget()
{
    delete ui;
}

void CubeWidget::updQuery()
{
    QString squery=query;
    squery.replace(":d1","'"+ui->dateEditBeg->date().toString("yyyy-MM-dd")+"'");
    squery.replace(":d2","'"+ui->dateEditEnd->date().toString("yyyy-MM-dd")+"'");
    olapmodel->setQuery(squery);
}

void CubeWidget::saveXls()
{
    ui->tableView->save(this->windowTitle()+tr(" с ")+ui->dateEditBeg->date().toString("dd.MM.yy")+tr(" по ")+ui->dateEditEnd->date().toString("dd.MM.yy"));
}
