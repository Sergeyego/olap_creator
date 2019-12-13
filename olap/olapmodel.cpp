#include "olapmodel.h"

OlapModel::OlapModel(QStringList axes, int dec, QObject *parent) :
    QAbstractTableModel(parent)
{
    decimal=dec;
    tSum = new Sum(tr("Сумма"),this);
    tAvg = new Avg(tr("Среднее"),this);
    tMin = new Min(tr("Минимум"),this);
    tMax = new Max(tr("Максимум"),this);
    pTtl=tSum;
    hCube = new hyper_cube;
    n=axes.size();
    for(int i=0; i<n; i++){
        m_axe[axes[i]]=i;
    }
}

OlapModel::~OlapModel()
{
    delete hCube;
}

int OlapModel::rowCount(const QModelIndex& /*parent*/) const
{
    return (xJ.size() && yI.size())? hCube->Ny+xJ.size() : 0;
}

int OlapModel::columnCount(const QModelIndex& /*parent*/) const
{
    return (yI.size() && xJ.size())? hCube->Nx+yI.size() : 0;
}

QVariant OlapModel::data(const QModelIndex &index, int role) const
{
    if (role==Qt::DisplayRole){
        double val=hCube->dp[index.row()][index.column()];
        return val!=0 ? QLocale().toString(hCube->dp[index.row()][index.column()],'f',decimal) : QString("");
    } else if (role==Qt::EditRole){
        return hCube->dp[index.row()][index.column()];
    } else if (role==Qt::TextAlignmentRole){
        return int(Qt::AlignRight | Qt::AlignVCenter);
    }
    return QVariant();
}

QVariant OlapModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QString ms;
    if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        if (section<hCube->pY->size()){
            ms=hCube->pY->key(section);
            ms.chop(1);
            ms=ms.replace(QChar('\n'),QChar(' '));
        } else if (section==hCube->pY->size()) ms=pTtl->getName();
        return ms;
    }
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section<hCube->pX->size()){
            ms=hCube->pX->key(section);
            ms.chop(1);
        } else if (section==hCube->pX->size()) ms=pTtl->getName();
        return ms;
    }
    return QAbstractTableModel::headerData(section,orientation,role);
}

void OlapModel::setQuery(QString query)
{
    QSqlQuery qu;
    s_keys vk(n);
    hCube->setN(n);
    qu.prepare(query);
    if(qu.exec()){
        while (qu.next()) {
            for(int i=0; i<n; i++){
                vk[i] = qu.value(i).toString() + '\n';
            }
            hCube->add(vk, qu.value(n).toDouble());
        }
    } else {
        QMessageBox::critical(NULL,"Error",qu.lastError().text(),QMessageBox::Ok);
    }
    refresh();
}

void OlapModel::setX(QStringList x)
{
    i_dims old=xJ;
    xJ.clear();
    for (int i=0; i<x.size(); ++i){
        xJ.push_back(m_axe[x.at(i)]);
    }
    if (xJ!=old) refresh();
}

void OlapModel::setY(QStringList y)
{
    i_dims old=yI;
    yI.clear();
    for (int i=0; i<y.size(); ++i){
        yI.push_back(m_axe[y.at(i)]);
    }
    if (yI!=old) refresh();
}

void OlapModel::refresh()
{
    beginResetModel();
    if( !(yI.empty()) && !(xJ.empty()) ) {
        hCube->make_proj(yI,xJ,pTtl);
    }
    endResetModel();
    emit headerDataChanged(Qt::Vertical,0,rowCount()-1);
    emit sigRefresh();
}

void OlapModel::setTypeSum(bool b)
{
    if (b){
        pTtl=tSum;
        refresh();
    }
}

void OlapModel::setTypeAvg(bool b)
{
    if (b){
        pTtl=tAvg;
        refresh();
    }
}

void OlapModel::setTypeMax(bool b)
{
    if (b){
        pTtl=tMax;
        refresh();
    }
}

void OlapModel::setTypeMin(bool b)
{
    if (b){
        pTtl=tMin;
        refresh();
    }
}
