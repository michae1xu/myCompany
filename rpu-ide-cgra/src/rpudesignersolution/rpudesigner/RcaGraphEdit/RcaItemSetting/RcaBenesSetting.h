#ifndef RCABENESSETTING_H
#define RCABENESSETTING_H

class QListWidgetItem;

#include <QDialog>
#include <QAbstractButton>
#include <QComboBox>

#include "RcaGraphScene.h"

namespace Ui {
class RcaBenesSetting;
}

class RcaBenesSetting : public QDialog
{
    Q_OBJECT

public:
    explicit RcaBenesSetting(QWidget *parent = 0);
    ~RcaBenesSetting();
    static RcaBenesSetting* instance(QWidget *parent = 0) {
        if ( !benesSetingInstance ) {
            benesSetingInstance = new RcaBenesSetting(parent);
        }
        return benesSetingInstance;
    }
    void setRcaScene(RcaGraphScene* scene);
    void showBenes(ElementBenes* benes,QPoint pos);
    void resetResourceMap(RcaGraphScene* scene);
    BaseItem* getItem() { return elementBenes;}
private slots:
    void handleBenesBeDeleted(BaseItem* deletedItem);

    void on_pushButton_explorer_clicked();

    void on_comboBox_input0Type_currentIndexChanged(int index);
    void on_comboBox_input1Type_currentIndexChanged(int index);
    void on_comboBox_input2Type_currentIndexChanged(int index);
    void on_comboBox_input3Type_currentIndexChanged(int index);

    void on_buttonBox_clicked(QAbstractButton *button);

    void on_lineEdit_textChanged(const QString &arg1);

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    void setComboBox(QComboBox* box,InputPortType type);
    void setComboBoxBfuX(QComboBox* box);
    void setComboBoxBfuY(QComboBox* box);
    void setComboBoxSbox(QComboBox* box);
    void setComboBoxBenes(QComboBox* box);
    void setComboBoxMem(QComboBox* box);
    void setComboBoxFifo(QComboBox* box);
private:
    Ui::RcaBenesSetting *ui;
    static RcaBenesSetting* benesSetingInstance;
    RcaGraphScene* rcaScene;
    ElementBenes* elementBenes;
    bool isTrue;
};

#endif // RCABENESSETTING_H
