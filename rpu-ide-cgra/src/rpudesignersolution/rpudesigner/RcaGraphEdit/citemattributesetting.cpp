#include "citemattributesetting.h"
#include "ui_citemattributesetting.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QTimer>
#include <QHBoxLayout>
#include "citemattributesetting.h"
#include "resourceselectdialog.h"
#include "RcaGraphScene.h"
#include "fileresolver/cxmlresolver.h"
#include "common/cappenv.hpp"
#include "../uistyle/cwidget.h"
#include "ModuleReadImd.h"
#include "ModuleReadRch.h"
#include "ModuleWriteRch.h"

CItemAttributeSetting::CItemAttributeSetting(RcaGraphScene *scene, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CItemAttributeSetting),
    m_curRcaScene(scene)
{
    ui->setupUi(this);
    m_widget = new CWidget(this);
    m_widget->hide();
    QHBoxLayout *layout = new QHBoxLayout(m_widget);
    m_widget->setLayout(layout);
    m_label = new QLabel();
    layout->setMargin(0);
    layout->addWidget(m_label);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    m_widget->setWindowFlags(m_widget->windowFlags() | Qt::Tool);

    ui->label_attribute_error->setText(" ");
    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::red);
    ui->label_attribute_error->setPalette(pe);
    ui->pushButton_help->setIcon(QIcon(CAppEnv::getImageDirPath() + "/help.png"));

    ui->toolButton_sbox->addAction(ui->action_sbox_resource);
    ui->toolButton_sbox->addAction(ui->action_sbox_explorer);
    ui->toolButton_benes->addAction(ui->action_benes_resource);
    ui->toolButton_benes->addAction(ui->action_benes_explorer);
    connect(ui->action_sbox_resource, SIGNAL(triggered(bool)), this, SLOT(showResourceWidget()));
    connect(ui->action_sbox_explorer, SIGNAL(triggered(bool)), this, SLOT(showExplorerWidget()));
    connect(ui->action_benes_resource, SIGNAL(triggered(bool)), this, SLOT(showResourceWidget()));
    connect(ui->action_benes_explorer, SIGNAL(triggered(bool)), this, SLOT(showExplorerWidget()));

    QList<QComboBox *> comboBoxList = this->findChildren<QComboBox *>();
    foreach (QComboBox* obj, comboBoxList)
    {
        if(obj->objectName().contains("input"))
            obj->setVisible(false);
    }
    QList<QLabel *> labelList = this->findChildren<QLabel *>();
    foreach (QLabel* obj, labelList)
    {
        if(obj->text().contains("输入"))
            obj->setVisible(false);
    }
}

CItemAttributeSetting::~CItemAttributeSetting()
{
    delete ui;
}

//【开始】图元属性设置
/*!
* CItemAttributeSetting::showItemAttribute
* \brief   显示图元属性停靠窗
* \author  zhangjun
* \date    2017-01-03
*/
void CItemAttributeSetting::updateItemAttribute(BaseItem *baseItem)
{
    disconnectAllChild();
    m_curBaseItem = baseItem;
    if(baseItem == nullptr)
    {
        ui->stackedWidget_attribute->setCurrentIndex(0);
        connectAllChild();
        return;
    }


    ModuleBcu* moduleBcu = dynamic_cast<ModuleBcu*>(baseItem);
    if(moduleBcu)
    {
        int indexBcu = moduleBcu->getIndexBcu() < BCU_MAX_COUNT ? moduleBcu->getIndexBcu() : 0;
        ui->comboBox_bcu_id->setCurrentIndex(indexBcu);

        ModuleBcuIndex bcuSrc = moduleBcu->srcDataFromBcu <= ModuleBcuIndex3 ? moduleBcu->srcDataFromBcu : ModuleBcuIndex0;
        ui->comboBox_bcu_source->setCurrentIndex(bcuSrc);
//NOTE 待删除，若有bug看这里
//        InputPortType inputType0 = moduleBcu->outFifo0->getInputType() <= InputPort_CurrentBenes ? moduleBcu->outFifo0->getInputType() : InputPort_NULL;
//        ui->comboBox_bcu_inputType0->setCurrentIndex(inputType0);

//        setComboBoxBcu(ui->comboBox_bcu_inputPort0,inputType0);
//        int inputPort0 = moduleBcu->outFifo0->getInputIndex() < ui->comboBox_bcu_inputPort0->count() ? moduleBcu->outFifo0->getInputIndex() : 0;
//        ui->comboBox_bcu_inputPort0->setCurrentIndex(inputPort0);

//        InputPortType inputType1 = moduleBcu->outFifo1->getInputType() <= InputPort_CurrentBenes ? moduleBcu->outFifo1->getInputType() : InputPort_NULL;
//        ui->comboBox_bcu_inputType1->setCurrentIndex(inputType1);

//        setComboBoxBcu(ui->comboBox_bcu_inputPort1,inputType1);
//        int inputPort1 = moduleBcu->outFifo1->getInputIndex() < ui->comboBox_bcu_inputPort1->count() ? moduleBcu->outFifo1->getInputIndex() : 0;
//        ui->comboBox_bcu_inputPort1->setCurrentIndex(inputPort1);

//        InputPortType inputType2 = moduleBcu->outFifo2->getInputType() <= InputPort_CurrentBenes ? moduleBcu->outFifo2->getInputType() : InputPort_NULL;
//        ui->comboBox_bcu_inputType2->setCurrentIndex(inputType2);

//        setComboBoxBcu(ui->comboBox_bcu_inputPort2,inputType2);
//        int inputPort2 = moduleBcu->outFifo2->getInputIndex() < ui->comboBox_bcu_inputPort2->count() ? moduleBcu->outFifo2->getInputIndex() : 0;
//        ui->comboBox_bcu_inputPort2->setCurrentIndex(inputPort2);

//        InputPortType inputType3 = moduleBcu->outFifo3->getInputType() <= InputPort_CurrentBenes ? moduleBcu->outFifo3->getInputType() : InputPort_NULL;
//        ui->comboBox_bcu_inputType3->setCurrentIndex(inputType3);

//        setComboBoxBcu(ui->comboBox_bcu_inputPort3,inputType3);
//        int inputPort3 = moduleBcu->outFifo3->getInputIndex() < ui->comboBox_bcu_inputPort3->count() ? moduleBcu->outFifo3->getInputIndex() : 0;
//        ui->comboBox_bcu_inputPort3->setCurrentIndex(inputPort3);

        ui->stackedWidget_attribute->setCurrentIndex(1);
        connectAllChild();
        return;
    }
    ModuleRcu* moduleRcu = dynamic_cast<ModuleRcu*>(baseItem);
    if(moduleRcu)
    {
        int indexRcu = moduleRcu->getIndexRcu() < RCU_MAX_COUNT ? moduleRcu->getIndexRcu() : 0;
        ui->comboBox_rcu_id->setCurrentIndex(indexRcu);

        int readMemMode128_32 = moduleRcu->readMemMode128_32 <= ui->comboBox_rcu_read_post->count() ? moduleRcu->readMemMode128_32 : 0;
        ui->comboBox_rcu_read_post->setCurrentIndex(readMemMode128_32);

        int readMemMode = moduleRcu->readMemMode <= ui->comboBox_rcu_read_mode->count() ? moduleRcu->readMemMode : 0;
        ui->comboBox_rcu_read_mode->setCurrentIndex(readMemMode);

        ui->spinBox_rcu_read_address1->setValue(moduleRcu->readMemAddr1);
        ui->spinBox_rcu_read_address2->setValue(moduleRcu->readMemAddr2);
        ui->spinBox_rcu_read_address3->setValue(moduleRcu->readMemAddr3);
        ui->spinBox_rcu_read_address4->setValue(moduleRcu->readMemAddr4);

        ui->spinBox_rcu_read_offset->setValue(moduleRcu->readMemOffset);
        ui->spinBox_rcu_read_rwTimes->setValue(moduleRcu->readMemThreashold);

        int writeMemMode = moduleRcu->writeMemMode <= ui->comboBox_rcu_write_mode->count() ? moduleRcu->writeMemMode : 0;
        ui->comboBox_rcu_write_mode->setCurrentIndex(writeMemMode);

        ui->spinBox_rcu_write_address->setValue(moduleRcu->writeMemAddr);

        int writeMemMask = moduleRcu->writeMemMask <= ui->comboBox_rcu_write_mask->count() ? moduleRcu->writeMemMask : 0;
        ui->comboBox_rcu_write_mask->setCurrentIndex(writeMemMask);

        ui->spinBox_rcu_write_offset->setValue(moduleRcu->writeMemOffset);
        ui->spinBox_rcu_write_rwTimes->setValue(moduleRcu->writeMemThreashold);
        on_comboBox_rcu_read_mode_currentIndexChanged(readMemMode);
        on_comboBox_rcu_write_mode_currentIndexChanged(writeMemMode);

        ui->stackedWidget_attribute->setCurrentIndex(2);
        connectAllChild();
        return;
    }
    ElementBfu* elementBfu = dynamic_cast<ElementBfu*>(baseItem);
    if(elementBfu)
    {
        int indexInRcu = elementBfu->getIndexInRcu() < BFU_MAX_COUNT ? elementBfu->getIndexInRcu() : 0;
        ui->comboBox_bfu_id->setCurrentIndex(indexInRcu);

        InputPortType inputTypeA = elementBfu->inPortA->getInputType() <= InputPort_Mem ? elementBfu->inPortA->getInputType() : InputPort_NULL;
        ui->comboBox_bfu_inputAType->setCurrentIndex(inputTypeA);
        setComboBoxBfu(ui->comboBox_bfu_inputAChoice,inputTypeA);

        int inputPortA = elementBfu->inPortA->getInputIndex() < ui->comboBox_bfu_inputAChoice->count() ? elementBfu->inPortA->getInputIndex() : 0;
        ui->comboBox_bfu_inputAChoice->setCurrentIndex(inputPortA);

        InputPortType inputTypeB = elementBfu->inPortB->getInputType() <= InputPort_Mem ? elementBfu->inPortB->getInputType() : InputPort_NULL;
        ui->comboBox_bfu_inputBType->setCurrentIndex(inputTypeB);
        setComboBoxBfu(ui->comboBox_bfu_inputBChoice,inputTypeB);

        int inputPortB = elementBfu->inPortB->getInputIndex() < ui->comboBox_bfu_inputBChoice->count() ? elementBfu->inPortB->getInputIndex() : 0;
        ui->comboBox_bfu_inputBChoice->setCurrentIndex(inputPortB);

        InputPortType inputTypeT = elementBfu->inPortT->getInputType() <= InputPort_Mem ? elementBfu->inPortT->getInputType() : InputPort_NULL;
        ui->comboBox_bfu_inputTType->setCurrentIndex(inputTypeT);

        setComboBoxBfu(ui->comboBox_bfu_inputTChoice,inputTypeT);
        int inputPortT = elementBfu->inPortT->getInputIndex() < ui->comboBox_bfu_inputTChoice->count() ? elementBfu->inPortT->getInputIndex() : 0;
        ui->comboBox_bfu_inputTChoice->setCurrentIndex(inputPortT);

        BfuFuncIndex funcIndex = elementBfu->funcIndex <= ui->comboBox_bfu_operator->count() ? elementBfu->funcIndex : Func_Au_Index;
        ui->comboBox_bfu_operator->setCurrentIndex(funcIndex);
        setBypassBox(funcIndex);
        setFuncMode(funcIndex, elementBfu);
        on_comboBox_bfu_operator_currentIndexChanged(funcIndex);

        ui->lineEdit_bfu_express->setText(elementBfu->funcExp);

        int bypassIndex = elementBfu->bypassIndex
                <= ui->comboBox_bfu_bypass->count() ? elementBfu->bypassIndex : 0;
        ui->comboBox_bfu_bypass->setCurrentIndex(bypassIndex);
        setBypassExp(bypassIndex, elementBfu);

        ui->stackedWidget_attribute->setCurrentIndex(3);
        connectAllChild();
        return;
    }
    ElementSbox* elementSbox = dynamic_cast<ElementSbox*>(baseItem);
    if(elementSbox)
    {
        int indexInRcu = elementSbox->getIndexInRcu() < SBOX_MAX_COUNT ? elementSbox->getIndexInRcu() : 0;
        ui->comboBox_sbox_id->setCurrentIndex(indexInRcu);

        InputPortType inputType0 = elementSbox->inPort0->getInputType()
                <= InputPort_Mem ? elementSbox->inPort0->getInputType() : InputPort_NULL;
        ui->comboBox_sbox_input0Type->setCurrentIndex(inputType0);

        setComboBoxBfu(ui->comboBox_sbox_input0Choice,inputType0);
        int inputPort0 = elementSbox->inPort0->getInputIndex()
                < ui->comboBox_sbox_input0Choice->count() ? elementSbox->inPort0->getInputIndex() : 0;
        ui->comboBox_sbox_input0Choice->setCurrentIndex(inputPort0);

        ui->comboBox_sbox_group0->setCurrentIndex(elementSbox->groupCnt0);
        ui->comboBox_sbox_group1->setCurrentIndex(elementSbox->groupCnt1);
        ui->comboBox_sbox_group2->setCurrentIndex(elementSbox->groupCnt2);
        ui->comboBox_sbox_group3->setCurrentIndex(elementSbox->groupCnt3);

        ui->comboBox_sbox_mode->setCurrentIndex(elementSbox->sboxMode);
        ui->lineEdit_sbox_path->setText(elementSbox->srcConfig);

        ui->stackedWidget_attribute->setCurrentIndex(4);
        connectAllChild();
        return;
    }
    ElementBenes* elementBenes = dynamic_cast<ElementBenes*>(baseItem);
    if(elementBenes)
    {
        int indexInRcu = elementBenes->getIndexInRcu()
                < BENES_MAX_COUNT ? elementBenes->getIndexInRcu() : 0;
        ui->comboBox_benes_id->setCurrentIndex(indexInRcu);

        InputPortType inputType0 = elementBenes->inPort0->getInputType()
                <= InputPort_Mem ? elementBenes->inPort0->getInputType() : InputPort_NULL;
        ui->comboBox_benes_input0Type->setCurrentIndex(inputType0);

        setComboBoxBfu(ui->comboBox_benes_input0Choice,inputType0);
        int inputPort0 = elementBenes->inPort0->getInputIndex()
                < ui->comboBox_benes_input0Choice->count() ? elementBenes->inPort0->getInputIndex() : 0;
        ui->comboBox_benes_input0Choice->setCurrentIndex(inputPort0);

        InputPortType inputType1 = elementBenes->inPort1->getInputType()
                <= InputPort_Mem ? elementBenes->inPort1->getInputType() : InputPort_NULL;
        ui->comboBox_benes_input1Type->setCurrentIndex(inputType1);

        setComboBoxBfu(ui->comboBox_benes_input1Choice,inputType1);
        int inputPort1 = elementBenes->inPort1->getInputIndex()
                < ui->comboBox_benes_input1Choice->count() ? elementBenes->inPort1->getInputIndex() : 0;
        ui->comboBox_benes_input1Choice->setCurrentIndex(inputPort1);

        InputPortType inputType2 = elementBenes->inPort2->getInputType()
                <= InputPort_Mem ? elementBenes->inPort2->getInputType() : InputPort_NULL;
        ui->comboBox_benes_input2Type->setCurrentIndex(inputType2);

        setComboBoxBfu(ui->comboBox_benes_input2Choice,inputType2);
        int inputPort2 = elementBenes->inPort2->getInputIndex()
                < ui->comboBox_benes_input2Choice->count() ? elementBenes->inPort2->getInputIndex() : 0;
        ui->comboBox_benes_input2Choice->setCurrentIndex(inputPort2);

        InputPortType inputType3 = elementBenes->inPort3->getInputType()
                <= InputPort_Mem ? elementBenes->inPort3->getInputType() : InputPort_NULL;
        ui->comboBox_benes_input3Type->setCurrentIndex(inputType3);

        setComboBoxBfu(ui->comboBox_benes_input3Choice,inputType3);
        int inputPort3 = elementBenes->inPort3->getInputIndex()
                < ui->comboBox_benes_input3Choice->count() ? elementBenes->inPort3->getInputIndex() : 0;
        ui->comboBox_benes_input3Choice->setCurrentIndex(inputPort3);

        ui->lineEdit_benes_path->setText(elementBenes->srcConfig);

        ui->stackedWidget_attribute->setCurrentIndex(5);
        connectAllChild();
        return;
    }
    ModuleReadImd* readImd = dynamic_cast<ModuleReadImd*>(baseItem);
    if(readImd)
    {
        ui->stackedWidget_attribute->setCurrentIndex(6);
        ui->comboBox_imdPort->setCurrentIndex(readImd->getReadImdIndex());
        connectAllChild();
        return;
    }
    ModuleReadRch* readRch = dynamic_cast<ModuleReadRch*>(baseItem);
    if(readRch)
    {
        ui->stackedWidget_attribute->setCurrentIndex(7);
//        resetRchComBobox(true,readRch->getReadRchIndex()/READ_RCH_MAX_COUNT_ONE_GROUP);
        ui->comboBox_rchPort->setCurrentText(QString::number(readRch->getReadRchIndex()));
        connectAllChild();
        return;
    }
    ModuleWriteRch* writeRch = dynamic_cast<ModuleWriteRch*>(baseItem);
    if(writeRch)
    {
        ui->stackedWidget_attribute->setCurrentIndex(7);
//        resetRchComBobox(false,writeRch->getBcuIndex());
        ui->comboBox_rchPort->setCurrentText(QString::number(writeRch->getWriteRchIndex()));
        connectAllChild();
        return;
    }
    ModuleReadMem* readMem = dynamic_cast<ModuleReadMem*>(baseItem);
    if(readMem)
    {
        ui->comboBox_memoryPort->setCurrentIndex(readMem->getReadMemIndex());
        ui->stackedWidget_attribute->setCurrentIndex(8);
        connectAllChild();
        return;
    }
    ModuleWriteMem* writeMem = dynamic_cast<ModuleWriteMem*>(baseItem);
    if(writeMem)
    {
        ui->comboBox_memoryPort->setCurrentIndex(writeMem->getWriteMemIndex());
        ui->stackedWidget_attribute->setCurrentIndex(8);
        connectAllChild();
        return;
    }

    connectAllChild();
}


void CItemAttributeSetting::connectAllChild()
{
    connect(ui->comboBox_bfu_operator,SIGNAL(currentIndexChanged(int)),this,SLOT(on_comboBox_bfu_operator_currentIndexChanged(int)));
    QList<QLineEdit *> lineEditList = this->findChildren<QLineEdit *>();
    foreach (QLineEdit* obj, lineEditList)
    {
        connect(obj,SIGNAL(editingFinished()),this,SLOT(saveAttribute()));
    }

    QList<QSpinBox *> spinBoxList = this->findChildren<QSpinBox *>();
    foreach (QSpinBox* obj, spinBoxList)
    {
        connect(obj,SIGNAL(valueChanged(int)),this,SLOT(saveAttribute()));
    }

    QList<QComboBox *> comboBoxList = this->findChildren<QComboBox *>();
    foreach (QComboBox* obj, comboBoxList)
    {
        connect(obj,SIGNAL(currentIndexChanged(int)),this,SLOT(saveAttribute()));
    }

//    on_comboBox_adsn_tds_currentIndexChanged(comboBox_adsn_tds->currentIndex());
//    on_comboBox_rcu_read_mode1_currentIndexChanged(comboBox_rcu_read_mode1->currentIndex());
//    on_comboBox_rcu_read_mode2_currentIndexChanged(comboBox_rcu_read_mode2->currentIndex());
//    on_comboBox_rcu_read_mode3_currentIndexChanged(comboBox_rcu_read_mode3->currentIndex());
//    on_comboBox_rcu_read_mode4_currentIndexChanged(comboBox_rcu_read_mode4->currentIndex());
//    on_comboBox_rcu_write_mode1_currentIndexChanged(comboBox_rcu_write_mode1->currentIndex());
//    on_comboBox_rcu_write_mode2_currentIndexChanged(comboBox_rcu_write_mode2->currentIndex());
//    on_comboBox_rcu_write_mode3_currentIndexChanged(comboBox_rcu_write_mode3->currentIndex());
//    on_comboBox_rcu_write_mode4_currentIndexChanged(comboBox_rcu_write_mode4->currentIndex());
}

void CItemAttributeSetting::disconnectAllChild()
{
    disconnect(ui->comboBox_bfu_operator,SIGNAL(currentIndexChanged(int)),this,SLOT(on_comboBox_bfu_operator_currentIndexChanged(int)));
    QList<QLineEdit *> lineEditList = this->findChildren<QLineEdit *>();
    foreach (QLineEdit* obj, lineEditList)
    {
        disconnect(obj,SIGNAL(editingFinished()),this,SLOT(saveAttribute()));
    }

    QList<QSpinBox *> spinBoxList = this->findChildren<QSpinBox *>();
    foreach (QSpinBox* obj, spinBoxList)
    {
        disconnect(obj,SIGNAL(valueChanged(int)),this,SLOT(saveAttribute()));
    }

    QList<QComboBox *> comboBoxList = this->findChildren<QComboBox *>();
    foreach (QComboBox* obj, comboBoxList)
    {
        disconnect(obj,SIGNAL(currentIndexChanged(int)),this,SLOT(saveAttribute()));
    }
}

void CItemAttributeSetting::reloadCurItemAttr()
{
    ui->label_attribute_error->setText(" ");
    updateItemAttribute(m_curBaseItem);
}

void CItemAttributeSetting::showResourceWidget()
{
    ResourceSelectDialog dlg(this);
    if(m_curRcaScene->getProject())
    {
        dlg.setScene(m_curRcaScene);
    }
    if(dlg.exec() == QDialog::Accepted)
    {
        if(m_curBaseItem->getUnitId() == SBOX_ID)
            ui->lineEdit_sbox_path->setText(CAppEnv::absToRel(m_curRcaScene->xmlInView->m_curFile, dlg.m_selectedResource));
        else if(m_curBaseItem->getUnitId() == BENES_ID)
            ui->lineEdit_benes_path->setText(CAppEnv::absToRel(m_curRcaScene->xmlInView->m_curFile, dlg.m_selectedResource));
        else
            return;
        saveAttribute();
    }
}

void CItemAttributeSetting::showExplorerWidget()
{
    QString filter;
    if(m_curBaseItem->getUnitId() == SBOX_ID)
        filter = u8"SBOX (*.sbox);;文本文档 (*.txt);;* (*.*)";
    else if(m_curBaseItem->getUnitId() == BENES_ID)
        filter = u8"BENES (*.benes);;文本文档 (*.txt);;* (*.*)";
    else
        filter = u8"文本文档 (*.txt);;* (*.*)";

    QString fileName = QFileDialog::getOpenFileName(
                this, tr(u8"请选择数据文件路径"),
                QFileInfo(m_curRcaScene->xmlInView->m_curFile).absolutePath(), filter);
    if(!fileName.trimmed().isEmpty())
    {
        if(m_curBaseItem->getUnitId() == SBOX_ID)
        {
            ui->lineEdit_sbox_path->setText(CAppEnv::absToRel(m_curRcaScene->xmlInView->m_curFile, fileName.trimmed()));
        }
        else if(m_curBaseItem->getUnitId() == BENES_ID)
        {
            ui->lineEdit_benes_path->setText(CAppEnv::absToRel(m_curRcaScene->xmlInView->m_curFile, fileName.trimmed()));
        }
        else
            return;
        saveAttribute();
    }
}

void CItemAttributeSetting::saveAttribute()
{
    if(!m_curBaseItem)
        return;

    switch (m_curBaseItem->getUnitId()) {
    case BCU_ID:
    {
        ModuleBcu moduleBcu(ui->comboBox_bcu_id->currentIndex());
        moduleBcu.srcDataFromBcu = ModuleBcuIndex(ui->comboBox_bcu_source->currentIndex());

        //NOTE 待删除，若有bug看这里
//        moduleBcu.outFifo0->setInputType(InputPortType(ui->comboBox_bcu_inputType0->currentIndex()));
//        moduleBcu.outFifo0->setInputIndex(ui->comboBox_bcu_inputPort0->currentIndex());
//        moduleBcu.outFifo1->setInputType(InputPortType(ui->comboBox_bcu_inputType1->currentIndex()));
//        moduleBcu.outFifo1->setInputIndex(ui->comboBox_bcu_inputPort1->currentIndex());
//        moduleBcu.outFifo2->setInputType(InputPortType(ui->comboBox_bcu_inputType2->currentIndex()));
//        moduleBcu.outFifo2->setInputIndex(ui->comboBox_bcu_inputPort2->currentIndex());
//        moduleBcu.outFifo3->setInputType(InputPortType(ui->comboBox_bcu_inputType3->currentIndex()));
//        moduleBcu.outFifo3->setInputIndex(ui->comboBox_bcu_inputPort3->currentIndex());
        QString ret = m_curRcaScene->setBaseItemAttr(&moduleBcu);
        if(!ret.isEmpty())
        {
            ui->label_attribute_error->setText(ret);
            QTimer::singleShot(1500,this, SLOT(reloadCurItemAttr()));
        }
    }
        break;
    case RCU_ID:
    {
        ModuleRcu* curRcu = dynamic_cast<ModuleRcu*>(m_curBaseItem);
        if(curRcu == NULL) return;

        ModuleRcu moduleRcu(curRcu->getIndexBcu(), ui->comboBox_rcu_id->currentIndex());
        moduleRcu.readMemMode128_32 = ui->comboBox_rcu_read_post->currentIndex();
        moduleRcu.readMemMode = ui->comboBox_rcu_read_mode->currentIndex();
        moduleRcu.readMemAddr1 = ui->spinBox_rcu_read_address1->value();
        moduleRcu.readMemAddr2 = ui->spinBox_rcu_read_address2->value();
        moduleRcu.readMemAddr3 = ui->spinBox_rcu_read_address3->value();
        moduleRcu.readMemAddr4 = ui->spinBox_rcu_read_address4->value();
        moduleRcu.readMemOffset = ui->spinBox_rcu_read_offset->value();
        moduleRcu.readMemThreashold = ui->spinBox_rcu_read_rwTimes->value();
        moduleRcu.writeMemMode = ui->comboBox_rcu_write_mode->currentIndex();
        moduleRcu.writeMemAddr = ui->spinBox_rcu_write_address->value();
        moduleRcu.writeMemMask = ui->comboBox_rcu_write_mask->currentIndex();
        moduleRcu.writeMemOffset = ui->spinBox_rcu_write_offset->value();
        moduleRcu.writeMemThreashold = ui->spinBox_rcu_write_rwTimes->value();
        QString ret = m_curRcaScene->setBaseItemAttr(&moduleRcu);
        if(!ret.isEmpty())
        {
            ui->label_attribute_error->setText(ret);
            QTimer::singleShot(1500,this, SLOT(reloadCurItemAttr()));
        }
    }
        break;
    case BFU_ID:
    {
        ElementBfu* curBfu = dynamic_cast<ElementBfu*>(m_curBaseItem);
        if(curBfu == NULL) return;

        ElementBfu elementBfu(curBfu->getIndexBcu(), curBfu->getIndexRcu(), ui->comboBox_bfu_id->currentIndex());
        elementBfu.inPortA->setInputType(InputPortType(ui->comboBox_bfu_inputAType->currentIndex()));
        elementBfu.inPortA->setInputIndex(ui->comboBox_bfu_inputAChoice->currentIndex());
        elementBfu.inPortB->setInputType(InputPortType(ui->comboBox_bfu_inputBType->currentIndex()));
        elementBfu.inPortB->setInputIndex(ui->comboBox_bfu_inputBChoice->currentIndex());
        elementBfu.inPortT->setInputType(InputPortType(ui->comboBox_bfu_inputTType->currentIndex()));
        elementBfu.inPortT->setInputIndex(ui->comboBox_bfu_inputTChoice->currentIndex());
        elementBfu.funcIndex = BfuFuncIndex(ui->comboBox_bfu_operator->currentIndex());
        elementBfu.funcExp = ui->lineEdit_bfu_express->text();
        elementBfu.bypassIndex = ui->comboBox_bfu_bypass->currentIndex();
        elementBfu.funcAuModIndex = ui->comboBox_bfu_funcAuMod->currentIndex();
        elementBfu.bypassExp = ui->lineEdit_bfu_bypassExp->text();
        QString ret = m_curRcaScene->setBaseItemAttr(&elementBfu);
        if(!ret.isEmpty())
        {
            ui->label_attribute_error->setText(ret);
            QTimer::singleShot(1500,this, SLOT(reloadCurItemAttr()));
        }
    }
        break;
    case SBOX_ID:
    {
        ElementSbox* curSbox = dynamic_cast<ElementSbox*>(m_curBaseItem);
        if(curSbox == NULL) return;

        ElementSbox elementSbox(curSbox->getIndexBcu(), curSbox->getIndexRcu(), ui->comboBox_sbox_id->currentIndex());
        elementSbox.inPort0->setInputType(InputPortType(ui->comboBox_sbox_input0Type->currentIndex()));
        elementSbox.inPort0->setInputIndex(ui->comboBox_sbox_input0Choice->currentIndex());
        elementSbox.groupCnt0 = SboxGroupValue(ui->comboBox_sbox_group0->currentIndex());
        elementSbox.groupCnt1 = SboxGroupValue(ui->comboBox_sbox_group1->currentIndex());
        elementSbox.groupCnt2 = SboxGroupValue(ui->comboBox_sbox_group2->currentIndex());
        elementSbox.groupCnt3 = SboxGroupValue(ui->comboBox_sbox_group3->currentIndex());
        elementSbox.sboxMode = ui->comboBox_sbox_mode->currentIndex();
        elementSbox.srcConfig = ui->lineEdit_sbox_path->text().trimmed();
        QString ret = m_curRcaScene->setBaseItemAttr(&elementSbox);
        if(!ret.isEmpty())
        {
            ui->label_attribute_error->setText(ret);
            QTimer::singleShot(1500,this, SLOT(reloadCurItemAttr()));
        }
    }
        break;
    case BENES_ID:
    {
        ElementBenes* curBenes = dynamic_cast<ElementBenes*>(m_curBaseItem);
        if(curBenes == NULL) return;

        ElementBenes elementBenes(curBenes->getIndexBcu(), curBenes->getIndexRcu(), ui->comboBox_benes_id->currentIndex());
        elementBenes.inPort0->setInputType(InputPortType(ui->comboBox_benes_input0Type->currentIndex()));
        elementBenes.inPort0->setInputIndex(ui->comboBox_benes_input0Choice->currentIndex());
        elementBenes.inPort1->setInputType(InputPortType(ui->comboBox_benes_input1Type->currentIndex()));
        elementBenes.inPort1->setInputIndex(ui->comboBox_benes_input1Choice->currentIndex());
        elementBenes.inPort2->setInputType(InputPortType(ui->comboBox_benes_input2Type->currentIndex()));
        elementBenes.inPort2->setInputIndex(ui->comboBox_benes_input2Choice->currentIndex());
        elementBenes.inPort3->setInputType(InputPortType(ui->comboBox_benes_input3Type->currentIndex()));
        elementBenes.inPort3->setInputIndex(ui->comboBox_benes_input3Choice->currentIndex());
        elementBenes.srcConfig = ui->lineEdit_benes_path->text().trimmed();
        QString ret = m_curRcaScene->setBaseItemAttr(&elementBenes);
        if(!ret.isEmpty())
        {
            ui->label_attribute_error->setText(ret);
            QTimer::singleShot(1500,this, SLOT(reloadCurItemAttr()));
        }
    }
        break;
    case RMEM_ID:
    {
        ModuleReadMem* curReadMem = dynamic_cast<ModuleReadMem*>(m_curBaseItem);
        if(curReadMem == NULL) return;

        ModuleReadMem readMem(ui->comboBox_memoryPort->currentIndex());
        QString ret = m_curRcaScene->setBaseItemAttr(&readMem);
        if(!ret.isEmpty())
        {
            ui->label_attribute_error->setText(ret);
            QTimer::singleShot(1500, this, SLOT(reloadCurItemAttr()));
        }
    }
        break;
    case WMEM_ID:
    {
        ModuleWriteMem* curWriteMem = dynamic_cast<ModuleWriteMem*>(m_curBaseItem);
        if(curWriteMem == NULL) return;

        ModuleWriteMem writeMem(ui->comboBox_memoryPort->currentIndex());
        QString ret = m_curRcaScene->setBaseItemAttr(&writeMem);
        if(!ret.isEmpty())
        {
            ui->label_attribute_error->setText(ret);
            QTimer::singleShot(1500, this, SLOT(reloadCurItemAttr()));
        }
    }
        break;
    case RIMD_ID:
    {
        ModuleReadImd readImd(ui->comboBox_imdPort->currentIndex());
        QString ret = m_curRcaScene->setBaseItemAttr(&readImd);
        if(!ret.isEmpty())
        {
            ui->label_attribute_error->setText(ret);
            QTimer::singleShot(1500,this,SLOT(reloadCurItemAttr()));
        }
    }break;
    case RRCH_ID:
    {
        ModuleReadRch readRch(ui->comboBox_rchPort->currentText().toInt());
        QString ret = m_curRcaScene->setBaseItemAttr(&readRch);
        if(!ret.isEmpty())
        {
            ui->label_attribute_error->setText(ret);
            QTimer::singleShot(1500,this,SLOT(reloadCurItemAttr()));
        }
    }
    break;
    case WRCH_ID:
    {
        int rchIndex = ui->comboBox_rchPort->currentText().toInt();

//        ModuleWriteRch writeRch(rchIndex,rchIndex/RCU_WRITE_RCH_MAX_COUNT,0);
        ModuleWriteRch writeRch(rchIndex);
        QString ret = m_curRcaScene->setBaseItemAttr(&writeRch);
        if(!ret.isEmpty())
        {
            ui->label_attribute_error->setText(ret);
            QTimer::singleShot(1500,this,SLOT(reloadCurItemAttr()));
        }
    }
    break;
    default:
        break;
    }
}


void CItemAttributeSetting::on_comboBox_bfu_operator_currentIndexChanged(int index)
{
    setBypassBox(BfuFuncIndex(index));
    setFuncMode(BfuFuncIndex(index));
    setBypassExp(ui->comboBox_bfu_bypass->currentIndex());
    QImage image;
    switch (index) {
    case 0:
        image.load(CAppEnv::getImageDirPath() + "/element/au.png");
        break;
    case 1:
        image.load(CAppEnv::getImageDirPath() + "/element/lu.png");
        break;
    case 2:
        image.load(CAppEnv::getImageDirPath() + "/element/su.png");
        break;
    default:
        break;
    }
    m_label->setPixmap(QPixmap::fromImage(image));
}

void CItemAttributeSetting::on_comboBox_bfu_bypass_currentIndexChanged(int index)
{
    setBypassExp(index);
}

void CItemAttributeSetting::on_comboBox_sbox_input0Type_currentIndexChanged(int index)
{
    setComboBoxBfu(ui->comboBox_sbox_input0Choice,InputPortType(index));
}

void CItemAttributeSetting::on_comboBox_bfu_inputAType_currentIndexChanged(int index)
{
    setComboBoxBfu(ui->comboBox_bfu_inputAChoice,InputPortType(index));
}

void CItemAttributeSetting::on_comboBox_bfu_inputBType_currentIndexChanged(int index)
{
    setComboBoxBfu(ui->comboBox_bfu_inputBChoice,InputPortType(index));
}

void CItemAttributeSetting::on_comboBox_bfu_inputTType_currentIndexChanged(int index)
{
    setComboBoxBfu(ui->comboBox_bfu_inputTChoice,InputPortType(index));
}

void CItemAttributeSetting::on_comboBox_bcu_inputType0_currentIndexChanged(int index)
{
    setComboBoxBcu(ui->comboBox_bcu_inputPort0,InputPortType(index));
}

void CItemAttributeSetting::on_comboBox_bcu_inputType1_currentIndexChanged(int index)
{
    setComboBoxBcu(ui->comboBox_bcu_inputPort1,InputPortType(index));
}

void CItemAttributeSetting::on_comboBox_bcu_inputType2_currentIndexChanged(int index)
{
    setComboBoxBcu(ui->comboBox_bcu_inputPort2,InputPortType(index));
}

void CItemAttributeSetting::on_comboBox_bcu_inputType3_currentIndexChanged(int index)
{
    setComboBoxBcu(ui->comboBox_bcu_inputPort3,InputPortType(index));
}

void CItemAttributeSetting::on_comboBox_benes_input0Type_currentIndexChanged(int index)
{
    setComboBoxBfu(ui->comboBox_benes_input0Choice,InputPortType(index));
}

void CItemAttributeSetting::on_comboBox_benes_input1Type_currentIndexChanged(int index)
{
    setComboBoxBfu(ui->comboBox_benes_input1Choice,InputPortType(index));
}

void CItemAttributeSetting::on_comboBox_benes_input2Type_currentIndexChanged(int index)
{
    setComboBoxBfu(ui->comboBox_benes_input2Choice,InputPortType(index));
}

void CItemAttributeSetting::on_comboBox_benes_input3Type_currentIndexChanged(int index)
{
    setComboBoxBfu(ui->comboBox_benes_input3Choice,InputPortType(index));
}

void CItemAttributeSetting::setBypassBox(BfuFuncIndex funcIndex)
{
    switch(funcIndex)
    {
    case Func_Su_Index:
    {
        if(ui->comboBox_bfu_bypass->count() == 3)
        {
            ui->comboBox_bfu_bypass->addItem("B'");
            ui->comboBox_bfu_bypass->setCurrentIndex(0);
        }
        break;
    }
    case Func_Au_Index:
    case Func_Lu_Index:
    default:
    {
        if(ui->comboBox_bfu_bypass->count() == 4)
        {
            ui->comboBox_bfu_bypass->removeItem(3);
            ui->comboBox_bfu_bypass->setCurrentIndex(0);
        }
        break;
    }
    }
}

void CItemAttributeSetting::setFuncMode(BfuFuncIndex funcIndex, ElementBfu* elementBfu)
{
    switch(funcIndex)
    {
    case Func_Au_Index:
    {
        ui->label_funcAuMod->setEnabled(true);
        ui->comboBox_bfu_funcAuMod->setEnabled(true);
        if(elementBfu)
            ui->comboBox_bfu_funcAuMod->setCurrentIndex(elementBfu->funcAuModIndex);
        break;
    }
    case Func_Lu_Index:
    case Func_Su_Index:
    default:
    {
        ui->label_funcAuMod->setEnabled(false);
        ui->comboBox_bfu_funcAuMod->setEnabled(false);
        break;
    }
    }
}

void CItemAttributeSetting::setBypassExp(int bypassIndex, ElementBfu *elementBfu)
{
    if((ui->comboBox_bfu_operator->currentIndex() == 2) && (bypassIndex == 3))
    {
        ui->label_bypassExp->setEnabled(true);
        ui->lineEdit_bfu_bypassExp->setEnabled(true);
        if(elementBfu)
            ui->lineEdit_bfu_bypassExp->setText(elementBfu->bypassExp);
    }
    else
    {
        ui->label_bypassExp->setEnabled(false);
        ui->lineEdit_bfu_bypassExp->setEnabled(false);
    }
}

void CItemAttributeSetting::on_comboBox_rcu_read_mode_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        ui->label_address1_6->setText(tr(u8"读写(阈值)："));
        ui->label_address1_6->setEnabled(false);
        ui->spinBox_rcu_read_rwTimes->setEnabled(false);
        ui->spinBox_rcu_read_rwTimes->setDisplayIntegerBase(16);
        break;
    case 1:
        ui->label_address1_6->setText(tr(u8"读写(阈值)："));
        ui->label_address1_6->setEnabled(false);
        ui->spinBox_rcu_read_rwTimes->setEnabled(false);
        ui->spinBox_rcu_read_rwTimes->setDisplayIntegerBase(16);
        break;
    case 2:
        ui->label_address1_6->setText(tr(u8"读写次数"));
        ui->label_address1_6->setEnabled(true);
        ui->spinBox_rcu_read_rwTimes->setEnabled(true);
        ui->spinBox_rcu_read_rwTimes->setDisplayIntegerBase(10);
        break;
    case 3:
        ui->label_address1_6->setText(tr(u8"边界地址"));
        ui->label_address1_6->setEnabled(true);
        ui->spinBox_rcu_read_rwTimes->setEnabled(true);
        ui->spinBox_rcu_read_rwTimes->setDisplayIntegerBase(16);
        break;
    default:
        break;
    }
}

void CItemAttributeSetting::on_comboBox_rcu_write_mode_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        ui->label_address1_12->setText(tr(u8"读写(阈值)："));
        ui->label_address1_12->setEnabled(false);
        ui->spinBox_rcu_write_rwTimes->setEnabled(false);
        ui->spinBox_rcu_write_rwTimes->setDisplayIntegerBase(16);
        break;
    case 1:
        ui->label_address1_12->setText(tr(u8"读写(阈值)："));
        ui->label_address1_12->setEnabled(false);
        ui->spinBox_rcu_write_rwTimes->setEnabled(false);
        ui->spinBox_rcu_write_rwTimes->setDisplayIntegerBase(16);
        break;
    case 2:
        ui->label_address1_12->setText(tr(u8"读写次数"));
        ui->label_address1_12->setEnabled(true);
        ui->spinBox_rcu_write_rwTimes->setEnabled(true);
        ui->spinBox_rcu_write_rwTimes->setDisplayIntegerBase(10);
        break;
    case 3:
        ui->label_address1_12->setText(tr(u8"边界地址"));
        ui->label_address1_12->setEnabled(true);
        ui->spinBox_rcu_write_rwTimes->setEnabled(true);
        ui->spinBox_rcu_write_rwTimes->setDisplayIntegerBase(16);
        break;
    default:
        break;
    }
}

void CItemAttributeSetting::setComboBoxBcu(QComboBox* box,InputPortType type)
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

void CItemAttributeSetting::setComboBoxBfu(QComboBox *box, InputPortType type)
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

void CItemAttributeSetting::setComboBoxBfuX(QComboBox* box)
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

void CItemAttributeSetting::setComboBoxBfuY(QComboBox* box)
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

void CItemAttributeSetting::setComboBoxSbox(QComboBox* box)
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

void CItemAttributeSetting::setComboBoxBenes(QComboBox* box)
{
    box->clear();
    box->insertItem(0,"0");
    box->insertItem(1,"1");
    box->insertItem(2,"2");
    box->insertItem(3,"3");
}

void CItemAttributeSetting::setComboBoxMem(QComboBox* box)
{
    box->clear();
    box->insertItem(0,"0");
    box->insertItem(1,"1");
    box->insertItem(2,"2");
    box->insertItem(3,"3");
}

void CItemAttributeSetting::setComboBoxFifo(QComboBox* box)
{
    box->clear();
    box->insertItem(0,"0");
    box->insertItem(1,"1");
    box->insertItem(2,"2");
    box->insertItem(3,"3");
}

void CItemAttributeSetting::on_pushButton_help_clicked()
{
    m_widget->adjustSize();
    m_widget->show();
    QRect screenRect = QApplication::desktop()->availableGeometry();
    if(!screenRect.contains(m_widget->rect(), true))
    {
        int x = m_widget->x();
        int y = m_widget->y();
        x = x < 0 ? 0 : x;
        y = y < 0 ? 0 : y;
        x = x > screenRect.width() - m_widget->width()
                ? screenRect.width() - m_widget->width() : x;
        y = y > screenRect.height() - m_widget->height()
                ? screenRect.height() - m_widget->height() : y;
        m_widget->move(x, y);
    }
}
