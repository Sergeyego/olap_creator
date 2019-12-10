#include "formedtolap.h"
#include "ui_formedtolap.h"

FormEdtOlap::FormEdtOlap(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormEdtOlap)
{
    ui->setupUi(this);
    changed=false;

    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    ui->tableWidgetColumns->setColumnCount(1);
    ui->tableWidgetColumns->setRowCount(ui->spinBoxCount->value());
    ui->tableWidgetColumns->verticalHeader()->setDefaultSectionSize(ui->tableWidgetColumns->verticalHeader()->fontMetrics().height()*1.5);
    ui->tableWidgetColumns->setColumnWidth(0,150);
    ui->tableWidgetColumns->setHorizontalHeaderItem(0,new QTableWidgetItem(QString("Название осей")));

    model = new QSqlQueryModel(this);
    ui->tableViewQuery->setModel(model);

    connect(ui->spinBoxCount,SIGNAL(valueChanged(int)),this,SLOT(setColumnCount(int)));

    connect(ui->lineEditId,SIGNAL(textChanged(QString)),this,SLOT(setChanged()));
    connect(ui->lineEditName,SIGNAL(textChanged(QString)),this,SLOT(setChanged()));
    connect(ui->spinBoxDec,SIGNAL(valueChanged(int)),this,SLOT(setChanged()));
    connect(ui->plainTextEditQury,SIGNAL(textChanged()),this,SLOT(setChanged()));
    connect(ui->tableWidgetColumns,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(setChanged()));
    connect(ui->spinBoxCount,SIGNAL(valueChanged(int)),this,SLOT(setChanged()));
}

FormEdtOlap::~FormEdtOlap()
{
    delete ui;
}

void FormEdtOlap::setData(const QString &title, const QStringList &axes, const QString &query, int dec, int id)
{
    ui->lineEditName->setText(title);
    if (axes.size()){
        ui->spinBoxCount->setValue(axes.size());
        ui->tableWidgetColumns->clear();
        ui->tableWidgetColumns->setColumnCount(1);
        ui->tableWidgetColumns->setRowCount(axes.size());
        for (int i=0; i<axes.size(); i++){
            ui->tableWidgetColumns->setItem(i,0,new QTableWidgetItem(axes.at(i)));
        }
    }
    ui->tableWidgetColumns->setHorizontalHeaderItem(0,new QTableWidgetItem(QString("Название осей")));
    ui->plainTextEditQury->document()->setPlainText(query);
    ui->spinBoxDec->setValue(dec);
    if (id>=0){
        ui->lineEditId->setText(QString::number(id));
    }
    this->setWindowTitle(title.isEmpty()? QString("New OLAP") : title);
    changed=false;
}

QString FormEdtOlap::title() const
{
    return ui->lineEditName->text();
}

QStringList FormEdtOlap::axes() const
{
    QStringList a;
    for (int i=0; i<ui->tableWidgetColumns->rowCount(); i++){
        QTableWidgetItem *it =ui->tableWidgetColumns->item(i,0);
        if (it){
            a.push_back(it->data(Qt::DisplayRole).toString());
        }
    }
    return a;
}

QString FormEdtOlap::strAxes() const
{
    QString s;
    for (int i=0; i<ui->tableWidgetColumns->rowCount(); i++){
        QTableWidgetItem *it =ui->tableWidgetColumns->item(i,0);
        if (it){
            if (!s.isEmpty()){
                s+=", ";
            }
            s+=it->data(Qt::DisplayRole).toString();
        }
    }
    s.prepend("{");
    s.append("}");
    return s;
}

QString FormEdtOlap::query() const
{
    return ui->plainTextEditQury->document()->toPlainText();
}

int FormEdtOlap::dec() const
{
    return ui->spinBoxDec->text().toInt();
}

bool FormEdtOlap::exist(int id)
{
    bool b=false;
    QSqlQuery qu;
    qu.prepare("select count(*) from olaps where id=:id");
    qu.bindValue(":id",id);
    if (qu.exec()){
        while (qu.next()){
            b=qu.value(0).toInt()>0;
        }
    }
    return b;
}

bool FormEdtOlap::saveEnable()
{
    return changed;
}

void FormEdtOlap::closeEvent(QCloseEvent *event)
{
    if (saveEnable()){
        int n=QMessageBox::question(this,QString("Есть несохраненные изменения"),this->windowTitle()+QString(".\n Сохранить изменения?"),QMessageBox::Ok,QMessageBox::No);
        if (n==QMessageBox::Ok){
            save();
            event->ignore();
        } else {
            event->accept();
        }
    } else {
        event->accept();
    }
}


QString FormEdtOlap::getStr(int d)
{
    int q= d % 10;
    int h= d % 100;
    QString ok;
    if (h<10 || h>19){
        if (q==1){
            ok=QString("у");
        } else if (q==2 || q==3 || q==4){
            ok=QString("и");
        }
    }
    return QLocale().toString(d)+QString(" строк")+ok;
}

void FormEdtOlap::setErrText(QString text, bool err)
{
    QPalette pal=ui->plainTextEditError->palette();
    if (err){
        ui->tabWidget->setCurrentIndex(1);
        pal.setColor(QPalette::Text,Qt::red);
    } else {
        pal.setColor(QPalette::Text,QColor(40,100,40));
    }
    ui->plainTextEditError->setPalette(pal);
    ui->plainTextEditError->document()->setPlainText(text);
}

void FormEdtOlap::setColumnCount(int i)
{
    ui->tableWidgetColumns->setRowCount(i);
}

void FormEdtOlap::setChanged()
{
    if (!changed){
        this->setWindowTitle("*"+this->windowTitle());
        changed=true;
        emit saveEnableChanged(changed);
    }
}

void FormEdtOlap::goQuery()
{
    ui->labelStatus->clear();
    ui->plainTextEditError->clear();
    QString qu=query();
    if (qu.isEmpty()){
        setErrText("Пустой запрос!");
        return;
    }

    QDateTime dt1=QDateTime::currentDateTime();
    qu.replace(":d1","'"+ui->dateEditBeg->date().toString("yyyy-MM-dd")+"'");
    qu.replace(":d2","'"+ui->dateEditEnd->date().toString("yyyy-MM-dd")+"'");

    model->setQuery(qu);
    qint64 querytime=dt1.msecsTo(QDateTime::currentDateTime());
    if (model->lastError().isValid()){
        setErrText(model->lastError().text(),true);
        ui->labelStatus->setText(QString("Выполнение запроса завершилось ошибкой"));
    } else {
        QString stat=QString("Время выполнения запроса %1 ms. Запрос вернул %2.").arg(QLocale().toString(querytime)).arg(getStr(model->rowCount()));
        ui->labelStatus->setText(stat);
        ui->tabWidget->setCurrentIndex(0);
        setErrText(stat,false);
    }
}

bool FormEdtOlap::save()
{
    bool saved=false;
    if (ui->lineEditId->text().isEmpty()){
        QSqlQuery qu;
        qu.prepare("insert into olaps (nam, columns, query, dc) values (:nam, :columns, :query, :dc) returning id");
        qu.bindValue(":nam",title());
        qu.bindValue(":columns",strAxes());
        qu.bindValue(":query",query());
        qu.bindValue(":dc",dec());
        if (!qu.exec()){
            QMessageBox::critical(this,QObject::tr("Error"),qu.lastError().text(),QMessageBox::Ok);
        } else {
            while (qu.next()){
                ui->lineEditId->setText(qu.value(0).toString());
            }
            saved=true;
        }
    } else {
        bool ok;
        int id=ui->lineEditId->text().toInt(&ok);
        if (ok){
            if (exist(id)){
                int n= QMessageBox::question(this,QString("Подтвердите действие"),QString("Перезаписать отчет с идентификатором %1?").arg(id),QMessageBox::Ok,QMessageBox::No);
                if (n==QMessageBox::Ok){
                    QSqlQuery qu;
                    qu.prepare("update olaps set nam=:nam, columns=:columns, query=:query, dc=:dc where id=:id");
                    qu.bindValue(":nam",title());
                    qu.bindValue(":columns",strAxes());
                    qu.bindValue(":query",query());
                    qu.bindValue(":dc",dec());
                    qu.bindValue(":id",id);
                    if (!qu.exec()){
                        QMessageBox::critical(this,QObject::tr("Error"),qu.lastError().text(),QMessageBox::Ok);
                    } else {
                        saved=true;
                    }
                }
            } else {
                QSqlQuery qu;
                qu.prepare("insert into olaps (id, nam, columns, query, dc) values (:id, :nam, :columns, :query, :dc)");
                qu.bindValue(":id",id);
                qu.bindValue(":nam",title());
                qu.bindValue(":columns",strAxes());
                qu.bindValue(":query",query());
                qu.bindValue(":dc",dec());
                if (!qu.exec()){
                    QMessageBox::critical(this,QObject::tr("Error"),qu.lastError().text(),QMessageBox::Ok);
                } else {
                    saved=true;
                }
            }
        } else {
            QMessageBox::critical(this,QObject::tr("Error"),QString("Неверный идентификатор: ")+ui->lineEditId->text(),QMessageBox::Ok);
        }
    }
    if (saved){
        this->setWindowTitle(title());
        changed=false;
        emit saveEnableChanged(changed);
    }
    return saved;
}
