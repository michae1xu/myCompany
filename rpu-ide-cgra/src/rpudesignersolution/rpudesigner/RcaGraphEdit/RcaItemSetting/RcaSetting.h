#ifndef RCASETTING_H
#define RCASETTING_H

#include "RcaBcuSetting.h"
#include "RcaRcuSetting.h"
#include "RcaBfuSetting.h"
#include "RcaSboxSetting.h"
#include "RcaBenesSetting.h"
#include "RcaRWMemSetting.h"

#include "BaseItem.h"

#include <QObject>
#include <QDialog>
#include <QWidget>
#include "RcaGraphScene.h"

class RcaSetting : public QDialog
{
    Q_OBJECT
public:
    RcaSetting(QWidget *parent = 0);
    void show(BaseItem* baseItem);
    void showCurrentItem();
    static RcaSetting* instance(RcaGraphScene* scene = 0,QWidget *parent = 0);
    void hideAll();
    void resetResourceMap(RcaGraphScene* scene);
private:
    static RcaSetting* rcaSetingInstance;
//    static RcaGraphScene* rcaSceneInstance;
    RcaBcuSetting* bcuSetting;
    RcaRcuSetting* rcuSetting;
    RcaBfuSetting* bfuSetting;
    RcaSboxSetting* sboxSetting;
    RcaBenesSetting* benesSetting;
    RcaRWMemSetting* rwMemSetting;
    QPoint showPos;
    BaseItem* currentShowItem;
};

#endif // RCASETTING_H
