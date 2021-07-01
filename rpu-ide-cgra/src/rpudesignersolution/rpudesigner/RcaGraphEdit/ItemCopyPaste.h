#ifndef ITEMCOPYPASTE_H
#define ITEMCOPYPASTE_H

#include "RcaGraphDef.h"
#include "BaseItem.h"
#include "ElementWire.h"
#include "ModuleBcu.h"
#include "ModuleRcu.h"
#include "BaseArithUnit.h"
#include "ElementBfu.h"
#include "ElementSbox.h"
#include "ElementBenes.h"

#include <QGraphicsItem>
#include <QList>
#include <QMap>
#include <QRectF>

using namespace rca_space;

class RcaGraphScene;

class ItemCopyPaste : public QObject
{
    Q_OBJECT
public:
    ItemCopyPaste(QObject *parent); //parent不要设置默认值，有QT负责回收内存
    ~ItemCopyPaste();
    void copyItems(QList<QGraphicsItem*> itemList, ModuleUnitId copyUnitId,bool hasWire);
//    void cutItems(QList<QGraphicsItem*> itemList);
    void pasteItems(RcaGraphScene * rcaScene,QGraphicsItem* hoverItem, QPointF posAtThis);
    bool canPaste();
    void clearItemsList();
    static ItemCopyPaste* instance(QObject *parent);
    static void deleteInstance();

private:
    void copyWireFromSelectedItems();
    void copyWireFrom2Port(ElementPort& inPort,ElementPort& outPort);
    void pasteWireWithCopy(ElementWire& wire, RcaGraphScene &rcaScene);
    ElementPort* getCopyPortFromSelectedPort(ElementPort& elePort);
    ElementPort* getPastePortFromCopyPort(ElementPort& elePort);
    ModuleBcu* getCopyBcu(int indexBcu);
    ModuleRcu* getCopyRcu(int indexBcu,int indexRcu);
    ElementBfu* getCopyBfu(int indexBcu,int indexRcu,int index);
    ElementCustomPE* getCopyCustomPE(int indexBcu,int indexRcu,int index);
    ElementSbox* getCopySbox(int indexBcu,int indexRcu,int index);
    ElementBenes* getCopyBenes(int indexBcu,int indexRcu,int index);
    void copyWireFromSelectedBcu(ModuleBcu *bcuItem);
    void copyWireFromSelectedRcu(ModuleRcu* rcuItem);
    void copyWireFromSelectedArith(BaseArithUnit* arithItem);
    void mapCopyBcuPasteBcu(ModuleBcu &copyBcu,ModuleBcu &pasteBcu);
    void mapCopyRcuPasteRcu(ModuleRcu &copyRcu,ModuleRcu &pasteRcu);
    void sortHadCopyedItems();
signals:
    void pasteStateChanged(bool isEnabled);

private:
    QList<BaseItem*> selectedItemsList;
    QList<BaseItem*> hadCopyItemsList;
    QList<BaseItem*> hadPasteItemsList;
    QList<ElementWire*> hadCopyWiresList;
    static ItemCopyPaste* m_instance;
    ModuleUnitId hadCopyUnitId;
    QMap<BaseItem*,BaseItem*> mapCopyAndPaste;
    QRectF copyItemsRect;
};

#endif // ITEMCOPYPASTE_H
