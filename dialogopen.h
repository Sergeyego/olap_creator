#ifndef DIALOGOPEN_H
#define DIALOGOPEN_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>

namespace Ui {
class DialogOpen;
}

class DialogOpen : public QDialog
{
    Q_OBJECT

public:
    explicit DialogOpen(QWidget *parent = 0);
    ~DialogOpen();
    int id() const;
    QString title() const;
    QString query() const;
    QStringList axes() const;
    int dec() const;

private:
    Ui::DialogOpen *ui;
    QSqlQueryModel *model;
    QVariant currentData(int col) const;
};

#endif // DIALOGOPEN_H
