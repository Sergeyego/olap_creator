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
    QString a=currentData(2).toString();
    a=a.replace('{',"");

    QStringList list;
    int pos=0;
    QRegExp ex1("^\"([^\"].*[^\\\\])[\"][,}]");
    ex1.setMinimal(true);
    QRegExp ex2("^([^\"].*)[,}]");
    ex2.setMinimal(true);

    while (ex1.indexIn(a)!=-1 || ex2.indexIn(a)!=-1){
        if (ex1.indexIn(a)!=-1){
            list << ex1.cap(1);
            pos=ex1.indexIn(a)+ex1.cap(1).size()+2;
        } else if (ex2.indexIn(a)!=-1){
            list << ex2.cap(1);
            pos=ex2.indexIn(a)+ex2.cap(1).size();
        } else {
            pos=-1;
        }
        a=a.mid(pos+1);
    }

    return list;
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
