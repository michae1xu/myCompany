#ifndef RESULTFORM_H
#define RESULTFORM_H

class QTreeWidgetItem;

#include "datafile.h"
#include <QDialog>
#include <QFile>

namespace Ui {
class ResultForm;
}

class ResultForm : public QDialog
{
    Q_OBJECT

public:
    explicit ResultForm(QList<SXmlParameter> *xmlParameterList, QStringList outfifoList, QStringList memoryList, QWidget *parent = 0);
    ~ResultForm();

private slots:
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void save();

private:
    Ui::ResultForm *ui;

//    QFile m_file;

    QList<SXmlParameter> *m_xmlParameterList;

    QStringList m_outfifoList;
    QStringList m_memoryList;

//    QString stpToNm(const QString &fullFileName);

    void showText(int row, int index);
    void showFifo(int row, int index);
    void showMem(int row, int index);


};

#endif // RESULTFORM_H
