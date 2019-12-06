#include "tableview.h"

TableView::TableView(QWidget *parent) : QTableView(parent)
{
    verticalHeader()->setDefaultSectionSize(verticalHeader()->fontMetrics().height()*1.5);
}

void TableView::resizeToContents()
{
    if (!model()) return;
    int n=model()->columnCount();
    int m=model()->rowCount();
    int max=0;
    QStringList l;
    QString s;
    /*for (int i=0; i<model()->rowCount(); i++){
        max=1;
        for (int j=0; j<n; j++){
            s=model()->data(model()->index(i,j)).toString();
            l=s.split("\n");
            if (max<l.size()) max=l.size();
        }
        setRowHeight(i,max*ui->tableView->fontMetrics().height()+12);
    }*/
    for (int i=0; i<n; i++){
        s=model()->headerData(i,Qt::Horizontal).toString();
        l=s.split("\n");
        max=0;
        for (int k=0; k<l.size(); k++){
            if (max<fontMetrics().width(l.at(k)))
                max=fontMetrics().width(l.at(k));
        }
        for (int j=0; j<m; j++){
            s=model()->data(model()->index(j,i)).toString();
            l=s.split("\n");
            for (int k=0; k<l.size(); k++){
                if (max<fontMetrics().width(l.at(k)))
                    max=fontMetrics().width(l.at(k));
            }
        }
        setColumnWidth(i,max+12);
    }
}

void TableView::save(QString fnam)
{
    if (!model()) return;
    int rows,cols;
    int i,j;
    rows=this->model()->rowCount();
    cols=this->model()->columnCount();

    if (rows*cols>1){
        workbook wb;
        worksheet *sh = wb.sheet("sheet");
        cell_t *cref;

        xf_t * fmt = wb.xformat();
        fmt->SetBorderStyle(BORDER_LEFT, BORDER_THIN);
        fmt->SetBorderStyle(BORDER_RIGHT, BORDER_THIN);
        fmt->SetBorderStyle(BORDER_TOP, BORDER_THIN);
        fmt->SetBorderStyle(BORDER_BOTTOM, BORDER_THIN);

        QString hCubeell;
        sh->label(0,0,fnam.toStdWString());
        cref=sh->FindCellOrMakeBlank(0,0);
        cref->fontbold(BOLDNESS_DOUBLE);
        sh->rowheight(0,300);

        sh->rowheight(1,(this->horizontalHeader()->sizeHint().height())*30);

        sh->colwidth(0,(this->verticalHeader()->sizeHint().width())*38);

        sh->blank(1,0,fmt);

        for(i=0;i<cols;i++){
            hCubeell=this->model()->headerData(i,Qt::Horizontal).toString();
            hCubeell.replace(QChar('\n'),QChar('\n'));
            sh->label(1,i+1,hCubeell.toStdWString(),fmt);
            cref=sh->FindCellOrMakeBlank(1,i+1);
            cref->fontbold(BOLDNESS_DOUBLE);
            cref->halign(HALIGN_JUSTIFY);
            sh->colwidth(i+1,(this->columnWidth(i)*38));
        }

        for(j=0;j<rows;j++){
            sh->label(j+2,0,this->model()->headerData(j,Qt::Vertical).toString().toStdWString(),fmt);
            cref=sh->FindCellOrMakeBlank(j+2,0);
            cref->halign(HALIGN_LEFT);
        }

        for (i=0;i<rows;i++)
            for(j=0;j<cols;j++){
                xf_t * pxf = wb.xformat();
                pxf->SetBorderStyle(BORDER_LEFT, BORDER_THIN);
                pxf->SetBorderStyle(BORDER_RIGHT, BORDER_THIN);
                pxf->SetBorderStyle(BORDER_TOP, BORDER_THIN);
                pxf->SetBorderStyle(BORDER_BOTTOM, BORDER_THIN);
                QVariant vdisp=this->model()->data(this->model()->index(i,j),Qt::DisplayRole);
                QVariant vedt=this->model()->data(this->model()->index(i,j),Qt::EditRole);

                QString tname=vedt.typeName();

                if (tname==QString("int")){
                    pxf->SetFormat(FMT_NUMBER3);
                } else if (tname==QString("double")){
                    pxf->SetFormat(FMT_NUMBER4);
                } else {
                    pxf->SetFormat(FMT_GENERAL);
                }
                if (!vdisp.toString().isEmpty()){
                    if (tname==QString("double") || tname==QString("int")){
                        sh->number(i+2,j+1,vedt.toDouble(),pxf);
                    } else {
                        sh->label(i+2,j+1,vdisp.toString().toStdWString(),pxf);
                    }
                } else {
                    sh->blank(i+2,j+1,pxf);
                }
            }

        QSettings settings("szsm", QApplication::applicationName());
        QDir dir(settings.value("savePath",QDir::homePath()).toString());
        QString filename = QFileDialog::getSaveFileName(this,tr("Сохранить документ"),
                                                        dir.path()+"/"+fnam+".xls",
                                                        tr("Documents (*.xls)") );
        if (!filename.isEmpty()){
            QFile file(filename);
            if (file.exists()) file.remove();
            std::string fil(filename.toLocal8Bit());
            wb.Dump(fil);
            QFileInfo info(file);
            settings.setValue("savePath",info.path());
        }
    }
}
