/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    attrtreewidget.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include "attrtreewidget.h"
#include <QRegExp>
#include <QValidator>
#include <QDebug>

/**
 * AttrTreeWidget::AttrTreeWidget
 * \brief   constructor of AttrTreeWidget
 * \param   filename
 * \param   xml
 * \author  zhangjun
 * \date    2016-10-12
 */
AttrTreeWidget::AttrTreeWidget(QString filename, XmlResolver *xml) :
    filename(filename),
    xml(xml)
{
    //头部
    setColumnCount(2);
    QStringList headers;
    headers << trUtf8("属性");
    headers << trUtf8("值");
    setHeaderLabels(headers);
    //初始化
    initAttributeItemMap();
    currentElement = ELEMENT_WIRE;
    currentBCU = -1;
    currentRCU = -1;
    currentID = -1;
    currentComBox = NULL;
    currentSpinBox = NULL;
    currentAttributeID = ID;
    //
    connect(this,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(itemClickedSlot(QTreeWidgetItem*,int)));
}

/**
 * AttrTreeWidget::~AttrTreeWidget
 * \brief   destructor of AttrTreeWidget
 * \author  zhangjun
 * \date    2016-10-12
 */
AttrTreeWidget::~AttrTreeWidget()
{
    delete currentComBox;
    delete currentSpinBox;
//    qDeleteAll(attributeItemMap);
    qDebug() << "~AttrTreeWidget()";
}


/*如果来源为NULL则赋值为0*/
void AttrTreeWidget::setInputAttribute(ModuleBody *firstBody, int firstWireDot, ModuleBody *secondBody, int secondWireDot)
{
    if(secondBody == NULL)return;

    ElementType element = secondBody->element;
    AttributeID attr_type = (element == ELEMENT_BFU)?(AttributeID)(BfuInputA_Type+secondWireDot):
                            (element == ELEMENT_SBOX)?(SboxInput_Type):
                            (element == ELEMENT_BENES)?(AttributeID)(BenesInput0_Type+secondWireDot):
                            (element == ELEMENT_OUTMEM)?(AttributeID)(RCUMEM0_Type+secondWireDot):(AttributeID)(BCUOUTFIFO0_Type+secondWireDot);
    AttributeID attr_index = (element == ELEMENT_BFU)?(AttributeID)(BfuInputA_Index+secondWireDot):
                             (element == ELEMENT_SBOX)?(SboxInput_Index):
                             (element == ELEMENT_BENES)?(AttributeID)(BenesInput0_Index+secondWireDot):
                             (element == ELEMENT_OUTMEM)?(AttributeID)(RCUMEM0_Index+secondWireDot):(AttributeID)(BCUOUTFIFO0_Index+secondWireDot);

    if(firstBody == NULL)
    {
        xml->setElementAttribute(secondBody->bcuID,secondBody->rcuID,secondBody->element,
                                 secondBody->ID,attr_type,0);
        xml->setElementAttribute(secondBody->bcuID,secondBody->rcuID,secondBody->element,
                                 secondBody->ID,attr_index,0);
        showAttribue(element,secondBody->bcuID,secondBody->rcuID,secondBody->ID);
        return;
    }


    ElementType lelement = firstBody->element;

    int type_value = (lelement==ELEMENT_BFU&&firstBody->rcuID==secondBody->rcuID&&firstWireDot%2==0)?(1):
                     (lelement==ELEMENT_BFU&&firstBody->rcuID==secondBody->rcuID&&firstWireDot%2==1)?(2):
                     (lelement==ELEMENT_BFU&&firstBody->rcuID==secondBody->rcuID-1&&firstWireDot%2==0)?(5):
                 (lelement==ELEMENT_BFU&&firstBody->rcuID==secondBody->rcuID-1&&firstWireDot%2==1)?(6):
                 (lelement==ELEMENT_SBOX&&firstBody->rcuID==secondBody->rcuID)?(3):
                 (lelement==ELEMENT_SBOX&&firstBody->rcuID==secondBody->rcuID-1)?(7):
                 (lelement==ELEMENT_BENES&&firstBody->rcuID==secondBody->rcuID)?(4):
                 (lelement==ELEMENT_BENES&&firstBody->rcuID==secondBody->rcuID-1)?(8):
                 (lelement==ELEMENT_INFIFO)?9:10;
    int index_value = (lelement==ELEMENT_BFU)?firstBody->ID:
                      (lelement==ELEMENT_SBOX)?(firstWireDot+4*firstBody->ID):
                      (lelement==ELEMENT_BENES)?(firstWireDot):firstWireDot;
    if(firstBody->rcuID==3&&secondBody->rcuID==0&&firstBody->bcuID!=secondBody->bcuID)
    {
        type_value = (lelement==ELEMENT_BFU)?(5+(firstWireDot%2==1)):
                     (lelement==ELEMENT_SBOX)?7:
                     (lelement==ELEMENT_BENES)?8:0;
        xml->setElementAttribute(secondBody->bcuID,0,ELEMENT_BFU,0,BCUROUTER,firstBody->bcuID);
        xml->setElementAttribute(secondBody->bcuID,secondBody->rcuID,secondBody->element,
                                 secondBody->ID,attr_type,type_value);
        xml->setElementAttribute(secondBody->bcuID,secondBody->rcuID,secondBody->element,
                                 secondBody->ID,attr_index,index_value);

        showAttribue(element,secondBody->bcuID,secondBody->rcuID,secondBody->ID);
        emit updateUI();
    }
    else
    {
        //判断第二个模块的类型
        xml->setElementAttribute(secondBody->bcuID,secondBody->rcuID,secondBody->element,
                                 secondBody->ID,attr_type,type_value);
        xml->setElementAttribute(secondBody->bcuID,secondBody->rcuID,secondBody->element,
                                 secondBody->ID,attr_index,index_value);

        showAttribue(element,secondBody->bcuID,secondBody->rcuID,secondBody->ID);
    }
}

/**
 * AttrTreeWidget::initAttributeItemMap
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void AttrTreeWidget::initAttributeItemMap()
{
    //定位
    QTreeWidgetItem *idItem = new QTreeWidgetItem(QStringList(trUtf8("定位")));
    QTreeWidgetItem *bcuidItem = new QTreeWidgetItem(QStringList(trUtf8("BCU ID")));
    QTreeWidgetItem *rcuidItem = new QTreeWidgetItem(QStringList(trUtf8("RCU ID")));
    QTreeWidgetItem *bfuidItem = new QTreeWidgetItem(QStringList(trUtf8("BFU ID")));
    QTreeWidgetItem *sboxidItem = new QTreeWidgetItem(QStringList(trUtf8("SBOX ID")));
    attributeItemMap.insert(ID,idItem);
    attributeItemMap.insert(BCUID,bcuidItem);
    attributeItemMap.insert(RCUID,rcuidItem);
    attributeItemMap.insert(BFUID,bfuidItem);
    attributeItemMap.insert(SBOXID,sboxidItem);
    //位置
    QTreeWidgetItem *posItem = new QTreeWidgetItem(QStringList(trUtf8("位置坐标")));
    QTreeWidgetItem *posXItem = new QTreeWidgetItem(QStringList(trUtf8("X")));
    QTreeWidgetItem *posYItem = new QTreeWidgetItem(QStringList(trUtf8("Y")));
    posItem->addChild(posXItem);
    posItem->addChild(posYItem);
    attributeItemMap.insert(Pos,posItem);
    attributeItemMap.insert(PosX,posXItem);
    attributeItemMap.insert(PosY,posYItem);
    //属性
    QTreeWidgetItem *attrItem = new QTreeWidgetItem(QStringList(trUtf8("属性")));
    QTreeWidgetItem *bfuAttrItem = new QTreeWidgetItem(QStringList(trUtf8("BFU")));
    QTreeWidgetItem *sboxAttrItem = new QTreeWidgetItem(QStringList(trUtf8("SBOX")));
    QTreeWidgetItem *benesAttrItem = new QTreeWidgetItem(QStringList(trUtf8("BENES")));
    QTreeWidgetItem *rcumemAttrItem = new QTreeWidgetItem(QStringList(trUtf8("MEMORY")));
    QTreeWidgetItem *bcuoutAttrItem = new QTreeWidgetItem(QStringList(trUtf8("OUTFIFO")));

    attributeItemMap.insert(Attribute,attrItem);
    attributeItemMap.insert(BfuAttr,bfuAttrItem);
    attributeItemMap.insert(SboxAttr,sboxAttrItem);
    attributeItemMap.insert(BenesAttr,benesAttrItem);
    attributeItemMap.insert(RCUMEMAttr,rcumemAttrItem);
    attributeItemMap.insert(BCUOUTPUTAttr,bcuoutAttrItem);
    //bfu
    QTreeWidgetItem *bfuFuncItem = new QTreeWidgetItem(QStringList(trUtf8("算子")));
    QTreeWidgetItem *bfuFuncExpItem = new QTreeWidgetItem(QStringList(trUtf8("表达式")));
    QTreeWidgetItem *bfuAuModItem = new QTreeWidgetItem(QStringList(trUtf8("模")));
    QTreeWidgetItem *bfuBypassExpItem = new QTreeWidgetItem(QStringList(trUtf8("Bypass表达式")));
    QTreeWidgetItem *bfuBypassItem = new QTreeWidgetItem(QStringList(trUtf8("Bypass")));
    QTreeWidgetItem *bfuInputAItem = new QTreeWidgetItem(QStringList(trUtf8("输入A")));
    QTreeWidgetItem *bfuInputATypeItem = new QTreeWidgetItem(QStringList(trUtf8("类型")));
    QTreeWidgetItem *bfuInputAIndexItem = new QTreeWidgetItem(QStringList(trUtf8("选择")));
    bfuInputAItem->addChild(bfuInputATypeItem);
    bfuInputAItem->addChild(bfuInputAIndexItem);
    QTreeWidgetItem *bfuInputBItem = new QTreeWidgetItem(QStringList(trUtf8("输入B")));
    QTreeWidgetItem *bfuInputBTypeItem = new QTreeWidgetItem(QStringList(trUtf8("类型")));
    QTreeWidgetItem *bfuInputBIndexItem = new QTreeWidgetItem(QStringList(trUtf8("选择")));
    bfuInputBItem->addChild(bfuInputBTypeItem);
    bfuInputBItem->addChild(bfuInputBIndexItem);
    QTreeWidgetItem *bfuInputTItem = new QTreeWidgetItem(QStringList(trUtf8("输入T")));
    QTreeWidgetItem *bfuInputTTypeItem = new QTreeWidgetItem(QStringList(trUtf8("类型")));
    QTreeWidgetItem *bfuInputTIndexItem = new QTreeWidgetItem(QStringList(trUtf8("选择")));
    bfuInputTItem->addChild(bfuInputTTypeItem);
    bfuInputTItem->addChild(bfuInputTIndexItem);
    bfuAttrItem->addChild(bfuInputAItem);
    bfuAttrItem->addChild(bfuInputBItem);
    bfuAttrItem->addChild(bfuInputTItem);
    bfuAttrItem->addChild(bfuFuncItem);
    bfuAttrItem->addChild(bfuFuncExpItem);
    bfuAttrItem->addChild(bfuAuModItem);
    bfuAttrItem->setData(0,Qt::UserRole,1); //用此标记代表已经包含aumod
    bfuAttrItem->addChild(bfuBypassItem);
    attributeItemMap.insert(BfuFunc,bfuFuncItem);
    attributeItemMap.insert(BfuFuncExp,bfuFuncExpItem);
    attributeItemMap.insert(BfuAUMod,bfuAuModItem);
    attributeItemMap.insert(BfuBypassExp,bfuBypassExpItem);
    attributeItemMap.insert(BfuBypass,bfuBypassItem);
    attributeItemMap.insert(BfuInputA,bfuInputAItem);
    attributeItemMap.insert(BfuInputA_Type,bfuInputATypeItem);
    attributeItemMap.insert(BfuInputA_Index,bfuInputAIndexItem);
    attributeItemMap.insert(BfuInputB,bfuInputBItem);
    attributeItemMap.insert(BfuInputB_Type,bfuInputBTypeItem);
    attributeItemMap.insert(BfuInputB_Index,bfuInputBIndexItem);
    attributeItemMap.insert(BfuInputT,bfuInputTItem);
    attributeItemMap.insert(BfuInputT_Type,bfuInputTTypeItem);
    attributeItemMap.insert(BfuInputT_Index,bfuInputTIndexItem);
    //sbox
    QTreeWidgetItem *sboxInputItem = new QTreeWidgetItem(QStringList(trUtf8("输入")));
    QTreeWidgetItem *sboxInputTypeItem = new QTreeWidgetItem(QStringList(trUtf8("类型")));
    QTreeWidgetItem *sboxInputIndexItem = new QTreeWidgetItem(QStringList(trUtf8("选择")));
    sboxInputItem->addChild(sboxInputTypeItem);
    sboxInputItem->addChild(sboxInputIndexItem);
    QTreeWidgetItem *sboxCntItem = new QTreeWidgetItem(QStringList(trUtf8("Group")));
    QTreeWidgetItem *sboxCnt0Item = new QTreeWidgetItem(QStringList(trUtf8("Group0")));
    QTreeWidgetItem *sboxCnt1Item = new QTreeWidgetItem(QStringList(trUtf8("Group1")));
    QTreeWidgetItem *sboxCnt2Item = new QTreeWidgetItem(QStringList(trUtf8("Group2")));
    QTreeWidgetItem *sboxCnt3Item = new QTreeWidgetItem(QStringList(trUtf8("Group3")));
    QTreeWidgetItem *sboxModeItem = new QTreeWidgetItem(QStringList(trUtf8("模式")));
    QTreeWidgetItem *sboxSrcItem = new QTreeWidgetItem(QStringList(trUtf8("数据源")));
    sboxCntItem->addChild(sboxCnt0Item);
    sboxCntItem->addChild(sboxCnt1Item);
    sboxCntItem->addChild(sboxCnt2Item);
    sboxCntItem->addChild(sboxCnt3Item);
    sboxAttrItem->addChild(sboxInputItem);
    sboxAttrItem->addChild(sboxCntItem);
    sboxAttrItem->addChild(sboxModeItem);
    sboxAttrItem->addChild(sboxSrcItem);
    attributeItemMap.insert(SboxInput,sboxInputItem);
    attributeItemMap.insert(SboxInput_Type,sboxInputTypeItem);
    attributeItemMap.insert(SboxInput_Index,sboxInputIndexItem);
    attributeItemMap.insert(sboxCNT,sboxCntItem);
    attributeItemMap.insert(SboxCNT0,sboxCnt0Item);
    attributeItemMap.insert(SboxCNT1,sboxCnt1Item);
    attributeItemMap.insert(SboxCNT2,sboxCnt2Item);
    attributeItemMap.insert(SboxCNT3,sboxCnt3Item);
    attributeItemMap.insert(SboxMode,sboxModeItem);
    attributeItemMap.insert(SboxSrc,sboxSrcItem);
    //benes
    QTreeWidgetItem *benesSrcItem = new QTreeWidgetItem(QStringList(trUtf8("配置数据源")));
    QTreeWidgetItem *benesInput0Item = new QTreeWidgetItem(QStringList(trUtf8("输入0")));
    QTreeWidgetItem *benesInput0TypeItem = new QTreeWidgetItem(QStringList(trUtf8("类型")));
    QTreeWidgetItem *benesInput0IndexItem = new QTreeWidgetItem(QStringList(trUtf8("选择")));
    benesInput0Item->addChild(benesInput0TypeItem);
    benesInput0Item->addChild(benesInput0IndexItem);
    QTreeWidgetItem *benesInput1Item = new QTreeWidgetItem(QStringList(trUtf8("输入1")));
    QTreeWidgetItem *benesInput1TypeItem = new QTreeWidgetItem(QStringList(trUtf8("类型")));
    QTreeWidgetItem *benesInput1IndexItem = new QTreeWidgetItem(QStringList(trUtf8("选择")));
    benesInput1Item->addChild(benesInput1TypeItem);
    benesInput1Item->addChild(benesInput1IndexItem);
    QTreeWidgetItem *benesInput2Item = new QTreeWidgetItem(QStringList(trUtf8("输入2")));
    QTreeWidgetItem *benesInput2TypeItem = new QTreeWidgetItem(QStringList(trUtf8("类型")));
    QTreeWidgetItem *benesInput2IndexItem = new QTreeWidgetItem(QStringList(trUtf8("选择")));
    benesInput2Item->addChild(benesInput2TypeItem);
    benesInput2Item->addChild(benesInput2IndexItem);
    QTreeWidgetItem *benesInput3Item = new QTreeWidgetItem(QStringList(trUtf8("输入3")));
    QTreeWidgetItem *benesInput3TypeItem = new QTreeWidgetItem(QStringList(trUtf8("类型")));
    QTreeWidgetItem *benesInput3IndexItem = new QTreeWidgetItem(QStringList(trUtf8("选择")));
    benesInput3Item->addChild(benesInput3TypeItem);
    benesInput3Item->addChild(benesInput3IndexItem);
    benesAttrItem->addChild(benesInput0Item);
    benesAttrItem->addChild(benesInput1Item);
    benesAttrItem->addChild(benesInput2Item);
    benesAttrItem->addChild(benesInput3Item);
    benesAttrItem->addChild(benesSrcItem);
    attributeItemMap.insert(BenesInput0,benesInput0Item);
    attributeItemMap.insert(BenesInput0_Type,benesInput0TypeItem);
    attributeItemMap.insert(BenesInput0_Index,benesInput0IndexItem);
    attributeItemMap.insert(BenesInput1,benesInput1Item);
    attributeItemMap.insert(BenesInput1_Type,benesInput1TypeItem);
    attributeItemMap.insert(BenesInput1_Index,benesInput1IndexItem);
    attributeItemMap.insert(BenesInput2,benesInput2Item);
    attributeItemMap.insert(BenesInput2_Type,benesInput2TypeItem);
    attributeItemMap.insert(BenesInput2_Index,benesInput2IndexItem);
    attributeItemMap.insert(BenesInput3,benesInput3Item);
    attributeItemMap.insert(BenesInput3_Type,benesInput3TypeItem);
    attributeItemMap.insert(BenesInput3_Index,benesInput3IndexItem);
    attributeItemMap.insert(BenesSrc,benesSrcItem);

    //rcu_mem
    QTreeWidgetItem *rcumem0Item = new QTreeWidgetItem(QStringList(trUtf8("MEMORY输入0")));
    QTreeWidgetItem *rcumem0TypeItem = new QTreeWidgetItem(QStringList(trUtf8("类型")));
    QTreeWidgetItem *rcumem0IndexItem = new QTreeWidgetItem(QStringList(trUtf8("选择")));
    rcumem0Item->addChild(rcumem0TypeItem);
    rcumem0Item->addChild(rcumem0IndexItem);
    QTreeWidgetItem *rcumem1Item = new QTreeWidgetItem(QStringList(trUtf8("MEMORY输入1")));
    QTreeWidgetItem *rcumem1TypeItem = new QTreeWidgetItem(QStringList(trUtf8("类型")));
    QTreeWidgetItem *rcumem1IndexItem = new QTreeWidgetItem(QStringList(trUtf8("选择")));
    rcumem1Item->addChild(rcumem1TypeItem);
    rcumem1Item->addChild(rcumem1IndexItem);
    QTreeWidgetItem *rcumem2Item = new QTreeWidgetItem(QStringList(trUtf8("MEMORY输入2")));
    QTreeWidgetItem *rcumem2TypeItem = new QTreeWidgetItem(QStringList(trUtf8("类型")));
    QTreeWidgetItem *rcumem2IndexItem = new QTreeWidgetItem(QStringList(trUtf8("选择")));
    rcumem2Item->addChild(rcumem2TypeItem);
    rcumem2Item->addChild(rcumem2IndexItem);
    QTreeWidgetItem *rcumem3Item = new QTreeWidgetItem(QStringList(trUtf8("MEMORY输入3")));
    QTreeWidgetItem *rcumem3TypeItem = new QTreeWidgetItem(QStringList(trUtf8("类型")));
    QTreeWidgetItem *rcumem3IndexItem = new QTreeWidgetItem(QStringList(trUtf8("选择")));
    rcumem3Item->addChild(rcumem3TypeItem);
    rcumem3Item->addChild(rcumem3IndexItem);
    rcumemAttrItem->addChild(rcumem0Item);
    rcumemAttrItem->addChild(rcumem1Item);
    rcumemAttrItem->addChild(rcumem2Item);
    rcumemAttrItem->addChild(rcumem3Item);
    attributeItemMap.insert(RCUMEM0,rcumem0Item);
    attributeItemMap.insert(RCUMEM0_Type,rcumem0TypeItem);
    attributeItemMap.insert(RCUMEM0_Index,rcumem0IndexItem);
    attributeItemMap.insert(RCUMEM1,rcumem1Item);
    attributeItemMap.insert(RCUMEM1_Type,rcumem1TypeItem);
    attributeItemMap.insert(RCUMEM1_Index,rcumem1IndexItem);
    attributeItemMap.insert(RCUMEM2,rcumem2Item);
    attributeItemMap.insert(RCUMEM2_Type,rcumem2TypeItem);
    attributeItemMap.insert(RCUMEM2_Index,rcumem2IndexItem);
    attributeItemMap.insert(RCUMEM3,rcumem3Item);
    attributeItemMap.insert(RCUMEM3_Type,rcumem3TypeItem);
    attributeItemMap.insert(RCUMEM3_Index,rcumem3IndexItem);
    //bcu_out
    QTreeWidgetItem *bcurouterItem = new QTreeWidgetItem(QStringList(trUtf8("BCU数据来源")));
    QTreeWidgetItem *bcuout0Item = new QTreeWidgetItem(QStringList(trUtf8("OUTFIFO输入0")));
    QTreeWidgetItem *bcuout0TypeItem = new QTreeWidgetItem(QStringList(trUtf8("类型")));
    QTreeWidgetItem *bcuout0IndexItem = new QTreeWidgetItem(QStringList(trUtf8("选择")));
    bcuout0Item->addChild(bcuout0TypeItem);
    bcuout0Item->addChild(bcuout0IndexItem);
    QTreeWidgetItem *bcuout1Item = new QTreeWidgetItem(QStringList(trUtf8("OUTFIFO输入1")));
    QTreeWidgetItem *bcuout1TypeItem = new QTreeWidgetItem(QStringList(trUtf8("类型")));
    QTreeWidgetItem *bcuout1IndexItem = new QTreeWidgetItem(QStringList(trUtf8("选择")));
    bcuout1Item->addChild(bcuout1TypeItem);
    bcuout1Item->addChild(bcuout1IndexItem);
    QTreeWidgetItem *bcuout2Item = new QTreeWidgetItem(QStringList(trUtf8("OUTFIFO输入2")));
    QTreeWidgetItem *bcuout2TypeItem = new QTreeWidgetItem(QStringList(trUtf8("类型")));
    QTreeWidgetItem *bcuout2IndexItem = new QTreeWidgetItem(QStringList(trUtf8("选择")));
    bcuout2Item->addChild(bcuout2TypeItem);
    bcuout2Item->addChild(bcuout2IndexItem);
    QTreeWidgetItem *bcuout3Item = new QTreeWidgetItem(QStringList(trUtf8("OUTFIFO输入3")));
    QTreeWidgetItem *bcuout3TypeItem = new QTreeWidgetItem(QStringList(trUtf8("类型")));
    QTreeWidgetItem *bcuout3IndexItem = new QTreeWidgetItem(QStringList(trUtf8("选择")));
    bcuout3Item->addChild(bcuout3TypeItem);
    bcuout3Item->addChild(bcuout3IndexItem);
    bcuoutAttrItem->addChild(bcuout0Item);
    bcuoutAttrItem->addChild(bcuout1Item);
    bcuoutAttrItem->addChild(bcuout2Item);
    bcuoutAttrItem->addChild(bcuout3Item);
    attrItem->addChild(bcurouterItem);
    attributeItemMap.insert(BCUOUTFIFO0,bcuout0Item);
    attributeItemMap.insert(BCUOUTFIFO0_Type,bcuout0TypeItem);
    attributeItemMap.insert(BCUOUTFIFO0_Index,bcuout0IndexItem);
    attributeItemMap.insert(BCUOUTFIFO1,bcuout1Item);
    attributeItemMap.insert(BCUOUTFIFO1_Type,bcuout1TypeItem);
    attributeItemMap.insert(BCUOUTFIFO1_Index,bcuout1IndexItem);
    attributeItemMap.insert(BCUOUTFIFO2,bcuout2Item);
    attributeItemMap.insert(BCUOUTFIFO2_Type,bcuout2TypeItem);
    attributeItemMap.insert(BCUOUTFIFO2_Index,bcuout2IndexItem);
    attributeItemMap.insert(BCUOUTFIFO3,bcuout3Item);
    attributeItemMap.insert(BCUOUTFIFO3_Type,bcuout3TypeItem);
    attributeItemMap.insert(BCUOUTFIFO3_Index,bcuout3IndexItem);
    attributeItemMap.insert(BCUROUTER,bcurouterItem);
    //TH_MEM_WT
    QTreeWidgetItem *bcumemWTItem = new QTreeWidgetItem(QStringList(trUtf8("Memery WRITE")));
    QTreeWidgetItem *bcumemWTModeItem = new QTreeWidgetItem(QStringList(trUtf8("模式")));
    QTreeWidgetItem *bcumemWTAddrItem = new QTreeWidgetItem(QStringList(trUtf8("起始地址(0x)")));
    QTreeWidgetItem *bcumemWTMaskItem = new QTreeWidgetItem(QStringList(trUtf8("数据掩盖(0b)")));
    QTreeWidgetItem *bcumemWTOffsetItem = new QTreeWidgetItem(QStringList(trUtf8("偏移(0x)")));
    QTreeWidgetItem *bcumemWTThreasholdItem = new QTreeWidgetItem(QStringList(trUtf8("阈值")));
    bcumemWTItem->addChild(bcumemWTModeItem);
    bcumemWTItem->addChild(bcumemWTAddrItem);
    bcumemWTItem->addChild(bcumemWTMaskItem);
    bcumemWTItem->addChild(bcumemWTOffsetItem);
    bcumemWTItem->addChild(bcumemWTThreasholdItem);
    attributeItemMap.insert(RCUMEMWT,bcumemWTItem);
    attributeItemMap.insert(RCUMEMWTMODE,bcumemWTModeItem);
    attributeItemMap.insert(RCUMEMWTADDR,bcumemWTAddrItem);
    attributeItemMap.insert(RCUMEMWTMASK,bcumemWTMaskItem);
    attributeItemMap.insert(RCUMEMWTOFFSET,bcumemWTOffsetItem);
    attributeItemMap.insert(RCUMEMWTTHREASHOLD,bcumemWTThreasholdItem);
    //TH_MEM_RD
    QTreeWidgetItem *bcumemRDItem = new QTreeWidgetItem(QStringList(trUtf8("Memery READ")));
    QTreeWidgetItem *bcumemRDMode128_32Item = new QTreeWidgetItem(QStringList(trUtf8("128/32选择")));
    QTreeWidgetItem *bcumemRDModeItem = new QTreeWidgetItem(QStringList(trUtf8("模式")));
    QTreeWidgetItem *bcumemRDAddr1Item = new QTreeWidgetItem(QStringList(trUtf8("起始地址1(0x)")));
    QTreeWidgetItem *bcumemRDAddr2Item = new QTreeWidgetItem(QStringList(trUtf8("起始地址2(0x)")));
    QTreeWidgetItem *bcumemRDAddr3Item = new QTreeWidgetItem(QStringList(trUtf8("起始地址3(0x)")));
    QTreeWidgetItem *bcumemRDAddr4Item = new QTreeWidgetItem(QStringList(trUtf8("起始地址4(0x)")));
    QTreeWidgetItem *bcumemRDOffsetItem = new QTreeWidgetItem(QStringList(trUtf8("偏移(0x)")));
    QTreeWidgetItem *bcumemRDThreasholdItem = new QTreeWidgetItem(QStringList(trUtf8("阈值")));
    bcumemRDItem->addChild(bcumemRDMode128_32Item);
    bcumemRDItem->addChild(bcumemRDModeItem);
    bcumemRDItem->addChild(bcumemRDAddr1Item);
    bcumemRDItem->addChild(bcumemRDAddr2Item);
    bcumemRDItem->addChild(bcumemRDAddr3Item);
    bcumemRDItem->addChild(bcumemRDAddr4Item);
    bcumemRDItem->addChild(bcumemRDOffsetItem);
    bcumemRDItem->addChild(bcumemRDThreasholdItem);
    attributeItemMap.insert(RCUMEMRD,bcumemRDItem);
    attributeItemMap.insert(RCUMEMRDMODE128_32,bcumemRDMode128_32Item);
    attributeItemMap.insert(RCUMEMRDMODE,bcumemRDModeItem);
    attributeItemMap.insert(RCUMEMRDADDR1,bcumemRDAddr1Item);
    attributeItemMap.insert(RCUMEMRDADDR2,bcumemRDAddr2Item);
    attributeItemMap.insert(RCUMEMRDADDR3,bcumemRDAddr3Item);
    attributeItemMap.insert(RCUMEMRDADDR4,bcumemRDAddr4Item);
    attributeItemMap.insert(RCUMEMRDOFFSET,bcumemRDOffsetItem);
    attributeItemMap.insert(RCUMEMRDTHREASHOLD,bcumemRDThreasholdItem);
}
/**
 * AttrTreeWidget::textEdit
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void AttrTreeWidget::textEdit()
{
    QString s = ((QLineEdit*)(itemWidget(attributeItemMap.value(currentAttributeID),1)))->text();
    attributeItemMap.value(currentAttributeID)->setText(1,s);
    xml->setElementAttributeStr(currentBCU,currentRCU,currentElement,currentID,currentAttributeID,s);
    removeItemWidget(attributeItemMap.value(currentAttributeID),1);
//    xml->save();
}

/**
 * AttrTreeWidget::getFuncAndUpdate
 * \brief   
 * \param   s
 * \param   b
 * \author  zhangjun
 * \date    2016-10-12
 */
void AttrTreeWidget::getFuncAndUpdate(QString s, QString b)
{
    switch(attributeItemMap.value(BfuAttr)->data(0,Qt::UserRole).toUInt())
    {
    case 0:
    {
        if(s == "au")
        {
            attributeItemMap.value(BfuAttr)->addChild(attributeItemMap.value(BfuAUMod));
            attributeItemMap.value(BfuAttr)->setData(0,Qt::UserRole,1);
        }
        else if(s == "su" && b == BYPASS_NB)
        {
            attributeItemMap.value(BfuAttr)->addChild(attributeItemMap.value(BfuBypassExp));
            attributeItemMap.value(BfuAttr)->setData(0,Qt::UserRole,2);
        }
        break;
    }
    case 1:
    {
        if(s == "su" && b == BYPASS_NB)
        {
            attributeItemMap.value(BfuAttr)->removeChild(attributeItemMap.value(BfuAUMod));
            attributeItemMap.value(BfuAttr)->addChild(attributeItemMap.value(BfuBypassExp));
            attributeItemMap.value(BfuAttr)->setData(0,Qt::UserRole,2);
        }
        else if(s != "au")
        {
            attributeItemMap.value(BfuAttr)->removeChild(attributeItemMap.value(BfuAUMod));
            attributeItemMap.value(BfuAttr)->setData(0,Qt::UserRole,0);
        }
        break;
    }
    case 2:
    {
        if(s == "lu" || (s == "su" && b != BYPASS_NB))
        {
            attributeItemMap.value(BfuAttr)->removeChild(attributeItemMap.value(BfuBypassExp));
            attributeItemMap.value(BfuAttr)->setData(0,Qt::UserRole,0);
        }
        else if(s == "au")
        {
            attributeItemMap.value(BfuAttr)->removeChild(attributeItemMap.value(BfuBypassExp));
            attributeItemMap.value(BfuAttr)->addChild(attributeItemMap.value(BfuAUMod));
            attributeItemMap.value(BfuAttr)->setData(0,Qt::UserRole,1);
        }
        if(b == BYPASS_NB && (s == "lu" || s == "au"))
        {
            xml->setElementAttribute(currentBCU,currentRCU,ELEMENT_BFU,currentID,BfuBypass,0);
            attributeItemMap.value(BfuBypass)->setText(1,BYPASS_A);
            xml->setElementAttributeStr(currentBCU,currentRCU,ELEMENT_BFU,currentID,BfuBypassExp,"");
        }
        break;
    }
    default:break;
    }
}


/**
 * AttrTreeWidget::comboxSelected
 * \brief   
 * \param   s
 * \author  zhangjun
 * \date    2016-10-12
 */
void AttrTreeWidget::comboxSelected(QString s)
{
    //判断是否选择了func
    if(currentAttributeID == BfuFunc)
    {
        getFuncAndUpdate(s,attributeItemMap.value(BfuBypass)->text(1));
    }
    if(currentAttributeID == BfuBypass)
    {
        getFuncAndUpdate(attributeItemMap.value(BfuFunc)->text(1),s);
    }
    //显示
    attributeItemMap.value(currentAttributeID)->setText(1,s);
    removeItemWidget(attributeItemMap.value(currentAttributeID),1);
}
/**
 * AttrTreeWidget::updateMemRDTh
 * \brief   
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void AttrTreeWidget::updateMemRDTh(int index)
{
    QTreeWidgetItem *item = attributeItemMap.value(RCUMEMRDTHREASHOLD);
    if(index == 3)
    {
        item->setText(0,trUtf8("边界地址"));
        if(!item->text(1).contains("0x"))item->setText(1,"0x"+QString::number(item->text(1).toInt(0,10),16));
    }
    else
    {
        item->setText(0,trUtf8("读写次数"));
        if(item->text(1).contains("0x"))item->setText(1,QString::number(item->text(1).toInt(0,16),10));
    }
}

/**
 * AttrTreeWidget::updateMemWTTh
 * \brief   
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void AttrTreeWidget::updateMemWTTh(int index)
{
    QTreeWidgetItem *item = attributeItemMap.value(RCUMEMWTTHREASHOLD);
    if(index == 3)
    {
        item->setText(0,trUtf8("边界地址"));
        if(!item->text(1).contains("0x"))item->setText(1,"0x"+QString::number(item->text(1).toInt(0,10),16));
    }
    else
    {
        item->setText(0,trUtf8("读写次数"));
        if(item->text(1).contains("0x"))item->setText(1,QString::number(item->text(1).toInt(0,16),10));
    }
}

/**
 * AttrTreeWidget::comboxSelected
 * \brief   
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void AttrTreeWidget::comboxSelected(int index)
{
    //判断是否选择了mem_mode
    if(currentAttributeID == RCUMEMRDMODE)
    {
        updateMemRDTh(index);
    }
    else if(currentAttributeID == RCUMEMWTMODE)
    {
        updateMemWTTh(index);
    }

    //保存
    xml->setElementAttribute(currentBCU,currentRCU,currentElement,currentID,currentAttributeID,index);
//    xml->save();
    emit updateUI();
}
/**
 * AttrTreeWidget::onToolButtonClick
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void AttrTreeWidget::onToolButtonClick()
{
    QDir temp_d(xml->m_curFile);
    QString temp_dir = temp_d.absolutePath();
    temp_dir = temp_dir.left(temp_dir.lastIndexOf("/")+1);

    QString curFilename = xml->getElementAttributeStr(currentBCU,currentRCU,currentElement,currentID,currentAttributeID);
    QString filename = QFileDialog::getOpenFileName(this,trUtf8("请选择数据文件路径"),temp_dir+curFilename,trUtf8("文本文档(*.txt)"));
    if(!filename.isEmpty())
    {
        //modified by zhangjun at 2016/9/7
        QDir dir(temp_dir);
        filename = dir.relativeFilePath(filename);
//        QDir *dir = new QDir(temp_dir);
//        filename = dir->relativeFilePath(filename);
        xml->setElementAttributeStr(currentBCU,currentRCU,currentElement,currentID,currentAttributeID,filename);
        attributeItemMap.value(currentAttributeID)->setText(1,filename);
    }
    removeItemWidget(attributeItemMap.value(currentAttributeID),1);
}

/**
 * AttrTreeWidget::onHexSpinboxValueChanged
 * \brief   
 * \param   v
 * \author  zhangjun
 * \date    2016-10-12
 */
void AttrTreeWidget::onHexSpinboxValueChanged(int v)
{
    attributeItemMap.value(currentAttributeID)->setText(1,"0x"+QString::number(v,16));
    xml->setElementAttribute(currentBCU,currentRCU,currentElement,currentID,currentAttributeID,v);
//    removeItemWidget(attributeItemMap.value(currentAttributeID),1);
//    xml->save();
}
/**
 * AttrTreeWidget::onSpinboxValueChanged
 * \brief   
 * \param   v
 * \author  zhangjun
 * \date    2016-10-12
 */
void AttrTreeWidget::onSpinboxValueChanged(int v)
{
    attributeItemMap.value(currentAttributeID)->setText(1,QString::number(v));
    xml->setElementAttribute(currentBCU,currentRCU,currentElement,currentID,currentAttributeID,v);
//        removeItemWidget(attributeItemMap.value(currentAttributeID),1);
//    xml->save();z
}
/**
 * AttrTreeWidget::setPosAttribute
 * \brief   
 * \param   pos
 * \author  zhangjun
 * \date    2016-10-12
 */
void AttrTreeWidget::setPosAttribute(QPointF pos)
{
    xml->setElementAttribute(currentBCU,currentRCU,currentElement,currentID,PosX,pos.x());
    xml->setElementAttribute(currentBCU,currentRCU,currentElement,currentID,PosY,pos.y());
    attributeItemMap.value(PosX)->setText(1,QString::number(pos.x()));
    attributeItemMap.value(PosY)->setText(1,QString::number(pos.y()));
}

/**
 * AttrTreeWidget::initAttributeValue
 * \brief   
 * \param   element
 * \param   bcu
 * \param   rcu
 * \param   id
 * \author  zhangjun
 * \date    2016-10-12
 */
void AttrTreeWidget::initAttributeValue(ElementType element, int bcu, int rcu, int id)
{
#define N(x) QString::number(x)
#define ADDITEM(x) addTopLevelItem(attributeItemMap.value(x));
#define ADDCHILD(x,y) attributeItemMap.value(x)->addChild(attributeItemMap.value(y));
#define REMOVECHILED(x,y) attributeItemMap.value(x)->removeChild(attributeItemMap.value(y));
#define EQUALVALUE(x,n) attributeItemMap.value(x)->setText(1,QString::number(n));
#define EQUALVALUESTR(x,s) attributeItemMap.value(x)->setText(1,s);
#define EQUAL(x) attributeItemMap.value(x)->setText(1,QString::number\
    (xml->getElementAttributeValue(bcu,rcu,element,id,x)));
#define EQUALSTR(x) attributeItemMap.value(x)->setText(1,\
    (xml->getElementAttributeStr(bcu,rcu,element,id,x)));
    switch(element)
    {
    case ELEMENT_NULL:
    {
        REMOVECHILED(Attribute,SboxAttr);
        REMOVECHILED(Attribute,BenesAttr);
        REMOVECHILED(Attribute,BfuAttr);
    }
    case ELEMENT_BFU:
    {
        REMOVECHILED(Attribute,SboxAttr);
        REMOVECHILED(Attribute,BenesAttr);
        REMOVECHILED(ID,SBOXID);
        ADDITEM(ID);
        ADDCHILD(ID,BCUID);
        ADDCHILD(ID,RCUID);
        ADDCHILD(ID,BFUID);
        ADDITEM(Pos);
        ADDITEM(Attribute);
        ADDCHILD(Attribute,BCUOUTPUTAttr);
        ADDCHILD(Attribute,RCUMEMRD);
        ADDCHILD(Attribute,RCUMEMWT);
        ADDCHILD(Attribute,RCUMEMAttr);
        ADDCHILD(Attribute,BfuAttr);
        EQUALVALUE(BCUID,bcu);
        EQUALVALUE(RCUID,rcu);
        EQUALVALUE(BFUID,id);
        EQUALSTR(BfuFunc);
        EQUALSTR(BfuFuncExp);
        EQUALSTR(BfuAUMod);
        EQUALSTR(BfuBypassExp);
        EQUALSTR(BfuBypass);
        EQUALSTR(BfuInputA_Type);
        EQUAL(BfuInputA_Index);
        EQUALSTR(BfuInputB_Type);
        EQUAL(BfuInputB_Index);
        EQUALSTR(BfuInputT_Type);
        EQUAL(BfuInputT_Index);
        EQUALSTR(RCUMEM0_Type);
        getFuncAndUpdate(attributeItemMap.value(BfuFunc)->text(1),attributeItemMap.value(BfuBypass)->text(1));
        break;
    }
    case ELEMENT_SBOX:
    {
//        this->clear();
        REMOVECHILED(Attribute,BfuAttr);
        REMOVECHILED(Attribute,BenesAttr);
        REMOVECHILED(ID,BFUID);
        ADDITEM(ID);
        ADDCHILD(ID,BCUID);
        ADDCHILD(ID,RCUID);
        ADDCHILD(ID,SBOXID);
        ADDITEM(Pos);
        ADDITEM(Attribute);
        ADDCHILD(Attribute,BCUOUTPUTAttr);
        ADDCHILD(Attribute,RCUMEMRD);
        ADDCHILD(Attribute,RCUMEMWT);
        ADDCHILD(Attribute,RCUMEMAttr);
        ADDCHILD(Attribute,SboxAttr);
        EQUALVALUE(BCUID,bcu);
        EQUALVALUE(RCUID,rcu);
        EQUALVALUE(SBOXID,id);
        EQUALSTR(SboxInput_Type);
        EQUAL(SboxInput_Index);
        EQUAL(SboxCNT0);
        EQUAL(SboxCNT1);
        EQUAL(SboxCNT2);
        EQUAL(SboxCNT3);
        EQUALSTR(SboxMode);
        EQUALSTR(SboxSrc);
        break;
    }
    case ELEMENT_BENES:
    {
//        this->clear();
        REMOVECHILED(Attribute,BfuAttr);
        REMOVECHILED(Attribute,SboxAttr);
        REMOVECHILED(ID,BFUID);
        REMOVECHILED(ID,SBOXID);
        ADDITEM(ID);
        ADDCHILD(ID,BCUID);
        ADDCHILD(ID,RCUID);
        ADDITEM(Pos);
        ADDITEM(Attribute);
        ADDCHILD(Attribute,BCUOUTPUTAttr);
        ADDCHILD(Attribute,RCUMEMRD);
        ADDCHILD(Attribute,RCUMEMWT);
        ADDCHILD(Attribute,RCUMEMAttr);
        ADDCHILD(Attribute,BenesAttr);
        EQUALVALUE(BCUID,bcu);
        EQUALVALUE(RCUID,rcu);
        EQUALSTR(BenesSrc);
        EQUALSTR(BenesInput0_Type);
        EQUAL(BenesInput0_Index);
        EQUALSTR(BenesInput1_Type);
        EQUAL(BenesInput1_Index);
        EQUALSTR(BenesInput2_Type);
        EQUAL(BenesInput2_Index);
        EQUALSTR(BenesInput3_Type);
        EQUAL(BenesInput3_Index);
        break;
    }
    default:break;
    }
    if(element <= ELEMENT_BENES)
    {
        //坐标
        EQUAL(PosX);
        EQUAL(PosY);
        //rcu mem
        EQUAL(RCUMEM0_Index);
        EQUALSTR(RCUMEM1_Type);
        EQUAL(RCUMEM1_Index);
        EQUALSTR(RCUMEM2_Type);
        EQUAL(RCUMEM2_Index);
        EQUALSTR(RCUMEM3_Type);
        EQUAL(RCUMEM3_Index);
        //bcu out fifo
        EQUALSTR(BCUOUTFIFO0_Type);
        EQUAL(BCUOUTFIFO0_Index);
        EQUALSTR(BCUOUTFIFO1_Type);
        EQUAL(BCUOUTFIFO1_Index);
        EQUALSTR(BCUOUTFIFO2_Type);
        EQUAL(BCUOUTFIFO2_Index);
        EQUALSTR(BCUOUTFIFO3_Type);
        EQUAL(BCUOUTFIFO3_Index);
        EQUAL(BCUROUTER);
        //THMEM
        EQUALSTR(RCUMEMWTMODE);
        EQUALSTR(RCUMEMWTADDR);
        EQUALSTR(RCUMEMWTMASK);
        EQUALSTR(RCUMEMWTOFFSET);
        EQUALSTR(RCUMEMWTTHREASHOLD);
        EQUALSTR(RCUMEMRDMODE128_32);
        EQUALSTR(RCUMEMRDMODE);
        EQUALSTR(RCUMEMRDADDR1);
        EQUALSTR(RCUMEMRDADDR2);
        EQUALSTR(RCUMEMRDADDR3);
        EQUALSTR(RCUMEMRDADDR4);
        EQUALSTR(RCUMEMRDOFFSET);
        EQUALSTR(RCUMEMRDTHREASHOLD);
        updateMemRDTh(xml->m_memModeMap.values().indexOf(attributeItemMap.value(RCUMEMRDMODE)->text(1)));
        updateMemWTTh(xml->m_memModeMap.values().indexOf(attributeItemMap.value(RCUMEMWTMODE)->text(1)));
    }
}

/**
 * AttrTreeWidget::itemClickedSlot
 * \brief   
 * \param   item
 * \param   column
 * \author  zhangjun
 * \date    2016-10-12
 */
void AttrTreeWidget::itemClickedSlot(QTreeWidgetItem *item, int column)
{
#define CASE(x)  else if(item==attributeItemMap.value(x))
#define ITEM(x) attributeItemMap.value(x)
#define STARTADDCOMBOX \
        QComboBox* combox = new QComboBox();
#define ENDADDCOMBOX(x) \
        combox->setCurrentText(ITEM(x)->text(1));\
        setItemWidget(ITEM(x),1,combox);\
        connect(combox,SIGNAL(activated(QString)),this,SLOT(comboxSelected(QString)));\
        connect(combox,SIGNAL(activated(int)),this,SLOT(comboxSelected(int)));\
        currentAttributeID = x;

#define STARTADDTEXTEDIT(x) \
        QLineEdit *edit = new QLineEdit(ITEM(x)->text(1));
#define ENDADDTEXTEDIT(x) \
        setItemWidget(ITEM(x),1,edit);\
        connect(edit,SIGNAL(editingFinished()),this,SLOT(textEdit()));\
        currentAttributeID = x;

#define STARTADDTOOLBUTTON(x) \
        QToolButton *button = new QToolButton();\
        setItemWidget(ITEM(x),1,button);\
        connect(button,SIGNAL(clicked()),this,SLOT(onToolButtonClick()));\
        currentAttributeID = x;

#define STARTADDHEXSPINBOX(n) \
    HexSpinBox *spin = new HexSpinBox(n);
#define STARTADDSPINBOX(n) \
    QSpinBox *spin = new QSpinBox();\
    spin->setMaximum(n);
#define ENDADDSPINBBOX(x) \
    spin->setValue(ITEM(x)->text(1).toInt(0,10));\
    setItemWidget(ITEM(x),1,spin);\
    connect(spin,SIGNAL(valueChanged(int)),this,SLOT(onSpinboxValueChanged(int)));\
    currentAttributeID = x;
#define ENDADDHEXSPINBBOX(x) \
    spin->setValue(ITEM(x)->text(1).toInt(0,16));\
    setItemWidget(ITEM(x),1,spin);\
    connect(spin,SIGNAL(valueChanged(int)),this,SLOT(onHexSpinboxValueChanged(int)));\
    currentAttributeID = x;



    //清除itemwidget
//    disconnect(itemWidget(ITEM(currentAttributeID),1));
    removeItemWidget(ITEM(currentAttributeID),1);
    if(item == NULL || column != 1)return;
    CASE(BfuFuncExp)
    {
        STARTADDTEXTEDIT(BfuFuncExp);
        QRegExp regx("[ ABT+&|^~!()<>0-9]*");
        QValidator *validator = new QRegExpValidator(regx);
        edit->setValidator(validator);
        ENDADDTEXTEDIT(BfuFuncExp);
    }
    CASE(BfuFunc)
    {
        STARTADDCOMBOX;
        combox->addItem(FUNC_AU);
        combox->addItem(FUNC_LU);
        combox->addItem(FUNC_SU);
        ENDADDCOMBOX(BfuFunc)
    }
    CASE(BfuAUMod)
    {
        STARTADDCOMBOX;
        for(int i = 0; i<4; i++)
        {
            combox->addItem(xml->m_auModMap.value(i));
        }
        ENDADDCOMBOX(BfuAUMod)
    }
    CASE(BfuBypassExp)
    {
        STARTADDTEXTEDIT(BfuBypassExp);
        QRegExp regx("[ BT+&|^~!()<>0-9]*");
        QValidator *validator = new QRegExpValidator(regx);
        edit->setValidator(validator);
        ENDADDTEXTEDIT(BfuBypassExp);
    }

    CASE(BfuBypass)
    {
        STARTADDCOMBOX;
        int count = 3;
        if(attributeItemMap.value(BfuFunc)->text(1) == "su")count = 4;
        for(int i = 0; i<count; i++)
        {
            combox->addItem(xml->m_bypassMap.value(i));
        }
        ENDADDCOMBOX(BfuBypass)
    }
#define CASEINPUTTYPE(x) CASE(x##_Type)\
    {\
        STARTADDCOMBOX;\
        combox->addItem(EMPTYCHOICE);\
        combox->addItem(CURBFUX);\
        combox->addItem(CURBFUY);\
        combox->addItem(CURSBOX);\
        combox->addItem(CURBENES);\
        combox->addItem(LASTBFUX);\
        combox->addItem(LASTBFUY);\
        combox->addItem(LASTSBOX);\
        combox->addItem(LASTBENES);\
        combox->addItem(INFIFO);\
        combox->addItem(MEMOUT);\
        ENDADDCOMBOX(x##_Type);\
    }

#define CASEINPUTINDEX(x) CASE(x##_Index)\
    {\
        STARTADDCOMBOX;\
        int tempNum = 0;\
        QString tempsel = ITEM(x##_Type)->text(1);\
        if(tempsel == CURBFUX || tempsel == CURBFUY || tempsel == LASTBFUX || tempsel == LASTBFUY)\
        {\
            tempNum = 8;\
        }\
        else if(tempsel == CURSBOX || tempsel == LASTSBOX)\
        {\
            tempNum = 16;\
        }\
        else if(tempsel == CURBENES || tempsel == LASTBENES || tempsel == INFIFO || tempsel == MEMOUT)\
        {\
            tempNum = 4;\
        }\
        for(int i = 0; i<tempNum; i++)\
        {\
            combox->addItem(QString::number(i));\
        }\
        ENDADDCOMBOX(x##_Index);\
    }

#define CASEINPUT(x) \
    CASEINPUTTYPE(x)\
    CASEINPUTINDEX(x)

    CASEINPUT(BfuInputA)
    CASEINPUT(BfuInputB)
    CASEINPUT(BfuInputT)
    CASEINPUT(SboxInput)
    CASEINPUT(BenesInput0)
    CASEINPUT(BenesInput1)
    CASEINPUT(BenesInput2)
    CASEINPUT(BenesInput3)
    CASEINPUT(RCUMEM0)
    CASEINPUT(RCUMEM1)
    CASEINPUT(RCUMEM2)
    CASEINPUT(RCUMEM3)


#define CASEFIFOINPUTTYPE(x) CASE(x##_Type)\
    {\
        STARTADDCOMBOX;\
        combox->addItem(EMPTYCHOICE);\
        combox->addItem(CURBFUX);\
        combox->addItem(CURBFUY);\
        combox->addItem(CURSBOX);\
        combox->addItem(CURBENES);\
        ENDADDCOMBOX(x##_Type);\
    }
#define CASEFIFOINPUT(x) \
        CASEFIFOINPUTTYPE(x) \
        CASEINPUTINDEX(x)

    CASEFIFOINPUT(BCUOUTFIFO0)
    CASEFIFOINPUT(BCUOUTFIFO1)
    CASEFIFOINPUT(BCUOUTFIFO2)
    CASEFIFOINPUT(BCUOUTFIFO3)
    CASE(BCUROUTER)
    {
        STARTADDCOMBOX;
        for(int i = 0; i<BCUMAX; i++)
        {
            combox->addItem(QString::number(i));
        }
        ENDADDCOMBOX(BCUROUTER);
    }

#define SBOXCNTMAX 4
#define CASESBOXCNT(x) \
    CASE(SboxCNT##x)\
    {\
        STARTADDCOMBOX;\
        for(int i = 0; i<SBOXCNTMAX; i++)\
        {\
            combox->addItem(QString::number(i));\
        }\
        ENDADDCOMBOX(SboxCNT##x);\
    }
    CASESBOXCNT(0)
    CASESBOXCNT(1)
    CASESBOXCNT(2)
    CASESBOXCNT(3)
    CASE(SboxMode)
    {
        STARTADDCOMBOX;
        for(int i = 0; i<xml->m_sboxModeMap.count(); i++)
        {
            combox->addItem(xml->m_sboxModeMap.value(i));
        }
        ENDADDCOMBOX(SboxMode);
    }
    CASE(SboxSrc)
    {
        STARTADDTOOLBUTTON(SboxSrc);
    }
    CASE(BenesSrc)
    {
        STARTADDTOOLBUTTON(BenesSrc);
    }
    CASE(RCUMEMWTMODE)
    {
        STARTADDCOMBOX;
        for(int i = 0; i<4; i++)
        {
            combox->addItem(xml->m_memModeMap.value(i));
        }
        ENDADDCOMBOX(RCUMEMWTMODE);
    }
#define CASEADDR(x,n) \
    CASE(x)\
    {\
        STARTADDHEXSPINBOX(n);\
        spin->setMinimum(0);\
        ENDADDHEXSPINBBOX(x);\
    }

    CASEADDR(RCUMEMWTADDR,0x5f)
    CASE(RCUMEMWTMASK)
    {
        STARTADDCOMBOX;
        for(int i = 0; i<16; i++)
        {
            combox->addItem("'"+QString("%1").arg(i,4,2,QLatin1Char('0')));
        }
        ENDADDCOMBOX(RCUMEMWTMASK);
    }

    CASE(RCUMEMWTOFFSET)
    {
        STARTADDHEXSPINBOX(0x5f);
        spin->setMinimum(0);
        ENDADDHEXSPINBBOX(RCUMEMWTOFFSET);
    }
    CASE(RCUMEMWTTHREASHOLD)
    {
        if(xml->m_memModeMap.values().indexOf(attributeItemMap.value(RCUMEMWTMODE)->text(1)) == 3)
        {
            STARTADDHEXSPINBOX(0x5f);
            spin->setMinimum(0);
            ENDADDHEXSPINBBOX(RCUMEMWTTHREASHOLD);
        }
        else
        {
            STARTADDSPINBOX(0x5f);
            spin->setMinimum(0);
            ENDADDSPINBBOX(RCUMEMWTTHREASHOLD);
        }
    }
    CASE(RCUMEMRDMODE128_32)
    {
        STARTADDCOMBOX;
        combox->addItem(trUtf8("128位"));
        combox->addItem(trUtf8("32位"));
        ENDADDCOMBOX(RCUMEMRDMODE128_32);
    }
    CASE(RCUMEMRDMODE)
    {
        STARTADDCOMBOX;
        for(int i = 0; i<4; i++)
        {
            combox->addItem(xml->m_memModeMap.value(i));
        }
        ENDADDCOMBOX(RCUMEMRDMODE);
    }
    CASEADDR(RCUMEMRDADDR1,0x17f)
    CASEADDR(RCUMEMRDADDR2,0x17f)
    CASEADDR(RCUMEMRDADDR3,0x17f)
    CASEADDR(RCUMEMRDADDR4,0x17f)
    CASE(RCUMEMRDOFFSET)
    {
        STARTADDHEXSPINBOX(0x5f);
        spin->setMinimum(0);
        ENDADDHEXSPINBBOX(RCUMEMRDOFFSET);
    }
    CASE(RCUMEMRDTHREASHOLD)
    {
        if(xml->m_memModeMap.values().indexOf(attributeItemMap.value(RCUMEMRDMODE)->text(1)) == 3)
        {
            STARTADDHEXSPINBOX(0x5f);
            spin->setMinimum(0);
            ENDADDHEXSPINBBOX(RCUMEMRDTHREASHOLD);
        }
        else
        {
            STARTADDSPINBOX(0x5f);
            spin->setMinimum(0);
            ENDADDSPINBBOX(RCUMEMRDTHREASHOLD);
        }
    }
}

/**
 * AttrTreeWidget::showAttribue
 * \brief   
 * \param   element
 * \param   bcu
 * \param   rcu
 * \param   id
 * \author  zhangjun
 * \date    2016-10-12
 */
void AttrTreeWidget::showAttribue(ElementType element, int bcu, int rcu, int id)
{
    initAttributeValue(element,bcu,rcu,id);
    currentElement = element;
    currentBCU = bcu;
    currentRCU = rcu;
    currentID = id;
}

