#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStyle>
#include "formedtolap.h"
#include <QMdiSubWindow>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QSettings>
#include <QCloseEvent>
#include "olap/cubewidget.h"
#include "dialogopen.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    FormEdtOlap *activeMdiChild() const;
    FormEdtOlap *newFormEdtOlap();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newReport();
    void goQuery();
    void mdiSubActivated(QMdiSubWindow *w);
    void importProfr();
    void goCube();
    void save();
    void open();
    void saveEnable(bool b);
};

#endif // MAINWINDOW_H
