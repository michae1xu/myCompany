#include "RcaBfuSetting.h"
#include "ui_RcaBfuSetting.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>

RcaBfuSetting* RcaBfuSetting::bfuSetingInstance = NULL;

RcaBfuSetting::RcaBfuSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RcaBfuSetting)
//    , rcaScene(NULL)
    , elementBfu(NULL)
{
    ui->setupUi(this);
    bfuSetingInstance = this;
    Qt::WindowFlags flags = windowFlags();
    setWindowFlags(flags | Qt::WindowStaysOnTopHint);
}

RcaBfuSetting::~RcaBfuSetting()
{
    delete ui;
}

//void RcaBfuSetting::setRcaScene(RcaGraphScene* scene)
//{
//    if(scene == NULL) return;
//    rcaScene = scene;
//}

void RcaBfuSetting::handleBfuBeDeleted(BaseItem* deletedItem)
{
    Q_UNUSED(deletedItem);
    disconnect(elementBfu,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleBfuBeDeleted(BaseItem*)));
    elementBfu = NULL;
    hide();
}

void RcaBfuSetting::showBfu(ElementBfu* bfu, QPoint pos)
{
    QL_DEBUG;
    if(bfu == NULL) return;
    if(elementBfu != bfu)
    {
        if(elementBfu)
        {
            disconnect(elementBfu,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleBfuBeDeleted(BaseItem*)));
        }
        elementBfu = bfu;
        connect(elementBfu,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleBfuBeDeleted(BaseItem*)));
    }
    QL_DEBUG;

    int indexInRcu = elementBfu->getIndexInRcu() < BFU_MAX_COUNT ? elementBfu->getIndexInRcu() : 0;
    ui->comboBox_bfuId->setCurrentIndex(indexInRcu);

    InputPortType inputTypeA = elementBfu->inPortA->getInputType() <= InputPort_Mem ? elementBfu->inPortA->getInputType() : InputPort_NULL;
    ui->comboBox_inputAType->setCurrentIndex(inputTypeA);

    setComboBox(ui->comboBox_inputAChoice,inputTypeA);
    int inputPortA = elementBfu->inPortA->getInputIndex() < ui->comboBox_inputAChoice->count() ? elementBfu->inPortA->getInputIndex() : 0;
    ui->comboBox_inputAChoice->setCurrentIndex(inputPortA);

    InputPortType inputTypeB = elementBfu->inPortB->getInputType() <= InputPort_Mem ? elementBfu->inPortB->getInputType() : InputPort_NULL;
    ui->comboBox_inputBType->setCurrentIndex(inputTypeB);
    setComboBox(ui->comboBox_inputBChoice,inputTypeB);

    int inputPortB = elementBfu->inPortB->getInputIndex() < ui->comboBox_inputBChoice->count() ? elementBfu->inPortB->getInputIndex() : 0;
    ui->comboBox_inputBChoice->setCurrentIndex(inputPortB);

    InputPortType inputTypeT = elementBfu->inPortT->getInputType() <= InputPort_Mem ? elementBfu->inPortT->getInputType() : InputPort_NULL;
    ui->comboBox_inputTType->setCurrentIndex(inputTypeT);

    setComboBox(ui->comboBox_inputTChoice,inputTypeT);
    int inputPortT = elementBfu->inPortT->getInputIndex() < ui->comboBox_inputTChoice->count() ? elementBfu->inPortT->getInputIndex() : 0;
    ui->comboBox_inputTChoice->setCurrentIndex(inputPortT);

    BfuFuncIndex funcIndex = elementBfu->funcIndex <= ui->comboBox_operator->count() ? elementBfu->funcIndex : Func_Au_Index;
    ui->comboBox_operator->setCurrentIndex(funcIndex);
    setBypassBox(funcIndex);
    setFuncMode(funcIndex);

    ui->lineEdit_express->setText(elementBfu->funcExp);

    int bypassIndex = elementBfu->bypassIndex <= ui->comboBox_bypass->count() ? elementBfu->bypassIndex : 0;
    ui->comboBox_bypass->setCurrentIndex(bypassIndex);
    setBypassExp(bypassIndex);

    show();
    if(!pos.isNull())
    {
        move(pos);
    }
}

void RcaBfuSetting::on_comboBox_operator_currentIndexChanged(int index)
{
    setBypassBox(BfuFuncIndex(index));
    setFuncMode(BfuFuncIndex(index));
    setBypassExp(ui->comboBox_bypass->currentIndex());
}

void RcaBfuSetting::on_comboBox_bypass_currentIndexChanged(int index)
{
    setBypassExp(index);
}

void RcaBfuSetting::setBypassBox(BfuFuncIndex funcIndex)
{
    switch(funcIndex)
    {
        case Func_Su_Index:
        {
            if(ui->comboBox_bypass->count() == 3)
            {
                ui->comboBox_bypass->addItem("B'");
                ui->comboBox_bypass->setCurrentIndex(0);
            }
            break;
        }
        case Func_Au_Index:
        case Func_Lu_Index:
        default:
        {
            if(ui->comboBox_bypass->count() == 4)
            {
                ui->comboBox_bypass->removeItem(3);
                ui->comboBox_bypass->setCurrentIndex(0);
            }
            break;
        }
    }
}

void RcaBfuSetting::setFuncMode(BfuFuncIndex funcIndex)
{
    if(elementBfu == NULL)  return;
    switch(funcIndex)
    {
        case Func_Au_Index:
        {
            ui->label_funcAuMod->setVisible(true);
            ui->comboBox_funcAuMod->setVisible(true);
            ui->comboBox_funcAuMod->setCurrentIndex(elementBfu->funcAuModIndex);
            break;
        }
        case Func_Lu_Index:
        case Func_Su_Index:
        default:
        {
            ui->label_funcAuMod->setVisible(false);
            ui->comboBox_funcAuMod->setVisible(false);
            break;
        }
    }
}

void RcaBfuSetting::setBypassExp(int bypassIndex)
{
    if((ui->comboBox_operator->currentIndex() == 2) && (bypassIndex == 3) && (elementBfu))
    {
        ui->label_bypassExp->setVisible(true);
        ui->lineEdit_bypassExp->setVisible(true);
        ui->lineEdit_bypassExp->setText(elementBfu->bypassExp);
    }
    else
    {
        ui->label_bypassExp->setVisible(false);
        ui->lineEdit_bypassExp->setVisible(false);
    }
}


void RcaBfuSetting::on_buttonBox_clicked(QAbstractButton *button)
{
    if(elementBfu == NULL) return;
    if(button == ui->buttonBox->button(QDialogButtonBox::Ok))
    {
        QL_DEBUG;
        elementBfu->setIndexInRcu(ui->comboBox_bfuId->currentIndex());

        InputPortType uiInputAType = InputPortType(ui->comboBox_inputAType->currentIndex());
        int uiInputAIndex = ui->comboBox_inputAChoice->currentIndex();
        if((uiInputAType != elementBfu->inPortA->getInputType())
                || uiInputAIndex != elementBfu->inPortA->getInputIndex())
        {
            //重新找到port口进行连接，如果没有则提示。
            QL_DEBUG << uiInputAType << uiInputAIndex;
            if(elementBfu->inPortA->resetInputSignal(uiInputAType,uiInputAIndex) == false)
            {
                QMessageBox::information(this, tr(u8"提示"), tr(u8"不存在输入A选择的输入"));
            }
        }

        InputPortType uiInputBType = InputPortType(ui->comboBox_inputBType->currentIndex());
        int uiInputBIndex = ui->comboBox_inputBChoice->currentIndex();
        if((uiInputBType != elementBfu->inPortB->getInputType())
                || uiInputBIndex != elementBfu->inPortB->getInputIndex())
        {
            //重新找到port口进行连接，如果没有则提示。
            QL_DEBUG << uiInputBType << uiInputBIndex;
            if(elementBfu->inPortB->resetInputSignal(uiInputBType,uiInputBIndex) == false)
            {
                QMessageBox::information(this, tr(u8"提示"), tr(u8"不存在输入B选择的输入"));
            }
        }

        InputPortType uiInputTType = InputPortType(ui->comboBox_inputTType->currentIndex());
        int uiInputTIndex = ui->comboBox_inputTChoice->currentIndex();
        if((uiInputTType != elementBfu->inPortT->getInputType())
                || uiInputTIndex != elementBfu->inPortT->getInputIndex())
        {
            //重新找到port口进行连接，如果没有则提示。
            QL_DEBUG << uiInputTType << uiInputTIndex;
            if(elementBfu->inPortT->resetInputSignal(uiInputTType,uiInputTIndex) == false)
            {
                QMessageBox::information(this, tr(u8"提示"), tr(u8"不存在输入T选择的输入"));
            }
        }
        elementBfu->funcIndex = BfuFuncIndex(ui->comboBox_operator->currentIndex());
        elementBfu->funcExp = ui->lineEdit_express->text();
        elementBfu->bypassIndex = ui->comboBox_bypass->currentIndex();

        elementBfu->funcAuModIndex = ui->comboBox_funcAuMod->currentIndex();

        elementBfu->bypassExp = ui->lineEdit_bypassExp->text();

        elementBfu->sendChangedItemToScene();
    }
}

void RcaBfuSetting::setComboBox(QComboBox* box,InputPortType type)
{
    box->clear();
    switch(type)
    {
        case InputPort_CurrentBfuX:
        case InputPort_LastBfuX: setComboBoxBfuX(box);break;
        case InputPort_CurrentBfuY:
        case InputPort_LastBfuY: setComboBoxBfuY(box);break;
        case InputPort_CurrentSbox:
        case InputPort_LastSbox: setComboBoxSbox(box);break;
        case InputPort_CurrentBenes:
        case InputPort_LastBenes: setComboBoxBenes(box);break;
        case InputPort_InFifo: setComboBoxFifo(box);break;
        case InputPort_Mem:setComboBoxMem(box);break;
        case InputPort_NULL:
        default: break;
    }
}

void RcaBfuSetting::setComboBoxBfuX(QComboBox* box)
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

void RcaBfuSetting::setComboBoxBfuY(QComboBox* box)
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

void RcaBfuSetting::setComboBoxSbox(QComboBox* box)
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

void RcaBfuSetting::setComboBoxBenes(QComboBox* box)
{
    box->clear();
    box->insertItem(0,"0");
    box->insertItem(1,"1");
    box->insertItem(2,"2");
    box->insertItem(3,"3");
}


void RcaBfuSetting::setComboBoxMem(QComboBox* box)
{
    box->clear();
    box->insertItem(0,"0");
    box->insertItem(1,"1");
    box->insertItem(2,"2");
    box->insertItem(3,"3");
}

void RcaBfuSetting::setComboBoxFifo(QComboBox* box)
{
    box->clear();
    box->insertItem(0,"0");
    box->insertItem(1,"1");
    box->insertItem(2,"2");
    box->insertItem(3,"3");
}

void RcaBfuSetting::on_comboBox_inputAType_currentIndexChanged(int index)
{
    QL_DEBUG;
    setComboBox(ui->comboBox_inputAChoice,InputPortType(index));
}

void RcaBfuSetting::on_comboBox_inputBType_currentIndexChanged(int index)
{
    QL_DEBUG;
    setComboBox(ui->comboBox_inputBChoice,InputPortType(index));
}

void RcaBfuSetting::on_comboBox_inputTType_currentIndexChanged(int index)
{
    QL_DEBUG;
    setComboBox(ui->comboBox_inputTChoice,InputPortType(index));
}

