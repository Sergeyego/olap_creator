#ifndef FORMEDTOLAP_H
#define FORMEDTOLAP_H

#include <QWidget>
#include <QDebug>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>

namespace Ui {
class FormEdtOlap;
}

class FormEdtOlap : public QWidget
{
    Q_OBJECT

public:
    explicit FormEdtOlap(QWidget *parent = 0);
    ~FormEdtOlap();
    void setData(const QString &title, const QStringList &axes, const QString &query, int dec, int id=-1);
    QString title() const;
    QStringList axes() const;
    QString strAxes() const;
    QString query() const;
    int dec() const;
    bool exist(int id);
    bool saveEnable();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::FormEdtOlap *ui;
    QSqlQueryModel *model;
    QString getStr(int d);
    void setErrText(QString text, bool err=true);
    bool changed;

private slots:
    void setColumnCount(int i);
    void setChanged();

public slots:
    void goQuery();
    bool save();

signals:
    void saveEnableChanged(bool en);
};

#endif // FORMEDTOLAP_H
