#ifndef DEBUGMODIFYDATA_H
#define DEBUGMODIFYDATA_H

#include <QAbstractButton>
#include <QDialog>
#include "ElementPort.h"

namespace Ui {
class DebugModifyData;
}

class DebugModifyData : public QDialog
{
    Q_OBJECT

public:
    explicit DebugModifyData(ElementTypeId_del typeId,QStringList outputList,QWidget *parent = 0);
    ~DebugModifyData();

    QStringList getOutputStrList() const;

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::DebugModifyData *ui;
    int outputCount;
    QStringList outputStrList;

private:
    void initEdit(ElementTypeId_del typeId);
    QString str2ItemData(QString str);
};

#endif // DEBUGMODIFYDATA_H
