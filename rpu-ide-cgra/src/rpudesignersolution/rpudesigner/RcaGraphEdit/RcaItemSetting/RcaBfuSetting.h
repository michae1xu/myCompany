#ifndef RCABFUSETTING_H
#define RCABFUSETTING_H

#include <QDialog>
#include <QAbstractButton>
#include <QComboBox>

#include "RcaGraphScene.h"

namespace Ui {
class RcaBfuSetting;
}

class RcaBfuSetting : public QDialog
{
    Q_OBJECT

public:
    explicit RcaBfuSetting(QWidget *parent = 0);
    ~RcaBfuSetting();
    static RcaBfuSetting* instance(QWidget *parent = 0) {
        if ( !bfuSetingInstance ) {
            bfuSetingInstance = new RcaBfuSetting(parent);
        }
        return bfuSetingInstance;
    }
//    void setRcaScene(RcaGraphScene* scene);
    void showBfu(ElementBfu* bfu,QPoint pos);
    BaseItem* getItem() { return elementBfu;}
private slots:
    void handleBfuBeDeleted(BaseItem* deletedItem);

    void on_comboBox_inputAType_currentIndexChanged(int index);
    void on_comboBox_inputBType_currentIndexChanged(int index);
    void on_comboBox_inputTType_currentIndexChanged(int index);

    void on_buttonBox_clicked(QAbstractButton *button);

    void on_comboBox_operator_currentIndexChanged(int index);

    void on_comboBox_bypass_currentIndexChanged(int index);

private:
    void setBypassBox(BfuFuncIndex funcIndex);
    void setFuncMode(BfuFuncIndex funcIndex);
    void setBypassExp(int bypassIndex);
    void setComboBox(QComboBox* box,InputPortType type);
    void setComboBoxBfuX(QComboBox* box);
    void setComboBoxBfuY(QComboBox* box);
    void setComboBoxSbox(QComboBox* box);
    void setComboBoxBenes(QComboBox* box);
    void setComboBoxMem(QComboBox* box);
    void setComboBoxFifo(QComboBox* box);

private:
    Ui::RcaBfuSetting *ui;
    static RcaBfuSetting* bfuSetingInstance;
//    RcaGraphScene* rcaScene;
    ElementBfu* elementBfu;
};

#endif // RCABFUSETTING_H
