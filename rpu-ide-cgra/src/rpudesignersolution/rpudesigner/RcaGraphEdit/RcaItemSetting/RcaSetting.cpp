#include "RcaSetting.h"

#include "ModuleBcu.h"
#include "ModuleRcu.h"
#include "ElementBfu.h"
#include "ElementSbox.h"
#include "ElementBenes.h"
#include "ModuleReadMem.h"
#include "ModuleWriteMem.h"

RcaSetting* RcaSetting::rcaSetingInstance = NULL;
//RcaGraphScene* RcaSetting::rcaSceneInstance = NULL;

RcaSetting::RcaSetting(QWidget *parent) : QDialog(parent)
    , bcuSetting(NULL)
    , rcuSetting(NULL)
    , bfuSetting(NULL)
    , sboxSetting(NULL)
    , benesSetting(NULL)
    , rwMemSetting(NULL)
    , showPos(QPoint())
    , currentShowItem(NULL)
{
    rcaSetingInstance = this;

    bcuSetting = RcaBcuSetting::instance();
    bcuSetting->setModal(false);
//    bcuSetting->setRcaScene(rcaSceneInstance);

    rcuSetting = RcaRcuSetting::instance();
    rcuSetting->setModal(false);
//    rcuSetting->setRcaScene(rcaSceneInstance);

    bfuSetting = RcaBfuSetting::instance();
    bfuSetting->setModal(false);
//    bfuSetting->setRcaScene(rcaSceneInstance);

    sboxSetting = RcaSboxSetting::instance();
    sboxSetting->setModal(false);
//    sboxSetting->setRcaScene(rcaSceneInstance);

    benesSetting = RcaBenesSetting::instance();
    benesSetting->setModal(false);
//    benesSetting->setRcaScene(rcaSceneInstance);

    rwMemSetting = RcaRWMemSetting::instance();
    rwMemSetting->setModal(false);
//    rwMemSetting->setRcaScene(rcaSceneInstance);
}

void RcaSetting::resetResourceMap(RcaGraphScene* scene)
{
    sboxSetting->resetResourceMap(scene);
    benesSetting->resetResourceMap(scene);
}

void RcaSetting::hideAll()
{
    if(!bcuSetting->isHidden())
    {
        showPos = bcuSetting->pos();
        currentShowItem = bcuSetting->getItem();
        bcuSetting->hide();
    }
    else if(!rcuSetting->isHidden())
    {
        showPos = rcuSetting->pos();
        currentShowItem = rcuSetting->getItem();
        rcuSetting->hide();
    }
    else if(!bfuSetting->isHidden())
    {
        showPos = bfuSetting->pos();
        currentShowItem = bfuSetting->getItem();
        bfuSetting->hide();
    }
    else if(!sboxSetting->isHidden())
    {
        showPos = sboxSetting->pos();
        currentShowItem = sboxSetting->getItem();
        sboxSetting->hide();
    }
    else if(!benesSetting->isHidden())
    {
        showPos = benesSetting->pos();
        currentShowItem = benesSetting->getItem();
        benesSetting->hide();
    }
    else if(!rwMemSetting->isHidden())
    {
        showPos = rwMemSetting->pos();
        currentShowItem = rwMemSetting->getItem();
        rwMemSetting->hide();
    }
    else
    {
        currentShowItem = NULL;
    }
}
void RcaSetting::showCurrentItem()
{
    QL_DEBUG << currentShowItem;
    if(currentShowItem)
    {
        show(currentShowItem);
    }
}

void RcaSetting::show(BaseItem* baseItem)
{
    if(baseItem == NULL) return;
    hideAll();

    ModuleBcu* moduleBcu = dynamic_cast<ModuleBcu*>(baseItem);
    ModuleRcu* moduleRcu = dynamic_cast<ModuleRcu*>(baseItem);
    ElementBfu* elementBfu = dynamic_cast<ElementBfu*>(baseItem);
    ElementSbox* elementSbox = dynamic_cast<ElementSbox*>(baseItem);
    ElementBenes* elementBenes = dynamic_cast<ElementBenes*>(baseItem);
    ModuleReadMem* readMem = dynamic_cast<ModuleReadMem*>(baseItem);
    ModuleWriteMem* writeMem = dynamic_cast<ModuleWriteMem*>(baseItem);
    if(moduleBcu)
    {
        bcuSetting->showBcu(moduleBcu,showPos);
    }
    else if(moduleRcu)
    {
        rcuSetting->showRcu(moduleRcu,showPos);
    }
    else if(elementBfu)
    {
        bfuSetting->showBfu(elementBfu,showPos);
    }
    else if(elementSbox)
    {
        sboxSetting->showSbox(elementSbox,showPos);
    }
    else if(elementBenes)
    {
        benesSetting->showBenes(elementBenes,showPos);
    }
    else if(readMem)
    {
        rwMemSetting->showRWMem(readMem,showPos);
    }
    else if(writeMem)
    {
        rwMemSetting->showRWMem(writeMem,showPos);
    }

}

RcaSetting* RcaSetting::instance(RcaGraphScene *scene, QWidget *parent)
{
    Q_UNUSED(scene);
//    if((rcaSceneInstance == NULL) && (scene == NULL)) return NULL;
//    if(rcaSceneInstance == NULL)
//    {
//        rcaSceneInstance = scene;
//    }

    if ( !rcaSetingInstance )
    {
        rcaSetingInstance = new RcaSetting(parent);
    }
    return rcaSetingInstance;
}
