#ifndef ITEMDATAFORM_H
#define ITEMDATAFORM_H

#include <QDialog>
#include <QAbstractButton>
#include "cfgdebuglog/DebugLogItem.h"

class QAbstractButton;

namespace Ui {
class ItemDataForm;
}

class ItemDataForm : public QDialog
{
    Q_OBJECT

public:
    explicit ItemDataForm(ElementTypeId typeId,QStringList inputList,QStringList outputList,QWidget *parent = 0);
    ~ItemDataForm();
    QStringList getInputStrList();
    QStringList getOutputStrList();


private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::ItemDataForm *ui;
    int inputCount;
    int outputCount;
    QStringList inputStrList;
    QStringList outputStrList;
private:
    void initEdit(ElementTypeId typeId);
    QString str2ItemData(QString str);
};

#endif // ITEMDATAFORM_H
