#ifndef CITEMATTRIBUTESETTING_H
#define CITEMATTRIBUTESETTING_H

#include <QWidget>
#include "BaseItem.h"
#include "ModuleBcu.h"
#include "ModuleRcu.h"

class RcaGraphScene;
class QListWidgetItem;
class QComboBox;
class CWidget;
class QLabel;

namespace Ui {
class CItemAttributeSetting;
}

class CItemAttributeSetting : public QWidget
{
    Q_OBJECT

public:
    explicit CItemAttributeSetting(RcaGraphScene* scene, QWidget *parent = 0);
    ~CItemAttributeSetting();

    void updateItemAttribute(BaseItem *baseItem);

private:
    Ui::CItemAttributeSetting *ui;
    CWidget *m_widget = nullptr;
    QLabel *m_label = nullptr;

    RcaGraphScene* m_curRcaScene = nullptr;
    BaseItem* m_curBaseItem = nullptr;

    void disconnectAllChild();
    void connectAllChild();

    void setBypassBox(BfuFuncIndex funcIndex);
    void setFuncMode(BfuFuncIndex funcIndex, ElementBfu *elementBfu = nullptr);
    void setBypassExp(int bypassIndex, ElementBfu *elementBfu = nullptr);
    void setComboBoxBcu(QComboBox *box, InputPortType type);
    void setComboBoxBfu(QComboBox *box, InputPortType type);
    void setComboBoxBfuX(QComboBox *box);
    void setComboBoxBfuY(QComboBox *box);
    void setComboBoxSbox(QComboBox *box);
    void setComboBoxBenes(QComboBox *box);
    void setComboBoxMem(QComboBox *box);
    void setComboBoxFifo(QComboBox *box);

private slots:
    void saveAttribute();
    void reloadCurItemAttr();
    void showResourceWidget();
    void showExplorerWidget();
    void on_comboBox_bfu_operator_currentIndexChanged(int index);
    void on_comboBox_bfu_bypass_currentIndexChanged(int index);
    void on_comboBox_sbox_input0Type_currentIndexChanged(int index);
    void on_comboBox_bfu_inputAType_currentIndexChanged(int index);
    void on_comboBox_bfu_inputBType_currentIndexChanged(int index);
    void on_comboBox_bfu_inputTType_currentIndexChanged(int index);
    void on_comboBox_bcu_inputType0_currentIndexChanged(int index);
    void on_comboBox_bcu_inputType1_currentIndexChanged(int index);
    void on_comboBox_bcu_inputType2_currentIndexChanged(int index);
    void on_comboBox_bcu_inputType3_currentIndexChanged(int index);
    void on_comboBox_benes_input0Type_currentIndexChanged(int index);
    void on_comboBox_benes_input1Type_currentIndexChanged(int index);
    void on_comboBox_benes_input2Type_currentIndexChanged(int index);
    void on_comboBox_benes_input3Type_currentIndexChanged(int index);
    void on_comboBox_rcu_read_mode_currentIndexChanged(int index);
    void on_comboBox_rcu_write_mode_currentIndexChanged(int index);
    void on_pushButton_help_clicked();
};

#endif // CITEMATTRIBUTESETTING_H
