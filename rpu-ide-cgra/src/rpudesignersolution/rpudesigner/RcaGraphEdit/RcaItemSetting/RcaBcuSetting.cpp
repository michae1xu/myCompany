#include "RcaBcuSetting.h"
#include "ui_RcaBcuSetting.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>

RcaBcuSetting* RcaBcuSetting::bcuSetingInstance = NULL;

RcaBcuSetting::RcaBcuSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RcaBcuSetting)
//    , rcaScene(NULL)
    , moduleBcu(NULL)
//    , indexBcu(0)
//    , bcuSrc(ModuleBcuIndex0)
//    , inputType0(InputPort_NULL)
//    , inputPort0(0)
//    , inputType1(InputPort_NULL)
//    , inputPort1(0)
//    , inputType2(InputPort_NULL)
//    , inputPort2(0)
//    , inputType3(InputPort_NULL)
//    , inputPort3(0)
{
    ui->setupUi(this);
    bcuSetingInstance = this;
    Qt::WindowFlags flags = windowFlags();
    setWindowFlags(flags | Qt::WindowStaysOnTopHint);
}

RcaBcuSetting::~RcaBcuSetting()
{
    delete ui;
}

void RcaBcuSetting::setRcaScene(RcaGraphScene* scene)
{
    if(scene == NULL) return;
//    rcaScene = scene;
}

void RcaBcuSetting::handleBcuBeDeleted(BaseItem *deletedItem)
{
    Q_UNUSED(deletedItem);
    disconnect(moduleBcu,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleBcuBeDeleted(BaseItem*)));
    moduleBcu = NULL;
    hide();
}

void RcaBcuSetting::showBcu(ModuleBcu* bcu, QPoint pos)
{
    QL_DEBUG;
    if(bcu == NULL) return;
    if(moduleBcu != bcu)
    {
        if(moduleBcu)
        {
            disconnect(moduleBcu,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleBcuBeDeleted(BaseItem*)));
        }
        moduleBcu = bcu;
        connect(moduleBcu,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleBcuBeDeleted(BaseItem*)));
    }
    QL_DEBUG;

    int indexBcu = moduleBcu->getIndexBcu() < BCU_MAX_COUNT ? moduleBcu->getIndexBcu() : 0;
    ui->comboBox_bcuId->setCurrentIndex(indexBcu);

    ModuleBcuIndex bcuSrc = moduleBcu->srcDataFromBcu <= ModuleBcuIndex3 ? moduleBcu->srcDataFromBcu : ModuleBcuIndex0;
    ui->comboBox_bcuSource->setCurrentIndex(bcuSrc);

    InputPortType inputType0 = moduleBcu->outFifo0->getInputType() <= InputPort_CurrentBenes ? moduleBcu->outFifo0->getInputType() : InputPort_NULL;
    ui->comboBox_inputType0->setCurrentIndex(inputType0);

    setComboBox(ui->comboBox_inputPort0,inputType0);
    int inputPort0 = moduleBcu->outFifo0->getInputIndex() < ui->comboBox_inputPort0->count() ? moduleBcu->outFifo0->getInputIndex() : 0;
    ui->comboBox_inputPort0->setCurrentIndex(inputPort0);

    InputPortType inputType1 = moduleBcu->outFifo1->getInputType() <= InputPort_CurrentBenes ? moduleBcu->outFifo1->getInputType() : InputPort_NULL;
    ui->comboBox_inputType1->setCurrentIndex(inputType1);

    setComboBox(ui->comboBox_inputPort1,inputType1);
    int inputPort1 = moduleBcu->outFifo1->getInputIndex() < ui->comboBox_inputPort1->count() ? moduleBcu->outFifo1->getInputIndex() : 0;
    ui->comboBox_inputPort1->setCurrentIndex(inputPort1);

    InputPortType inputType2 = moduleBcu->outFifo2->getInputType() <= InputPort_CurrentBenes ? moduleBcu->outFifo2->getInputType() : InputPort_NULL;
    ui->comboBox_inputType2->setCurrentIndex(inputType2);

    setComboBox(ui->comboBox_inputPort2,inputType2);
    int inputPort2 = moduleBcu->outFifo2->getInputIndex() < ui->comboBox_inputPort2->count() ? moduleBcu->outFifo2->getInputIndex() : 0;
    ui->comboBox_inputPort2->setCurrentIndex(inputPort2);

    InputPortType inputType3 = moduleBcu->outFifo3->getInputType() <= InputPort_CurrentBenes ? moduleBcu->outFifo3->getInputType() : InputPort_NULL;
    ui->comboBox_inputType3->setCurrentIndex(inputType3);

    setComboBox(ui->comboBox_inputPort3,inputType3);
    int inputPort3 = moduleBcu->outFifo3->getInputIndex() < ui->comboBox_inputPort3->count() ? moduleBcu->outFifo3->getInputIndex() : 0;
    ui->comboBox_inputPort3->setCurrentIndex(inputPort3);

    show();
    if(!pos.isNull())
    {
        move(pos);
    }

}

void RcaBcuSetting::setComboBox(QComboBox* box,InputPortType type)
{
    box->clear();
    switch(type)
    {
        case InputPort_CurrentBfuX: setComboBoxBfuX(box);break;
        case InputPort_CurrentBfuY: setComboBoxBfuY(box);break;
        case InputPort_CurrentSbox: setComboBoxSbox(box);break;
        case InputPort_CurrentBenes: setComboBoxBenes(box);break;
        case InputPort_NULL:
        default: box->insertItem(0,"");break;
    }
}

void RcaBcuSetting::setComboBoxBfuX(QComboBox* box)
{
    box->clear();
    box->insertItem(0,"0");
    box->insertItem(1,"1");
    box->insertItem(2,"2");
    box->insertItem(3,"3");
    box->insertItem(4,"4");
    box->insertItem(5,"5");
    box->insertItem(6,"6");
    box->insertItem(7,"7");
}

void RcaBcuSetting::setComboBoxBfuY(QComboBox* box)
{
    box->clear();
    box->insertItem(0,"0");
    box->insertItem(1,"1");
    box->insertItem(2,"2");
    box->insertItem(3,"3");
    box->insertItem(4,"4");
    box->insertItem(5,"5");
    box->insertItem(6,"6");
    box->insertItem(7,"7");
}

void RcaBcuSetting::setComboBoxSbox(QComboBox* box)
{
    box->clear();
    box->insertItem(0,"0");
    box->insertItem(1,"1");
    box->insertItem(2,"2");
    box->insertItem(3,"3");
    box->insertItem(4,"4");
    box->insertItem(5,"5");
    box->insertItem(6,"6");
    box->insertItem(7,"7");
    box->insertItem(8,"8");
    box->insertItem(9,"9");
    box->insertItem(10,"10");
    box->insertItem(11,"11");
    box->insertItem(12,"12");
    box->insertItem(13,"13");
    box->insertItem(14,"14");
    box->insertItem(15,"15");
}

void RcaBcuSetting::setComboBoxBenes(QComboBox* box)
{
    box->clear();
    box->insertItem(0,"0");
    box->insertItem(1,"1");
    box->insertItem(2,"2");
    box->insertItem(3,"3");
}

void RcaBcuSetting::on_buttonBox_clicked(QAbstractButton *button)
{
    if(moduleBcu == NULL) return;
    if(button == ui->buttonBox->button(QDialogButtonBox::Ok))
    {
        QL_DEBUG;
        int uiIndexBcu = ui->comboBox_bcuId->currentIndex();
        if(moduleBcu->getIndexBcu() != uiIndexBcu)
        {
            emit bcuIndexChanged();
            moduleBcu->setIndexBcu(uiIndexBcu);
        }
        ModuleBcuIndex uiBcuSrc = ModuleBcuIndex(ui->comboBox_bcuSource->currentIndex());
        if(moduleBcu->srcDataFromBcu != uiBcuSrc)
        {
            emit bcuSrcChanged();
            moduleBcu->srcDataFromBcu = uiBcuSrc;
        }

        InputPortType uiInputType0 = InputPortType(ui->comboBox_inputType0->currentIndex());
        int uiInputPort0 = ui->comboBox_inputPort0->currentIndex();
        if((uiInputType0 != moduleBcu->outFifo0->getInputType())
                || uiInputPort0 != moduleBcu->outFifo0->getInputIndex())
        {
            emit outputFifo0Changed();
            //重新找到port口进行连接，如果没有则提示。
            QL_DEBUG << uiInputType0 << uiInputPort0;
            if(moduleBcu->outFifo0->resetInputSignal(uiInputType0,uiInputPort0) == false)
            {
                QMessageBox::information(this, tr(u8"提示"), tr(u8"不存在OutFiFo_0选择的输入"));
            }
        }

        InputPortType uiInputType1 = InputPortType(ui->comboBox_inputType1->currentIndex());
        int uiInputPort1 = ui->comboBox_inputPort1->currentIndex();
        if((uiInputType1 != moduleBcu->outFifo1->getInputType())
                || uiInputPort1 != moduleBcu->outFifo1->getInputIndex())
        {
            emit outputFifo1Changed();
            //重新找到port口进行连接，如果没有则提示。
            QL_DEBUG << uiInputType1 << uiInputPort1;
            if(moduleBcu->outFifo1->resetInputSignal(uiInputType1,uiInputPort1) == false)
            {
                QMessageBox::information(this, tr(u8"提示"), tr(u8"不存在OutFiFo_1选择的输入"));
            }
        }

        InputPortType uiInputType2 = InputPortType(ui->comboBox_inputType2->currentIndex());
        int uiInputPort2 = ui->comboBox_inputPort2->currentIndex();
        if((uiInputType2 != moduleBcu->outFifo2->getInputType())
                || uiInputPort2 != moduleBcu->outFifo2->getInputIndex())
        {
            emit outputFifo2Changed();
            //重新找到port口进行连接，如果没有则提示。
            QL_DEBUG << uiInputType2 << uiInputPort2;
            if(moduleBcu->outFifo2->resetInputSignal(uiInputType2,uiInputPort2) == false)
            {
                QMessageBox::information(this, tr(u8"提示"), tr(u8"不存在OutFiFo_2选择的输入"));
            }
        }

        InputPortType uiInputType3 = InputPortType(ui->comboBox_inputType3->currentIndex());
        int uiInputPort3 = ui->comboBox_inputPort3->currentIndex();
        if((uiInputType3 != moduleBcu->outFifo3->getInputType())
                || uiInputPort3 != moduleBcu->outFifo3->getInputIndex())
        {
            emit outputFifo3Changed();
            //重新找到port口进行连接，如果没有则提示。
            QL_DEBUG << uiInputType3 << uiInputPort3;
            if(moduleBcu->outFifo3->resetInputSignal(uiInputType3,uiInputPort3) == false)
            {
                QMessageBox::information(this, tr(u8"提示"), tr(u8"不存在OutFiFo_3选择的输入"));
            }
        }
        moduleBcu->sendChangedItemToScene();
    }
}

void RcaBcuSetting::on_comboBox_inputType0_currentIndexChanged(int index)
{
    QL_DEBUG;
    setComboBox(ui->comboBox_inputPort0,InputPortType(index));
}

void RcaBcuSetting::on_comboBox_inputType1_currentIndexChanged(int index)
{
    QL_DEBUG;
    setComboBox(ui->comboBox_inputPort1,InputPortType(index));
}

void RcaBcuSetting::on_comboBox_inputType2_currentIndexChanged(int index)
{
    QL_DEBUG;
    setComboBox(ui->comboBox_inputPort2,InputPortType(index));
}

void RcaBcuSetting::on_comboBox_inputType3_currentIndexChanged(int index)
{
    QL_DEBUG;
    setComboBox(ui->comboBox_inputPort3,InputPortType(index));
}
