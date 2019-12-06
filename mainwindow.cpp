#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->actionNew->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon)));
    ui->mainToolBar->addAction(ui->actionNew);

    ui->actionOpen->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogOpenButton)));
    ui->mainToolBar->addAction(ui->actionOpen);

    ui->actionSave->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton)));
    ui->mainToolBar->addAction(ui->actionSave);

    ui->actionProfr->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowRight)));

    ui->actionQuery->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_CommandLink)));
    ui->mainToolBar->addAction(ui->actionQuery);

    ui->actionReport->setIcon(QIcon(":images/icon.ico"));
    ui->mainToolBar->addAction(ui->actionReport);

    connect(ui->actionNew,SIGNAL(triggered(bool)),this,SLOT(newReport()));
    connect(ui->actionOpen,SIGNAL(triggered(bool)),this,SLOT(open()));
    connect(ui->actionSave,SIGNAL(triggered(bool)),this,SLOT(save()));
    connect(ui->actionQuery,SIGNAL(triggered(bool)),this,SLOT(goQuery()));
    connect(ui->actionProfr,SIGNAL(triggered(bool)),this,SLOT(importProfr()));
    connect(ui->actionReport,SIGNAL(triggered(bool)),this,SLOT(goCube()));

    connect(ui->mdiArea,SIGNAL(subWindowActivated(QMdiSubWindow*)),this,SLOT(mdiSubActivated(QMdiSubWindow*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

FormEdtOlap *MainWindow::activeMdiChild() const
{
    if (QMdiSubWindow *activeSubWindow = ui->mdiArea->activeSubWindow()){
        return qobject_cast<FormEdtOlap *>(activeSubWindow->widget());
    }
    return 0;
}

FormEdtOlap *MainWindow::newFormEdtOlap()
{
    FormEdtOlap *fe = new FormEdtOlap;
    fe->setAttribute(Qt::WA_DeleteOnClose);
    ui->mdiArea->addSubWindow(fe);
    connect(fe,SIGNAL(saveEnableChanged(bool)),this,SLOT(saveEnable(bool)));
    fe->show();
    return fe;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    ui->mdiArea->closeAllSubWindows();
    if (ui->mdiArea->currentSubWindow()) {
        event->ignore();
    } else {
        event->accept();
    }
}

void MainWindow::newReport()
{
    newFormEdtOlap();
}

void MainWindow::goQuery()
{
    FormEdtOlap *ol = activeMdiChild();
    if (ol){
        ol->goQuery();
    }
}

void MainWindow::mdiSubActivated(QMdiSubWindow */*w*/)
{
    bool active=false;
    FormEdtOlap *ol = activeMdiChild();
    if (ol){
        active=true;
        ui->actionSave->setEnabled(ol->saveEnable());
    } else {
        ui->actionSave->setEnabled(false);
    }
    ui->actionReport->setEnabled(active);
    ui->actionQuery->setEnabled(active);
}

void MainWindow::importProfr()
{
    QSettings settings("szsm", QApplication::applicationName());
    QDir dir(settings.value("openPath",QDir::homePath()).toString());
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open profr"),dir.path(), tr("Profr Files (*.txt)"));
    if (!fileName.isEmpty()){
        QStringList axes;
        QString head, query;
        int dec=1;
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QFileInfo info(file);
            settings.setValue("openPath",info.path());

            QTextStream in(&file);
            in.setCodec(QTextCodec::codecForName("UTF-8"));

            enum stat {UNC=1, HEAD, CONN, AXES, QUERY, DEC} st;
            st=stat::UNC;

            while(!in.atEnd()){
                QString str=in.readLine();
                if (str=="<h>"){
                    st=stat::HEAD;
                } else if (str=="<c>"){
                    st=stat::CONN;
                } else if (str=="<a>"){
                    st=stat::AXES;
                } else if (str=="<q>"){
                    st=stat::QUERY;
                } else if (str=="<p>"){
                    st=stat::DEC;
                }

                if (st==stat::HEAD && str!="<h>" && str!="</h>"){
                    head.append(str);
                }
                if (st==stat::AXES && str!="<a>" && str!="</a>"){
                    axes.push_back(str);
                }
                if (st==stat::QUERY && str!="<q>" && str!="</q>"){
                    query.append(str+"\n");
                }
                if (st==stat::DEC && str!="<p>" && str!="</p>"){
                    dec=str.toInt();
                }
            }
            file.close();

            FormEdtOlap *fe = newFormEdtOlap();
            fe->setData(head,axes,query,dec);
        } else {
            QMessageBox::critical(this,QObject::tr("Error"),QObject::tr("Ошибка открытия файла"),QMessageBox::Ok);
        }
    }
}

void MainWindow::goCube()
{
    FormEdtOlap *ol = activeMdiChild();
    if (ol){
        if (!ol->query().isEmpty()){
            CubeWidget *w = new CubeWidget(ol->title(),ol->axes(),ol->query(),ol->dec());
            w->setAttribute(Qt::WA_DeleteOnClose);
            w->show();
        }
    }
}

void MainWindow::save()
{
    FormEdtOlap *ol = activeMdiChild();
    if (ol){
        ol->save();
    }
}

void MainWindow::open()
{
    DialogOpen d;
    if (d.exec()==QDialog::Accepted){
        FormEdtOlap *fe = newFormEdtOlap();
        fe->setData(d.title(),d.axes(),d.query(),d.dec(),d.id());
    }
}

void MainWindow::saveEnable(bool b)
{
    if (this->sender()==activeMdiChild()){
        ui->actionSave->setEnabled(b);
    }
}


