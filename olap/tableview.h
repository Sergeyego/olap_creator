#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QWidget>
#include <QTableView>
#include <QHeaderView>
#include <xlslib.h>
#include <QDir>
#include <QFileDialog>
#include <QSettings>
#include <QApplication>
#include <QDebug>

using namespace xlslib_core;

class TableView : public QTableView
{
        Q_OBJECT
public:

    TableView(QWidget *parent=0);

public slots:
    void resizeToContents();
    void save(QString fnam);
};

#endif // TABLEVIEW_H
