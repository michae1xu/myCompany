#include "CPropertyBrowser.h"
#include "ui_CPropertyBrowser.h"

#include "RcaGraphScene.h"
#include "ModuleBcu.h"
#include "ModuleRcu.h"
#include "ElementBfu.h"
#include "ElementSbox.h"
#include "ElementBenes.h"
#include "ModuleReadImd.h"
#include "ModuleReadMem.h"
#include "ModuleReadRch.h"
#include "ModuleWriteMem.h"
#include "ModuleWriteRch.h"
#include "fileresolver/cprojectresolver.h"
#include "datafile.h"
#include "common/cappenv.hpp"

#include <QMap>
#include <QDebug>
#include <QString>
#include <QTimer>

CPropertyBrowser::CPropertyBrowser(RcaGraphScene *scene, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CPropertyBrowser),
    m_curRcaScene(scene)
{
    ui->setupUi(this);
    m_pVarManager = new QtVariantPropertyManager(ui->propertyTree);//关联factory，属性可以修改
    m_pVarFactory = new QtVariantEditorFactory(ui->propertyTree);    

    ui->label_error->setText(" ");
    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::red);
    ui->label_error->setPalette(pe);
}

CPropertyBrowser::~CPropertyBrowser()
{
    delete ui;
}

void CPropertyBrowser::reloadCurItemAttr()
{
    ui->label_error->setText(" ");
    updateItemProperty(m_curBaseItem);
}

void CPropertyBrowser::updateItemProperty(BaseItem *baseItem)
{
    //xf baseItem为空指针则不覆盖，保留上次赋值，让属性界面得以呈现
//    if(baseItem) // zhangjun 注释于20210101，空指针时属性界面不应呈现
        m_curBaseItem = baseItem;

    if(m_curBaseItem == nullptr)
    {
        ItemPropertyView(PropertyNone);
        return;
    }

    ModuleBcu* moduleBcu = dynamic_cast<ModuleBcu*>(m_curBaseItem);
    if(moduleBcu)
    {
       ItemPropertyView(PropertyBcu);
       return;
    }
    ModuleRcu* moduleRcu = dynamic_cast<ModuleRcu*>(m_curBaseItem);
    if(moduleRcu)
    {
        ItemPropertyView(PropertyRcu);
        return;
    }
    ElementBfu* elementBfu = dynamic_cast<ElementBfu*>(m_curBaseItem);
    if(elementBfu)
    {
        ItemPropertyView(PropertyBfu);
        return;
    }
    ElementCustomPE* elementCustomPE = dynamic_cast<ElementCustomPE*>(m_curBaseItem);
    if(elementCustomPE)
    {
        ItemPropertyView(PropertyCustomPE);
        return;
    }
    ElementSbox* elementSbox = dynamic_cast<ElementSbox*>(m_curBaseItem);
    if(elementSbox)
    {
        ItemPropertyView(PropertySbox);
        return;
    }
    ElementBenes* elementBenes = dynamic_cast<ElementBenes*>(m_curBaseItem);
    if(elementBenes)
    {
        ItemPropertyView(PropertyBenes);
        return;
    }
    ModuleReadImd* readImd = dynamic_cast<ModuleReadImd*>(m_curBaseItem);
    if(readImd)
    {
        ItemPropertyView(PropertyRImd);
        return;
    }
    ModuleReadMem* readMem = dynamic_cast<ModuleReadMem*>(m_curBaseItem);
    if(readMem)
    {
        ItemPropertyView(PropertyRMem);
        return;
    }
    ModuleWriteMem* writeMem = dynamic_cast<ModuleWriteMem*>(m_curBaseItem);
    if(writeMem)
    {
        ItemPropertyView(PropertyWMem);
        return;
    }
    ModuleReadRch* readRch = dynamic_cast<ModuleReadRch*>(m_curBaseItem);
    if(readRch)
    {
        ItemPropertyView(PropertyRRch);
        return;
    }
    ModuleWriteRch* writeRch = dynamic_cast<ModuleWriteRch*>(m_curBaseItem);
    if(writeRch)
    {
        ItemPropertyView(PropertyWRch);
        return;
    }
    ModuleReadLfsr* readLfsr = dynamic_cast<ModuleReadLfsr*>(m_curBaseItem);
    if(readLfsr)
    {
        ItemPropertyView(PropertyRLfsr);
        return;
    }
    ModuleWriteLfsr* writeLfsr = dynamic_cast<ModuleWriteLfsr*>(m_curBaseItem);
    if(writeLfsr)
    {
        ItemPropertyView(PropertyWLfsr);
        return;
    }
}

void CPropertyBrowser::setEnable(bool enable)
{
    m_enable = enable;
}

bool CPropertyBrowser::validLoopChange(RcaGraphScene* m_curRcaScene,int iRcuIndex,int iNowPosFlag,int iChangeFlag,bool bLoop2)
{
    return true;
    QList<int> listRcu,listUp,listDown;
    QList<QGraphicsItem *> childList = m_curRcaScene->items();
    QList<QGraphicsItem*>::iterator iter;
    for(iter=childList.begin();iter!=childList.end();iter++)
    {
        if((*iter))
        {
            ModuleRcu* moduleRcu = dynamic_cast<ModuleRcu*>(*iter);
            if(moduleRcu)
                listRcu.append(moduleRcu->getIndexRcu());
        }
    }
    qSort(listRcu.begin(),listRcu.end());
    for (int i = 0; i < listRcu.size(); ++i)
    {
        if(listRcu.at(i) > iRcuIndex)
            listDown.append(listRcu.at(i));
        else if(listRcu.at(i) < iRcuIndex)
            listUp.append(listRcu.at(i));
    }

    enum{none,start,end,start_end};
    int iNextFlag = 0;
    bool bChangeFlag = false,bContinue = true;
    //当前值为none,可改为start,end,start_end
    //条件：none->start/start_end,upNext为end或iRcu==0
    //条件：none->end,upNext为start
    if(iNowPosFlag == none)
    {
        for (int i = listUp.size()-1; i >= 0; --i)
        {
            if(!bContinue)
                break;
            for(iter=childList.begin();iter!=childList.end();iter++)
            {
                if((*iter))
                {
                    ModuleRcu* moduleRcu = dynamic_cast<ModuleRcu*>(*iter);
                    if(moduleRcu && moduleRcu->getIndexRcu() == listUp.at(i))
                    {
                        if(bLoop2)
                            iNextFlag = moduleRcu->loopStartEndFlag2;
                        else
                            iNextFlag = moduleRcu->loopStartEndFlag;
                        if(iNextFlag != none)
                            bContinue = false;
                        else if(iNextFlag == none && listUp.at(i) == 0)
                        {
                            bChangeFlag = true;
                            break;
                        }

                        if((iChangeFlag == start || iChangeFlag == start_end) &&
                                (iNextFlag == end ||
                                 iNextFlag == start_end))
                        {
                            bChangeFlag = true;
                            break;
                        }
                        else if(iChangeFlag == end && iNextFlag == start)
                        {
                            bChangeFlag = true;
                            break;
                        }
                    }
                }
            }
            if(bChangeFlag)
                break;
        }

    }
    //当前值为end,可改为none,不可改为start,start_end
    //条件：end->none,upNext为start(肯定的)
    else if(iNowPosFlag == end && iChangeFlag == none)
        bChangeFlag = true;
    //当前值为start,可改为none，start_end，不可改为end
    //条件：start->none,downNext不是end
    //条件：start->start_end,downNext不是end
    else if(iNowPosFlag == start && (iChangeFlag == start_end || iChangeFlag == none))
    {
        for (int i = 0; i < listDown.size(); ++i)
        {
            if(!bContinue)
                break;
            for(iter=childList.begin();iter!=childList.end();iter++)
            {
                if((*iter))
                {
                    ModuleRcu* moduleRcu = dynamic_cast<ModuleRcu*>(*iter);
                    if(moduleRcu && moduleRcu->getIndexRcu() == listDown.at(i))
                    {
                        if(bLoop2)
                            iNextFlag = moduleRcu->loopStartEndFlag2;
                        else
                            iNextFlag = moduleRcu->loopStartEndFlag;
                        if(iNextFlag != none)
                            bContinue = false;
                        else if(iNextFlag == none && listDown.at(i) == 0)
                        {
                            bChangeFlag = true;
                            break;
                        }

                        if(iNextFlag != end)
                            bChangeFlag = true;
                    }
                }
            }
            if(bChangeFlag)
                break;
        }
    }
    //当前值为start_end,可改为none,start，不可改为end
    //条件：start_end->none,无条件
    else if(iNowPosFlag == start_end && iChangeFlag == none)
        bChangeFlag = true;
    //条件：start_end->start,downNext不是end或者为空
    else if(iNowPosFlag == start_end && iChangeFlag == start)
    {
        for (int i = 0; i < listDown.size(); ++i)
        {
            if(!bContinue)
                break;
            for(iter=childList.begin();iter!=childList.end();iter++)
            {
                if((*iter))
                {
                    ModuleRcu* moduleRcu = dynamic_cast<ModuleRcu*>(*iter);
                    if(moduleRcu && moduleRcu->getIndexRcu() == listDown.at(i))
                    {
                        if(bLoop2)
                            iNextFlag = moduleRcu->loopStartEndFlag2;
                        else
                            iNextFlag = moduleRcu->loopStartEndFlag;
                        if(iNextFlag != none)
                            bContinue = false;
                        else if(iNextFlag == none && listDown.at(i) == 0)
                        {
                            bChangeFlag = true;
                            break;
                        }

                        if(iNextFlag != end)
                            bChangeFlag = true;
                    }
                }
            }
            if(bChangeFlag)
                break;
        }

    }
    return bChangeFlag;
}

void CPropertyBrowser::ItemPropertyView(PropertyId propertyId)
{
    if(!m_enable)
        return;
    m_resourceKeys.clear();
    m_resourceKeys.append("unset");
    QVariant icons;
    if(m_curRcaScene->getProject()) {
        m_resourceKeys.append(m_curRcaScene->getProject()->getProjectParameter()->resourceMap.keys());
        QtIconMap iconMap;
        for(auto i = 0; i< m_resourceKeys.count(); ++i) {
            const QString &key = m_resourceKeys[i];
            QIcon icon;
            QString suffix = key.right(key.length() - key.lastIndexOf(".") - 1);
            if(QFileInfo(CAppEnv::getImageDirPath() + QString("/srcico/%0.png").arg(suffix).toLower()).exists()) {
                icon = QIcon(CAppEnv::getImageDirPath() + QString("/srcico/%0.png").arg(suffix).toLower());
            }
            else
            {
                icon = QIcon(CAppEnv::getImageDirPath() + QString("/128/puzzle128.png"));
            }
            iconMap.insert(i, icon);
        }
        icons.setValue(iconMap);
    }

    disconnect(m_pVarManager, &QtVariantPropertyManager::valueChanged, this, &CPropertyBrowser::onValueChanged);//绑定信号槽，当值改变的时候会发送信号
    ui->propertyTree->clear();
    QStringList enumNames;
    if(propertyId == PropertyBcu)
    {
        ModuleBcu* moduleBcu = dynamic_cast<ModuleBcu*>(m_curBaseItem);
        if(!moduleBcu)
            return;
        QtVariantProperty *property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(),tr(u8"RCU ID："));
        enumNames.clear();
        enumNames << "0" << "1" << "2" << "3" << "4" << "5" << "6" << "7"
                  << "8" << "9" << "10" << "11" << "12" << "13" << "14" << "15";
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(moduleBcu->sort);
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = tr(u8"RCU ID：");

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(),tr(u8"Router："));
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(moduleBcu->srcDataFromBcu);
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = tr(u8"Router：");

        property = m_pVarManager->addProperty(QVariant::Int, QString("Burst："));
        property->setAttribute(QLatin1String("minimum"), 0);
        property->setAttribute(QLatin1String("maximum"), 255);
        property->setAttribute(QLatin1String("displayIntegerBase"), 10);
        property->setValue(moduleBcu->burst);
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = QString("Burst：");

//        property = m_pVarManager->addProperty(QVariant::Int, QString("Gap："));
//        property->setAttribute(QLatin1String("minimum"), 0);
//        property->setAttribute(QLatin1String("maximum"), 255);
//        property->setAttribute(QLatin1String("displayIntegerBase"), 10);
//        property->setValue(moduleBcu->gap);
//        ui->propertyTree->addProperty(property);
//        m_property_dic[property] = QString("Gap：");

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), tr(u8"Infifo来源："));
        enumNames.clear();
        enumNames << tr(u8"当前核") << tr(u8"上一个核");
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(moduleBcu->infifoSrc);
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = tr(u8"Infifo来源：");

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), tr(u8"Outfifo输出："));
        enumNames.clear();
        enumNames << tr(u8"当前核") << tr(u8"下一个核");
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(moduleBcu->outfifoSrc);
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = tr(u8"Outfifo输出：");

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), tr(u8"LFSR分组："));
        enumNames.clear();
        enumNames << tr(u8"未分组") << tr(u8"分组1") << tr(u8"分组2") << tr(u8"分组3") << tr(u8"分组4");
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(moduleBcu->lfsrGroup);
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = tr(u8"LFSR分组：");

        QtVariantProperty *groupProperty = m_pVarManager->addProperty(QtVariantPropertyManager::groupTypeId(),QString("Memory Read"));//添加组
        groupProperty = m_pVarManager->addProperty(QtVariantPropertyManager::groupTypeId(),QString("IMD"));//添加组

        property  = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(),tr(u8"128/32位选择："));
        enumNames.clear();
        enumNames << tr(u8"128位") << tr(u8"32位");
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(moduleBcu->readImdMode128_32);
        groupProperty->addSubProperty(property);
        ui->propertyTree->addProperty(groupProperty);
        m_property_dic[property] = tr(u8"128/32位选择：");

        // group5
        groupProperty = m_pVarManager->addProperty(QtVariantPropertyManager::groupTypeId(),QString("LFSR"));//添加组
        property = m_pVarManager->addProperty(QVariant::Int, tr(u8"Depth："));
        property->setAttribute(QLatin1String("minimum"), 0);
        property->setAttribute(QLatin1String("maximum"), 1024);
        property->setAttribute(QLatin1String("displayIntegerBase"), 10);
        property->setValue(moduleBcu->writeDepth);
        groupProperty->addSubProperty(property);
        ui->propertyTree->addProperty(groupProperty);
        m_property_dic[property] = tr(u8"Depth：");

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), tr(u8"Width："));
        enumNames.clear();
        enumNames << "0" << "1" << "8" << "16" << "32";
        property->setAttribute(QString("enumNames"),enumNames);
        int iWidth = moduleBcu->writeWidth;
        int iValue = iWidth==0?0:iWidth==1?1:iWidth==8?2:iWidth==16?3:iWidth==32?4:0;
        property->setValue(iValue);
        groupProperty->addSubProperty(property);
        ui->propertyTree->addProperty(groupProperty);
        m_property_dic[property] = tr(u8"Width：");

        property = m_pVarManager->addProperty(QVariant::String, QString("Address Mode(0x)："));
        property->setValue(moduleBcu->addressMode);
        groupProperty->addSubProperty(property);
        ui->propertyTree->addProperty(groupProperty);
        m_property_dic[property] = QString("Address Mode(0x)：");

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), tr(u8"Memory数据："));
        enumNames.clear();
        enumNames.append(m_resourceKeys);
        property->setAttribute(QString("enumNames"), enumNames);
        property->setAttribute(QString("enumIcons"), icons);
        property->setValue(m_resourceKeys.indexOf(moduleBcu->memorySrcConfig));
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = tr(u8"Memory数据：");

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), tr(u8"InFifo数据："));
        property->setAttribute(QString("enumNames"), enumNames);
        property->setValue(m_resourceKeys.indexOf(moduleBcu->inFifoSrcConfig));
        property->setAttribute(QString("enumIcons"), icons);
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = tr(u8"InFifo数据：");

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), tr(u8"IMD数据："));
        property->setAttribute(QString("enumNames"), enumNames);
        property->setValue(m_resourceKeys.indexOf(moduleBcu->imdSrcConfig));
        property->setAttribute(QString("enumIcons"), icons);
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = tr(u8"IMD数据：");


        ui->propertyTree->setFactoryForManager(m_pVarManager,m_pVarFactory);
    }
    else if(propertyId == PropertyRcu)
    {
        ModuleRcu* moduleRcu = dynamic_cast<ModuleRcu*>(m_curBaseItem);
        if(!moduleRcu)
            return;
        //0
//        QtVariantProperty *property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("RCU CFG:"));
//        enumNames << "0" << "1" << "2";
//        property->setAttribute(QString("enumNames"),enumNames);
//        property->setValue(moduleRcu->getIndexRcu());
//        ui->propertyTree->addProperty(property);
//        m_property_dic[property] = QString("RCU CFG:");

        QtVariantProperty *property = m_pVarManager->addProperty(QVariant::Int, QString("RCU CFG："));
        property->setAttribute(QLatin1String("minimum"), 0);
        property->setAttribute(QLatin1String("maximum"), 9999);
        property->setAttribute(QLatin1String("displayIntegerBase"), 10);
        property->setValue(moduleRcu->getIndexRcu());
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = QString("RCU CFG:");

        //group 1
        QtVariantProperty *groupProperty = m_pVarManager->addProperty(QtVariantPropertyManager::groupTypeId(),QString("Loop"));//添加组

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(),tr(u8"Loop："));
        enumNames.clear();
        enumNames << "none" << tr(u8"start")  << tr(u8"end") << tr(u8"start & end");
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(moduleRcu->loopStartEndFlag);
        groupProperty->addSubProperty(property);
        ui->propertyTree->addProperty(groupProperty);
        m_property_dic[property] = tr(u8"Loop：");

        property = m_pVarManager->addProperty(QVariant::Int, tr(u8"Loop次数："));
        property->setAttribute(QLatin1String("minimum"), 0);
        property->setAttribute(QLatin1String("maximum"), 9999);
        property->setAttribute(QLatin1String("displayIntegerBase"), 10);
        property->setValue(moduleRcu->loopTimes);
        groupProperty->addSubProperty(property);
        m_property_dic[property] = tr(u8"Loop次数：");

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(),tr(u8"Loop2："));
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(moduleRcu->loopStartEndFlag2);
        groupProperty->addSubProperty(property);
        m_property_dic[property] = tr(u8"Loop2：");

        property = m_pVarManager->addProperty(QVariant::Int, tr(u8"Loop2次数："));
        property->setAttribute(QLatin1String("minimum"), 0);
        property->setAttribute(QLatin1String("maximum"), 9999);
        property->setAttribute(QLatin1String("displayIntegerBase"), 10);
        property->setValue(moduleRcu->loopTimes2);
        groupProperty->addSubProperty(property);
        m_property_dic[property] = tr(u8"Loop2次数：");

        groupProperty = m_pVarManager->addProperty(QtVariantPropertyManager::groupTypeId(),QString("Memory Read"));//添加组

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(),tr(u8"128/32位选择："));
        enumNames.clear();enumNames << tr(u8"128位")  << tr(u8"32位");
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(moduleRcu->readMemMode128_32);
        groupProperty->addSubProperty(property);
        ui->propertyTree->addProperty(groupProperty);
        m_property_dic[property] = tr(u8"128/32位选择：");

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(),tr(u8"模式："));
        enumNames.clear();enumNames << tr(u8"递减模式") << tr(u8"累加模式")/* << tr(u8"阈值模式") << tr(u8"累加&阈值清零")*/;
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(moduleRcu->readMemMode);
        groupProperty->addSubProperty(property);
        m_property_dic[property] = tr(u8"模式：");

        property = m_pVarManager->addProperty(QVariant::Int, tr(u8"起始地址1(0x)："));
        property->setAttribute(QLatin1String("minimum"), 0);
        property->setAttribute(QLatin1String("maximum"), 597);
        property->setAttribute(QLatin1String("displayIntegerBase"), 16);
        property->setValue(moduleRcu->readMemAddr1);
        groupProperty->addSubProperty(property);
        m_property_dic[property] = tr(u8"起始地址1(0x)：");

        property = m_pVarManager->addProperty(QVariant::Int, tr(u8"起始地址2(0x)："));
        property->setAttribute(QLatin1String("minimum"), 0);
        property->setAttribute(QLatin1String("maximum"), 597);
        property->setAttribute(QLatin1String("displayIntegerBase"), 16);
        property->setValue(moduleRcu->readMemAddr2);
        groupProperty->addSubProperty(property);
        m_property_dic[property] = tr(u8"起始地址2(0x)：");

        property = m_pVarManager->addProperty(QVariant::Int, tr(u8"起始地址3(0x)："));
        property->setAttribute(QLatin1String("minimum"), 0);
        property->setAttribute(QLatin1String("maximum"), 597);
        property->setAttribute(QLatin1String("displayIntegerBase"), 16);
        property->setValue(moduleRcu->readMemAddr3);
        groupProperty->addSubProperty(property);
        m_property_dic[property] = tr(u8"起始地址3(0x)：");

        property = m_pVarManager->addProperty(QVariant::Int, tr(u8"起始地址4(0x)："));
        property->setAttribute(QLatin1String("minimum"), 0);
        property->setAttribute(QLatin1String("maximum"), 597);
        property->setAttribute(QLatin1String("displayIntegerBase"), 16);
        property->setValue(moduleRcu->readMemAddr4);
        groupProperty->addSubProperty(property);
        m_property_dic[property] = tr(u8"起始地址4(0x)：");

        property = m_pVarManager->addProperty(QVariant::Int, tr(u8"偏移(0x)："));
        property->setAttribute(QLatin1String("minimum"), 0);
        property->setAttribute(QLatin1String("maximum"), 256);
        property->setAttribute(QLatin1String("displayIntegerBase"), 16);
        property->setValue(moduleRcu->readMemOffset);
        groupProperty->addSubProperty(property);
        m_property_dic[property] = tr(u8"偏移(0x)：");

        property = m_pVarManager->addProperty(QVariant::Int, tr(u8"读写(阈值)："));
        property->setAttribute(QLatin1String("minimum"), 0);
        property->setAttribute(QLatin1String("maximum"), 256);
        property->setAttribute(QLatin1String("displayIntegerBase"), 16);
        property->setValue(moduleRcu->readMemThreashold);
        groupProperty->addSubProperty(property);
        m_property_dic[property] = tr(u8"读写(阈值)：");

        ui->propertyTree->setFactoryForManager(m_pVarManager,m_pVarFactory);
        //group 2
        groupProperty = m_pVarManager->addProperty(QtVariantPropertyManager::groupTypeId(),QString("Memory Write"));//添加组

        property  = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(),tr(u8"模式："));
        enumNames.clear();enumNames << tr(u8"固定模式") << tr(u8"累加模式")/* << tr(u8"阈值模式") << tr(u8"累加&阈值清零")*/;
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(moduleRcu->writeMemMode);
        groupProperty->addSubProperty(property);
        ui->propertyTree->addProperty(groupProperty);
        m_property_dic[property] = tr(u8"2 模式：");

        property = m_pVarManager->addProperty(QVariant::Int, tr(u8"起始地址(0x)："));
        property->setAttribute(QLatin1String("minimum"), 0);
        property->setAttribute(QLatin1String("maximum"), 256);
        property->setAttribute(QLatin1String("displayIntegerBase"), 16);
        property->setValue(moduleRcu->writeMemAddr);
        groupProperty->addSubProperty(property);
        m_property_dic[property] = tr(u8"起始地址(0x)：");

        property  = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(),tr(u8"数据掩盖(0b)："));
        enumNames.clear();
        enumNames << tr(u8"'0000") << tr(u8"'0001") << tr(u8"'0010") << tr(u8"'0011") << tr(u8"'0100")
                  << tr(u8"'0101") << tr(u8"'0110") << tr(u8"'0111") << tr(u8"'1000") << tr(u8"'1001")
                  << tr(u8"'1010") << tr(u8"'1011") << tr(u8"'1100") << tr(u8"'1101") << tr(u8"'1110")
                  << tr(u8"'1111");
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(moduleRcu->writeMemMask);
        groupProperty->addSubProperty(property);
        ui->propertyTree->addProperty(groupProperty);
        m_property_dic[property] = tr(u8"数据掩盖(0b)：");

        property = m_pVarManager->addProperty(QVariant::Int, tr(u8"偏移(0x)："));
        property->setAttribute(QLatin1String("minimum"), 0);
        property->setAttribute(QLatin1String("maximum"), 256);
        property->setAttribute(QLatin1String("displayIntegerBase"), 16);
        property->setValue(moduleRcu->writeMemOffset);
        groupProperty->addSubProperty(property);
        m_property_dic[property] = tr(u8"2 偏移(0x)：");

        property = m_pVarManager->addProperty(QVariant::Int, tr(u8"读写(阈值)："));
        property->setAttribute(QLatin1String("minimum"), 0);
        property->setAttribute(QLatin1String("maximum"), 256);
        property->setAttribute(QLatin1String("displayIntegerBase"), 16);
        property->setValue(moduleRcu->writeMemThreashold);
        groupProperty->addSubProperty(property);
        m_property_dic[property] = tr(u8"2 读写(阈值)：");

        // group3
        groupProperty = m_pVarManager->addProperty(QtVariantPropertyManager::groupTypeId(),QString("RCH"));//添加组

        property  = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(),tr(u8"模式："));
        enumNames.clear();
//        enumNames << "1*16" << "1*16 >>" << "2*8 >>" << "4*4 >>";
        enumNames << "1*16" << "1*16 >>" << "4*4 >>";
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(moduleRcu->rchMode);
        groupProperty->addSubProperty(property);
        ui->propertyTree->addProperty(groupProperty);
        m_property_dic[property] = tr(u8"3 模式：");

//        property  = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(),tr(u8"写地址1："));
//        enumNames.clear();
//        for(int i = 0; i < 16; ++i)
//        {
//            enumNames << QString::number(i);
//        }
//        property->setAttribute(QString("enumNames"),enumNames);
//        property->setValue(moduleRcu->writeRchAddr1);
//        groupProperty->addSubProperty(property);
//        ui->propertyTree->addProperty(groupProperty);
//        m_property_dic[property] = tr(u8"写地址1：");

//        property  = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(),tr(u8"写地址2："));
//        property->setAttribute(QString("enumNames"),enumNames);
//        property->setValue(moduleRcu->writeRchAddr2);
//        groupProperty->addSubProperty(property);
//        ui->propertyTree->addProperty(groupProperty);
//        m_property_dic[property] = tr(u8"写地址2：");

//        property  = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(),tr(u8"写地址3："));
//        property->setAttribute(QString("enumNames"),enumNames);
//        property->setValue(moduleRcu->writeRchAddr3);
//        groupProperty->addSubProperty(property);
//        ui->propertyTree->addProperty(groupProperty);
//        m_property_dic[property] = tr(u8"写地址3：");

//        property  = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(),tr(u8"写地址4："));
//        property->setAttribute(QString("enumNames"),enumNames);
//        property->setValue(moduleRcu->writeRchAddr4);
//        groupProperty->addSubProperty(property);
//        ui->propertyTree->addProperty(groupProperty);
//        m_property_dic[property] = tr(u8"写地址4：");

        ui->propertyTree->setFactoryForManager(m_pVarManager,m_pVarFactory);
    }
    else if(propertyId == PropertyBfu)
    {
        ElementBfu* elementBfu = dynamic_cast<ElementBfu*>(m_curBaseItem);
        if(!elementBfu)
            return;
        QtVariantProperty *property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("BFU ID："));
        enumNames << "0" << "1" << "2" << "3" << "4" << "5" << "6" << "7";
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(elementBfu->getIndexInRcu());
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = QString("BFU ID：");

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("算子："));
        enumNames.clear();
        enumNames << "AU"<< "SU"  << "LU" << "MU";
        property->setAttribute(QString("enumNames"), enumNames);
        property->setValue(elementBfu->funcIndex);
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = QString("算子：");

        property = m_pVarManager->addProperty(QVariant::String, QString("表达式："));
        ui->propertyTree->addProperty(property);
        property->setValue(elementBfu->funcExp);
//        ui->propertyTree->setFactoryForManager(m_pVarManager,m_pVarFactory);
        m_property_dic[property] = QString("表达式：");

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("Bypass："));
        enumNames.clear();
        enumNames << "A" << "B" << "T" << "Y";
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(elementBfu->bypassIndex);
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = QString("Bypass：");

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), tr(u8"模："));
        enumNames.clear();
        enumNames << "2^32" << "2^16" << "2^8" << "2^31-1";
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(elementBfu->funcAuModIndex);
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = tr(u8"模：");

//        property = m_pVarManager->addProperty(QVariant::Bool, tr(u8"进位："));
//        property->setValue(elementBfu->funcAuCarry);
//        ui->propertyTree->addProperty(property);
//        m_property_dic[property] = tr(u8"进位：");

        if(elementBfu->funcIndex == 3)
        {
            property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), tr(u8"乘法模式："));
            enumNames.clear();
            enumNames << "16位乘法" << "8位乘法";
            //        enumNames << "16位乘法" << "8位乘法-A[7:0]*B[7:0]" << "8位乘法-A[7:0]*B[15:8]"
            //                  << "8位乘法-A[15:8]*B[7:0]" << "8位乘法-A[15:8]*B[15:8]";
            property->setAttribute(QString("enumNames"),enumNames);
            property->setValue(elementBfu->funcMuModIndex);
            ui->propertyTree->addProperty(property);
            m_property_dic[property] = tr(u8"乘法模式：");
        }

        property = m_pVarManager->addProperty(QVariant::String, tr(u8"Bypass表达式："));
        ui->propertyTree->addProperty(property);
        property->setValue(elementBfu->bypassExp);
        m_property_dic[property] = tr(u8"Bypass表达式：");

        ui->propertyTree->setFactoryForManager(m_pVarManager,m_pVarFactory);
    }
    else if(propertyId == PropertyCustomPE)
    {
        ElementCustomPE* elementCustomPE = dynamic_cast<ElementCustomPE*>(m_curBaseItem);
        if(!elementCustomPE)
            return;
        QtVariantProperty *property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("BFU ID："));
        enumNames << "0" << "1" << "2" << "3" << "4" << "5" << "6" << "7";
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(elementCustomPE->getIndexInRcu());
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = QString("BFU ID：");

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("inputPortCount："));
        enumNames.clear();
        enumNames << "1" << "2" << "3" << "4";
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(elementCustomPE->inputNum() - 1);
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = QString("inputPortCount：");

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("outputPortCount："));
        enumNames.clear();
        enumNames << "1" << "2" << "3" << "4";
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(elementCustomPE->outputNum() - 1);
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = QString("outputPortCount：");

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("算子："));
        enumNames.clear();
        enumNames << "AU" << "LU" << "SU" << "MU";
        property->setAttribute(QString("enumNames"), enumNames);
        property->setValue(elementCustomPE->funcIndex);
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = QString("算子：");

        property = m_pVarManager->addProperty(QVariant::String, QString("表达式："));
        ui->propertyTree->addProperty(property);
        property->setValue(elementCustomPE->funcExp);
//        ui->propertyTree->setFactoryForManager(m_pVarManager,m_pVarFactory);
        m_property_dic[property] = QString("表达式：");

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("Bypass："));
        enumNames.clear();
        enumNames << "A" << "B" << "T" << "Y";
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(elementCustomPE->bypassIndex);
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = QString("Bypass：");

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), tr(u8"模："));
        enumNames.clear();
        enumNames << "2^32" << "2^16" << "2^8" << "2^31-1";
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(elementCustomPE->funcAuModIndex);
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = tr(u8"模：");

//        property = m_pVarManager->addProperty(QVariant::Bool, tr(u8"进位："));
//        property->setValue(elementBfu->funcAuCarry);
//        ui->propertyTree->addProperty(property);
//        m_property_dic[property] = tr(u8"进位：");

//        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), tr(u8"乘法模式："));
//        enumNames.clear();
//        enumNames << "16位乘法" << "8位乘法";
////        enumNames << "16位乘法" << "8位乘法-A[7:0]*B[7:0]" << "8位乘法-A[7:0]*B[15:8]"
////                  << "8位乘法-A[15:8]*B[7:0]" << "8位乘法-A[15:8]*B[15:8]";
//        property->setAttribute(QString("enumNames"),enumNames);
//        property->setValue(elementCustomPE->funcMuModIndex);
//        ui->propertyTree->addProperty(property);
//        m_property_dic[property] = tr(u8"乘法模式：");

        property = m_pVarManager->addProperty(QVariant::String, tr(u8"Bypass表达式："));
        ui->propertyTree->addProperty(property);
        property->setValue(elementCustomPE->bypassExp);
        m_property_dic[property] = tr(u8"Bypass表达式：");

        ui->propertyTree->setFactoryForManager(m_pVarManager,m_pVarFactory);
    }
    else if(propertyId == PropertySbox)
    {
        ElementSbox* elementSbox = dynamic_cast<ElementSbox*>(m_curBaseItem);
        if(!elementSbox)
            return;
        QtVariantProperty *property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("SBOX ID："));
        enumNames.clear();enumNames << "0" << "1" << "2" << "3";
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(elementSbox->getIndexInRcu());
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = QString("SBOX ID：");

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("Group："));
        enumNames.clear();enumNames << "0" << "1";
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(elementSbox->sboxGroup);
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = QString("Group：");

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), tr(u8"模式："));
        enumNames.clear();
        enumNames << tr(u8"4bit-4bit拼接") << tr(u8"6bit-4bit拼接") << tr(u8"8bit-8bit拼接")
                  << tr(u8"8bit-8bit异或") << tr(u8"8bit-32bit") << tr(u8"1路9bit-9bit")
                  << tr(u8"1路10bit-8bit") << tr(u8"1路12bit-8bit");
        property->setAttribute(QString("enumNames"), enumNames);
        property->setValue(elementSbox->sboxMode);
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = tr(u8"模式：");

        if(elementSbox->sboxMode == 0 || elementSbox->sboxMode == 1 || elementSbox->sboxMode == 2 )
        {
            property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("Byte Sel："));
            enumNames.clear();enumNames << "0" << "1" << "2" << "3";
            property->setAttribute(QString("enumNames"),enumNames);
            property->setValue(elementSbox->sboxByteSel);
            ui->propertyTree->addProperty(property);
            m_property_dic[property] = QString("Byte Sel：");
        }

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), tr(u8"自增模式："));
        enumNames.clear();
        enumNames << tr(u8"否") << tr(u8"是");
        property->setAttribute(QString("enumNames"), enumNames);
        property->setValue(elementSbox->sboxIncreaseMode);
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = tr(u8"自增模式：");

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), tr(u8"数据源："));
        enumNames.clear();
        enumNames.append(m_resourceKeys);
        property->setAttribute(QString("enumNames"), enumNames);
        property->setValue(m_resourceKeys.indexOf(elementSbox->srcConfig));
        property->setAttribute(QString("enumIcons"), icons);
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = tr(u8"数据源：");

        ui->propertyTree->setFactoryForManager(m_pVarManager,m_pVarFactory);
    }
    else if(propertyId == PropertyBenes)
    {
        ElementBenes* elementBenes = dynamic_cast<ElementBenes*>(m_curBaseItem);
        if(!elementBenes)
            return;
        QtVariantProperty *property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("BENES ID："));
        enumNames.clear();enumNames << "0";
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(elementBenes->getIndexInRcu());
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = QString("BENES ID：");

        property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), tr(u8"数据源："));
        enumNames.clear();
        enumNames.append(m_resourceKeys);
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(m_resourceKeys.indexOf(elementBenes->srcConfig));
        property->setAttribute(QString("enumIcons"), icons);
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = tr(u8"数据源：");

        ui->propertyTree->setFactoryForManager(m_pVarManager, m_pVarFactory);
    }
    else if(propertyId == PropertyRImd)
    {
        ModuleReadImd* moduleRImd = dynamic_cast<ModuleReadImd*>(m_curBaseItem);
        if(!moduleRImd)
            return;
        QtVariantProperty *property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("IMD PORT："));
        enumNames.clear();enumNames << "0" << "1" << "2" << "3";
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(moduleRImd->getReadImdIndex());
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = QString("IMD PORT：");

        ui->propertyTree->setFactoryForManager(m_pVarManager,m_pVarFactory);
    }
    else if(propertyId == PropertyRMem)
    {
        ModuleReadMem* moduleRMem = dynamic_cast<ModuleReadMem*>(m_curBaseItem);
        if(!moduleRMem)
            return;
        QtVariantProperty *property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("MEM PORT："));
        enumNames.clear();
        enumNames << "0" << "1" << "2" << "3";
        property->setAttribute(QString("enumNames"), enumNames);
        property->setValue(moduleRMem->getReadMemIndex());
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = QString("MEM PORT：");

        ui->propertyTree->setFactoryForManager(m_pVarManager,m_pVarFactory);
    }
    else if(propertyId == PropertyWMem)
    {
        ModuleWriteMem* moduleWMem = dynamic_cast<ModuleWriteMem*>(m_curBaseItem);
        if(!moduleWMem)
            return;
        QtVariantProperty *property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("MEM PORT："));
        enumNames.clear();enumNames << "0" << "1" << "2" << "3";
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(moduleWMem->getWriteMemIndex());
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = QString("MEM PORT：");

        ui->propertyTree->setFactoryForManager(m_pVarManager,m_pVarFactory);
    }
    else if(propertyId == PropertyRRch)
    {
        ModuleReadRch* moduleRRch = dynamic_cast<ModuleReadRch*>(m_curBaseItem);
        if(!moduleRRch)
            return;
        QtVariantProperty *property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("RCH PORT："));
        enumNames.clear();enumNames << "0" << "1" << "2" << "3" << "4" << "5" << "6" << "7" <<
                                       "8" << "9" << "10" << "11" << "12" << "13" << "14" << "15";
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(moduleRRch->getReadRchIndex());
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = QString("RCH PORT：");

        ui->propertyTree->setFactoryForManager(m_pVarManager,m_pVarFactory);
    }
    else if(propertyId == PropertyWRch)
    {
        ModuleWriteRch* moduleWRch = dynamic_cast<ModuleWriteRch*>(m_curBaseItem);
        if(!moduleWRch)
            return;
        QtVariantProperty *property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("RCH PORT："));
        enumNames.clear();enumNames << "0" << "1" << "2" << "3" ;
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(moduleWRch->getWriteRchIndex());
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = QString("RCH PORT：");

        property  = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(),tr(u8"写地址："));
        enumNames.clear();enumNames << "0" << "1" << "2" << "3" << "4" << "5" << "6" << "7"
                                       << "8" << "9" << "10" << "11" << "12" << "13" << "14" << "15" ;
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(moduleWRch->getWriteAddress());
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = tr(u8"写地址：");

        ui->propertyTree->setFactoryForManager(m_pVarManager,m_pVarFactory);
    }
    else if(propertyId == PropertyRLfsr)
    {
        ModuleReadLfsr* moduleRLfsr = dynamic_cast<ModuleReadLfsr*>(m_curBaseItem);
        if(!moduleRLfsr)
            return;
        QtVariantProperty *property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("LFSR PORT："));
        enumNames.clear();enumNames << "0" << "1" << "2" << "3" << "4" << "5" << "6" << "7" <<
                                       "8" << "9" << "10" << "11" << "12" << "13" << "14" << "15";
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(moduleRLfsr->getReadLfsrIndex());
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = QString("LFSR PORT：");

        ui->propertyTree->setFactoryForManager(m_pVarManager,m_pVarFactory);
    }
    else if(propertyId == PropertyWLfsr)
    {
        ModuleWriteLfsr* moduleWLfsr = dynamic_cast<ModuleWriteLfsr*>(m_curBaseItem);
        if(!moduleWLfsr)
            return;
        QtVariantProperty *property = m_pVarManager->addProperty(QtVariantPropertyManager::enumTypeId(), QString("LFSR PORT："));
        enumNames.clear();enumNames << "0";
        property->setAttribute(QString("enumNames"),enumNames);
        property->setValue(moduleWLfsr->getWriteLfsrIndex());
        ui->propertyTree->addProperty(property);
        m_property_dic[property] = QString("LFSR PORT：");

        ui->propertyTree->setFactoryForManager(m_pVarManager,m_pVarFactory);
    }

    connect(m_pVarManager,&QtVariantPropertyManager::valueChanged,this, &CPropertyBrowser::onValueChanged);//绑定信号槽，当值改变的时候会发送信号
}

void CPropertyBrowser::onValueChanged(QtProperty *property, const QVariant &value)
{
    QString strPropertyName = m_property_dic[property];
    qDebug()<<strPropertyName<<"："<<value;

    switch (m_curBaseItem->getUnitId()) {
    case BCU_ID:
    {
        ModuleBcu* moduleBcu = dynamic_cast<ModuleBcu*>(m_curBaseItem);
        if(moduleBcu)
        {
            ModuleBcu moduleBcuChanged(*moduleBcu);
            if(strPropertyName == tr(u8"RCU ID："))
                moduleBcuChanged.sort = value.toInt();
            if(strPropertyName == tr(u8"Router："))
                moduleBcuChanged.srcDataFromBcu = ModuleBcuIndex(value.toInt());
            else if(strPropertyName == tr(u8"Burst："))
                moduleBcuChanged.burst = value.toInt();
//            else if(strPropertyName == tr(u8"Gap："))
//                moduleBcuChanged.gap = value.toInt();
            else if(strPropertyName == tr(u8"Infifo来源："))
                moduleBcuChanged.infifoSrc = value.toInt();
            else if(strPropertyName == tr(u8"Outfifo输出："))
                moduleBcuChanged.outfifoSrc = value.toInt();
            else if(strPropertyName == tr(u8"LFSR分组："))
                moduleBcuChanged.lfsrGroup = value.toInt();
            else if(strPropertyName == tr(u8"128/32位选择："))
                moduleBcuChanged.readImdMode128_32 = value.toInt();
            else if(strPropertyName == tr(u8"Memory数据："))
                moduleBcuChanged.memorySrcConfig =  m_resourceKeys.at(value.toInt());
            else if(strPropertyName == tr(u8"InFifo数据："))
                moduleBcuChanged.inFifoSrcConfig =  m_resourceKeys.at(value.toInt());
            else if(strPropertyName == tr(u8"IMD数据："))
                moduleBcuChanged.imdSrcConfig =  m_resourceKeys.at(value.toInt());
            else if(strPropertyName == tr(u8"Depth："))
                moduleBcuChanged.writeDepth =  value.toInt();
            else if(strPropertyName == tr(u8"Width："))
            {
                int iIndex = value.toInt();
                int iValue = iIndex==0?0:iIndex==1?1:iIndex==2?8:iIndex==3?16:iIndex==4?32:0;
                moduleBcuChanged.writeWidth = iValue;
            }
            else if(strPropertyName == tr(u8"Address Mode(0x)："))
            {
                QRegExp regExp{"[^a-f0-9]+"};
                int iPos = regExp.indexIn(value.toString());
                if(value.toString().length() == 8 && iPos == -1)
                    moduleBcuChanged.addressMode =  value.toString();
                else
                    moduleBcuChanged.addressMode =  "00000000";
            }

            if(m_curRcaScene) {
                const QString &ret = m_curRcaScene->setBaseItemAttr(&moduleBcuChanged);
                if(!ret.isEmpty())
                {
                    ui->label_error->setText(ret);
                    QTimer::singleShot(1000, this, SLOT(reloadCurItemAttr()));
                }
            }
        }
    }
        break;
    case RCU_ID:
    {
        ModuleRcu* moduleRcu = dynamic_cast<ModuleRcu*>(m_curBaseItem);
        if(moduleRcu)
        {
            ModuleRcu moduleRcuChanged(*moduleRcu);
            if(strPropertyName == tr(u8"RCU CFG:"))
                moduleRcuChanged.indexRcu = value.toInt();
            else if(strPropertyName == tr(u8"Loop："))
            {
                if(validLoopChange(m_curRcaScene,moduleRcuChanged.getIndexRcu(),moduleRcuChanged.loopStartEndFlag,value.toInt()))
                    moduleRcuChanged.loopStartEndFlag = value.toInt();
                else
                {
                    QString strReturn;
                    int ret = QMessageBox::information(CAppEnv::m_mainWgt, tr(u8"RCU设置一级循环"),
                                                   tr(u8"一级循环发生嵌套，请检查后重新设置！"),
                                                   QMessageBox::Ok);
                    if (ret == QMessageBox::Ok)
                        strReturn =  QString(tr(u8"请重新检查！"));
                    ui->label_error->setText(strReturn);
                    QTimer::singleShot(1000, this, SLOT(reloadCurItemAttr()));
                }
            }
            else if(strPropertyName == tr(u8"Loop次数："))
                moduleRcuChanged.loopTimes = value.toInt();
            else if(strPropertyName == tr(u8"Loop2："))
            {
                if(validLoopChange(m_curRcaScene,moduleRcuChanged.getIndexRcu(),moduleRcuChanged.loopStartEndFlag2,value.toInt(),true))
                    moduleRcuChanged.loopStartEndFlag2 = value.toInt();
                else
                {
                    QString strReturn;
                    int ret = QMessageBox::information(CAppEnv::m_mainWgt, tr(u8"RCU设置二级循环"),
                                                   tr(u8"二级循环发生嵌套，请检查后重新设置！"),
                                                   QMessageBox::Ok);
                    if (ret == QMessageBox::Ok)
                        strReturn =  QString(tr(u8"请重新检查！"));
                    ui->label_error->setText(strReturn);
                    QTimer::singleShot(1000, this, SLOT(reloadCurItemAttr()));
                }
            }
            else if(strPropertyName == tr(u8"Loop2次数："))
                moduleRcuChanged.loopTimes2 = value.toInt();
            else if(strPropertyName == tr(u8"128/32位选择："))
                moduleRcuChanged.readMemMode128_32 = value.toInt();
            else if(strPropertyName == tr(u8"模式："))
                moduleRcuChanged.readMemMode = value.toInt();
            else if(strPropertyName == tr(u8"起始地址1(0x)："))
                moduleRcuChanged.readMemAddr1 = value.toInt();
            else if(strPropertyName == tr(u8"起始地址2(0x)："))
                moduleRcuChanged.readMemAddr2 = value.toInt();
            else if(strPropertyName == tr(u8"起始地址3(0x)："))
                moduleRcuChanged.readMemAddr3 = value.toInt();
            else if(strPropertyName == tr(u8"起始地址4(0x)："))
                moduleRcuChanged.readMemAddr4 = value.toInt();
            else if(strPropertyName == tr(u8"偏移(0x)："))
                moduleRcuChanged.readMemOffset = value.toInt();
            else if(strPropertyName == tr(u8"读写(阈值)："))
                moduleRcuChanged.readMemThreashold = value.toInt();
            else if(strPropertyName == tr(u8"2 模式："))
                moduleRcuChanged.writeMemMode = value.toInt();
            else if(strPropertyName == tr(u8"起始地址(0x)："))
                moduleRcuChanged.writeMemAddr = value.toInt();
            else if(strPropertyName == tr(u8"数据掩盖(0b)："))
                moduleRcuChanged.writeMemMask = value.toInt();
            else if(strPropertyName == tr(u8"2 偏移(0x)："))
                moduleRcuChanged.writeMemOffset = value.toInt();
            else if(strPropertyName == tr(u8"2 读写(阈值)："))
                moduleRcuChanged.writeMemThreashold = value.toInt();
            else if(strPropertyName == tr(u8"3 模式："))
                moduleRcuChanged.rchMode = value.toInt();

            if(m_curRcaScene) {
                const QString &ret = m_curRcaScene->setBaseItemAttr(&moduleRcuChanged);
                if(!ret.isEmpty())
                {
                    ui->label_error->setText(ret);
                    QTimer::singleShot(1000, this, SLOT(reloadCurItemAttr()));
                }
            }
        }
    }
        break;
    case BFU_ID:
    {
        ElementBfu* elementBfu = dynamic_cast<ElementBfu*>(m_curBaseItem);
        if(elementBfu)
        {
            ElementBfu elementBfuChanged(*elementBfu);
            elementBfuChanged.inPortA->setInputType(elementBfu->inPortA->getInputType());
            elementBfuChanged.inPortA->setInputIndex(elementBfu->inPortA->getInputIndex());
            elementBfuChanged.inPortB->setInputType(elementBfu->inPortB->getInputType());
            elementBfuChanged.inPortB->setInputIndex(elementBfu->inPortB->getInputIndex());
            elementBfuChanged.inPortT->setInputType(elementBfu->inPortT->getInputType());
            elementBfuChanged.inPortT->setInputIndex(elementBfu->inPortT->getInputIndex());
            if(strPropertyName == tr(u8"BFU ID："))
                elementBfuChanged.indexInRcu = value.toInt();
            else if(strPropertyName == tr(u8"算子："))
                elementBfuChanged.funcIndex = BfuFuncIndex(value.toInt());
            else if(strPropertyName == tr(u8"表达式："))
                elementBfuChanged.funcExp = value.toString();
            else if(strPropertyName == tr(u8"Bypass："))
                elementBfuChanged.bypassIndex = value.toInt();
            else if(strPropertyName == tr(u8"模："))
                elementBfuChanged.funcAuModIndex = value.toInt();
//            else if(strPropertyName == tr(u8"进位："))
//                elementBfuChanged.funcAuCarry = value.toBool();
            else if(strPropertyName == tr(u8"乘法模式："))
                elementBfuChanged.funcMuModIndex = value.toInt();
            else if(strPropertyName == tr(u8"Bypass表达式："))
                elementBfuChanged.bypassExp = value.toString();

            if(m_curRcaScene) {
                const QString &ret = m_curRcaScene->setBaseItemAttr(&elementBfuChanged);
                if(!ret.isEmpty())
                {
                    ui->label_error->setText(ret);
                    QTimer::singleShot(1000, this, SLOT(reloadCurItemAttr()));
                }
            }
        }
    }
        break;
    case CUSTOMPE_ID:
    {
        ElementCustomPE* elementCustomPE = dynamic_cast<ElementCustomPE*>(m_curBaseItem);
        if(elementCustomPE)
        {
            ElementCustomPE elementCustomPEChanged(*elementCustomPE);
            for(int i = 0;i < elementCustomPEChanged.inputNum();i++)
            {
                if(elementCustomPE->m_inputPortVector.at(i))
                    elementCustomPEChanged.m_inputPortVector.at(i)->setInputType(
                                elementCustomPE->m_inputPortVector.at(i)->getInputType());
            }
            for(int i = 0;i < elementCustomPEChanged.outputNum();i++)
            {
                if(elementCustomPE->m_outputPortVector.at(i))
                    elementCustomPEChanged.m_outputPortVector.at(i)->setInputType(
                                elementCustomPE->m_outputPortVector.at(i)->getInputType());
            }
            if(strPropertyName == tr(u8"BFU ID："))
                elementCustomPEChanged.indexInRcu = value.toInt();
            else if(strPropertyName == tr(u8"inputPortCount："))
                elementCustomPEChanged.setInputNum(value.toInt() + 1);
            else if(strPropertyName == tr(u8"outputPortCount："))
                elementCustomPEChanged.setOutputNum(value.toInt() + 1);
            else if(strPropertyName == tr(u8"算子："))
                elementCustomPEChanged.funcIndex = BfuFuncIndex(value.toInt());
            else if(strPropertyName == tr(u8"表达式："))
                elementCustomPEChanged.funcExp = value.toString();
            else if(strPropertyName == tr(u8"Bypass："))
                elementCustomPEChanged.bypassIndex = value.toInt();
            else if(strPropertyName == tr(u8"模："))
                elementCustomPEChanged.funcAuModIndex = value.toInt();
//            else if(strPropertyName == tr(u8"进位："))
//                elementBfuChanged.funcAuCarry = value.toBool();
            else if(strPropertyName == tr(u8"乘法模式："))
                elementCustomPEChanged.funcMuModIndex = value.toInt();
            else if(strPropertyName == tr(u8"Bypass表达式："))
                elementCustomPEChanged.bypassExp = value.toString();

            if(m_curRcaScene) {
                const QString &ret = m_curRcaScene->setBaseItemAttr(&elementCustomPEChanged);
                if(!ret.isEmpty())
                {
                    ui->label_error->setText(ret);
                    QTimer::singleShot(1000, this, SLOT(reloadCurItemAttr()));
                }
            }
        }
    }
        break;
    case SBOX_ID:
    {
        ElementSbox* elementSbox = dynamic_cast<ElementSbox*>(m_curBaseItem);
        if(elementSbox)
        {
            ElementSbox elementSboxChanged(*elementSbox);
            elementSboxChanged.inPort0->setInputType(elementSbox->inPort0->getInputType());
            elementSboxChanged.inPort0->setInputIndex(elementSbox->inPort0->getInputIndex());
            if(strPropertyName == tr(u8"SBOX ID："))
                elementSboxChanged.indexInRcu = value.toInt();
//            else if(strPropertyName == tr(u8"Group0："))
//                elementSboxChanged.groupCnt0 = SboxGroupValue(value.toInt());
//            else if(strPropertyName == tr(u8"Group1："))
//                elementSboxChanged.groupCnt1 = SboxGroupValue(value.toInt());
//            else if(strPropertyName == tr(u8"Group2："))
//                elementSboxChanged.groupCnt2 = SboxGroupValue(value.toInt());
//            else if(strPropertyName == tr(u8"Group3："))
//                elementSboxChanged.groupCnt3 = SboxGroupValue(value.toInt());
            else if(strPropertyName == tr(u8"Group："))
                elementSboxChanged.sboxGroup = value.toInt();
            else if(strPropertyName == tr(u8"模式："))
                elementSboxChanged.sboxMode = value.toInt();
            else if(strPropertyName == tr(u8"Byte Sel："))
                elementSboxChanged.sboxByteSel = value.toInt();
            else if(strPropertyName == tr(u8"自增模式："))
                elementSboxChanged.sboxIncreaseMode = value.toInt();
            else if(strPropertyName == tr(u8"数据源："))
                elementSboxChanged.srcConfig = m_resourceKeys.at(value.toInt());

            if(m_curRcaScene) {
                const QString &ret = m_curRcaScene->setBaseItemAttr(&elementSboxChanged);
                if(!ret.isEmpty())
                {
                    ui->label_error->setText(ret);
                    QTimer::singleShot(1000, this, SLOT(reloadCurItemAttr()));
                }
            }
        }
    }
        break;
    case BENES_ID:
    {
        ElementBenes* elementBenes = dynamic_cast<ElementBenes*>(m_curBaseItem);
        if(elementBenes)
        {
            ElementBenes elementBenesChanged(*elementBenes);
            elementBenesChanged.inPort0->setInputType(elementBenes->inPort0->getInputType());
            elementBenesChanged.inPort0->setInputIndex(elementBenes->inPort0->getInputIndex());
            elementBenesChanged.inPort1->setInputType(elementBenes->inPort1->getInputType());
            elementBenesChanged.inPort1->setInputIndex(elementBenes->inPort1->getInputIndex());
            elementBenesChanged.inPort2->setInputType(elementBenes->inPort2->getInputType());
            elementBenesChanged.inPort2->setInputIndex(elementBenes->inPort2->getInputIndex());
            elementBenesChanged.inPort3->setInputType(elementBenes->inPort3->getInputType());
            elementBenesChanged.inPort3->setInputIndex(elementBenes->inPort3->getInputIndex());
            if(strPropertyName == tr(u8"BENES ID："))
                elementBenesChanged.indexInRcu = value.toInt();
            else if(strPropertyName == tr(u8"数据源："))
                elementBenesChanged.srcConfig =  m_resourceKeys.at(value.toInt());

            if(m_curRcaScene) {
                const QString &ret = m_curRcaScene->setBaseItemAttr(&elementBenesChanged);
                if(!ret.isEmpty())
                {
                    ui->label_error->setText(ret);
                    QTimer::singleShot(1000, this, SLOT(reloadCurItemAttr()));
                }
            }
        }
    }
        break;
    case RIMD_ID:
    {
        ModuleReadImd* moduleRImd = dynamic_cast<ModuleReadImd*>(m_curBaseItem);
        if(moduleRImd)
        {
            if(strPropertyName == tr(u8"IMD PORT："))
            {
                ModuleReadImd moduleRImdChanged(value.toInt());

                if(m_curRcaScene) {
                    const QString &ret = m_curRcaScene->setBaseItemAttr(&moduleRImdChanged);
                    if(!ret.isEmpty())
                    {
                        ui->label_error->setText(ret);
                        QTimer::singleShot(1000, this, SLOT(reloadCurItemAttr()));
                    }
                }
            }
        }
    }
        break;
    case RMEM_ID:
    {
        ModuleReadMem* moduleRMem = dynamic_cast<ModuleReadMem*>(m_curBaseItem);
        if(moduleRMem)
        {
            if(strPropertyName == tr(u8"MEM PORT："))
            {
                ModuleReadMem moduleRMemChanged(value.toInt());

                if(m_curRcaScene) {
                    const QString &ret = m_curRcaScene->setBaseItemAttr(&moduleRMemChanged);
                    if(!ret.isEmpty())
                    {
                        ui->label_error->setText(ret);
                        QTimer::singleShot(1000, this, SLOT(reloadCurItemAttr()));
                    }
                }
            }
        }
    }
        break;
    case WMEM_ID:
    {
        ModuleWriteMem* moduleWMem = dynamic_cast<ModuleWriteMem*>(m_curBaseItem);
        if(moduleWMem)
        {
            if(strPropertyName == tr(u8"MEM PORT："))
            {
                ModuleWriteMem moduleWMemChanged(value.toInt());

                if(m_curRcaScene) {
                    const QString &ret = m_curRcaScene->setBaseItemAttr(&moduleWMemChanged);
                    if(!ret.isEmpty())
                    {
                        ui->label_error->setText(ret);
                        QTimer::singleShot(1000, this, SLOT(reloadCurItemAttr()));
                    }
                }
            }
        }
    }
        break;
    case RRCH_ID:
    {
        ModuleReadRch* moduleRRch = dynamic_cast<ModuleReadRch*>(m_curBaseItem);
        if(moduleRRch)
        {
            if(strPropertyName == tr(u8"RCH PORT："))
            {
                ModuleReadRch moduleRRchChanged(value.toInt());

                if(m_curRcaScene) {
                    const QString &ret = m_curRcaScene->setBaseItemAttr(&moduleRRchChanged);
                    if(!ret.isEmpty())
                    {
                        ui->label_error->setText(ret);
                        QTimer::singleShot(1000, this, SLOT(reloadCurItemAttr()));
                    }
                }
            }
        }
    }
        break;
    case WRCH_ID:
    {
        ModuleWriteRch* moduleWRch = dynamic_cast<ModuleWriteRch*>(m_curBaseItem);
        if(moduleWRch)
        {
            if(strPropertyName == tr(u8"RCH PORT："))
            {
                ModuleWriteRch moduleWRchChanged(value.toInt());
                if(m_curRcaScene)
                {
                    const QString &ret = m_curRcaScene->setBaseItemAttr(&moduleWRchChanged);
                    if(!ret.isEmpty())
                    {
                        ui->label_error->setText(ret);
                        QTimer::singleShot(1000, this, SLOT(reloadCurItemAttr()));
                    }
                }
            }
            else if(strPropertyName == tr(u8"写地址："))
            {
                ModuleWriteRch moduleWRchChanged(moduleWRch->getWriteRchIndex());
                moduleWRchChanged.setWriteAddress(value.toInt());

                if(m_curRcaScene)
                {
                    const QString &ret = m_curRcaScene->setBaseItemAttr(&moduleWRchChanged);
                    if(!ret.isEmpty())
                    {
                        ui->label_error->setText(ret);
                        QTimer::singleShot(1000, this, SLOT(reloadCurItemAttr()));
                    }
                }
            }
        }
    }
        break;
    case RLFSR_ID:
    {
        ModuleReadLfsr* moduleRLfsr = dynamic_cast<ModuleReadLfsr*>(m_curBaseItem);
        if(moduleRLfsr)
        {
            if(strPropertyName == tr(u8"LFSR PORT："))
            {
                ModuleReadLfsr moduleRLfsrChanged(value.toInt());

                if(m_curRcaScene) {
                    const QString &ret = m_curRcaScene->setBaseItemAttr(&moduleRLfsrChanged);
                    if(!ret.isEmpty())
                    {
                        ui->label_error->setText(ret);
                        QTimer::singleShot(1000, this, SLOT(reloadCurItemAttr()));
                    }
                }
            }
        }
    }
        break;
    case WLFSR_ID:
    {
        ModuleWriteLfsr* moduleWLfsr = dynamic_cast<ModuleWriteLfsr*>(m_curBaseItem);
        if(moduleWLfsr)
        {
            if(strPropertyName == tr(u8"LFSR PORT："))
            {
                ModuleWriteLfsr moduleWLfsrChanged(value.toInt());
                if(m_curRcaScene)
                {
                    const QString &ret = m_curRcaScene->setBaseItemAttr(&moduleWLfsrChanged);
                    if(!ret.isEmpty())
                    {
                        ui->label_error->setText(ret);
                        QTimer::singleShot(1000, this, SLOT(reloadCurItemAttr()));
                    }
                }
            }
        }
    }
        break;
    default:
        break;
    }
}
