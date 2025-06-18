#include "dialogopen.h"
#include "ui_dialogopen.h"

DialogOpen::DialogOpen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOpen)
{
    ui->setupUi(this);

    model = new QSqlQueryModel(this);
    model->setQuery("select id, nam, array_to_json(columns), query, dc from olaps order by nam");
    if (model->lastError().isValid()){
        QMessageBox::critical(this,QObject::tr("Error"),model->lastError().text(),QMessageBox::Ok);
    } else {
        ui->tableView->setModel(model);
        model->setHeaderData(0,Qt::Horizontal,QString("Id"));
        model->setHeaderData(1,Qt::Horizontal,QString("Название"));
        for (int i=2; i<model->columnCount();i++){
            ui->tableView->setColumnHidden(i,true);
        }
        ui->tableView->resizeColumnsToContents();
    }
    connect(ui->tableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(accept()));
}

DialogOpen::~DialogOpen()
{
    delete ui;
}

int DialogOpen::id() const
{
    return currentData(0).toInt();
}

QString DialogOpen::title() const
{
    return currentData(1).toString();
}

QString DialogOpen::query() const
{
    return currentData(3).toString();
}

QStringList DialogOpen::axes() const
{
    QStringList axes;
    QVariantList list;
    QJsonDocument obj = QJsonDocument::fromJson(currentData(2).toByteArray());
    if (obj.isArray()){
        list = obj.array().toVariantList();
    }
    for (QVariant v : list){
        axes.push_back(v.toString());
    }

    return axes;
}

int DialogOpen::dec() const
{
    return currentData(4).toInt();
}

QVariant DialogOpen::currentData(int col) const
{
    QModelIndex ind=ui->tableView->currentIndex();
    return ind.isValid()? ui->tableView->model()->data(ui->tableView->model()->index(ind.row(),col),Qt::EditRole) : QVariant();
}
