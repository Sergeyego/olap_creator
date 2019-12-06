#include "dialogopen.h"
#include "ui_dialogopen.h"

DialogOpen::DialogOpen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOpen)
{
    ui->setupUi(this);

    model = new QSqlQueryModel(this);
    model->setQuery("select id, nam, columns, query, dc from olaps order by nam");
    if (model->lastError().isValid()){

    } else {
        ui->tableView->setModel(model);
        model->setHeaderData(0,Qt::Horizontal,QString("Id"));
        model->setHeaderData(1,Qt::Horizontal,QString("Название"));
        for (int i=2; i<model->columnCount();i++){
            ui->tableView->setColumnHidden(i,true);
        }
        ui->tableView->resizeColumnsToContents();
    }
}

DialogOpen::~DialogOpen()
{
    delete ui;
}

int DialogOpen::id() const
{
    return 0;
}

QString DialogOpen::title() const
{
    return QString();
}

QString DialogOpen::query() const
{
    return QString();
}

QStringList DialogOpen::axes() const
{
    return QStringList();
}

int DialogOpen::dec() const
{
    return 0;
}
