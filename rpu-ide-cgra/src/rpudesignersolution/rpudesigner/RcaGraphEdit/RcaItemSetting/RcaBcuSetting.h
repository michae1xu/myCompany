#ifndef RCABCUSETTING_H
#define RCABCUSETTING_H

#include <QDialog>
#include <QAbstractButton>
#include <QComboBox>

#include "RcaGraphScene.h"

namespace Ui {
class RcaBcuSetting;
}

class RcaBcuSetting : public QDialog
{
    Q_OBJECT

public:
    explicit RcaBcuSetting(QWidget *parent = 0);
    ~RcaBcuSetting();
    static RcaBcuSetting* instance(QWidget *parent = 0) {
        if ( !bcuSetingInstance ) {
            bcuSetingInstance = new RcaBcuSetting(parent);
        }
        return bcuSetingInstance;
    }
    void setRcaScene(RcaGraphScene* scene);
    void showBcu(ModuleBcu* bcu,QPoint pos);
    BaseItem* getItem() { return moduleBcu;}
private slots:
    void on_buttonBox_clicked(QAbstractButton *button);
    void on_comboBox_inputType0_currentIndexChanged(int index);
    void on_comboBox_inputType1_currentIndexChanged(int index);
    void on_comboBox_inputType2_currentIndexChanged(int index);
    void on_comboBox_inputType3_currentIndexChanged(int index);

    void handleBcuBeDeleted(BaseItem* deletedItem);

signals:
    void bcuIndexChanged();
    void bcuSrcChanged();
    void outputFifo0Changed();
    void outputFifo1Changed();
    void outputFifo2Changed();
    void outputFifo3Changed();

private:
    void setComboBox(QComboBox* box,InputPortType type);
    void setComboBoxBfuX(QComboBox* box);
    void setComboBoxBfuY(QComboBox* box);
    void setComboBoxSbox(QComboBox* box);
    void setComboBoxBenes(QComboBox* box);

private:
    Ui::RcaBcuSetting *ui;
    static RcaBcuSetting* bcuSetingInstance;
//    RcaGraphScene* rcaScene;
    ModuleBcu* moduleBcu;

//    int indexBcu;
//    ModuleBcuIndex bcuSrc;
//    InputPortType inputType0;
//    int inputPort0;
//    InputPortType inputType1;
//    int inputPort1;
//    InputPortType inputType2;
//    int inputPort2;
//    InputPortType inputType3;
//    int inputPort3;

};

#endif // RCABCUSETTING_H
