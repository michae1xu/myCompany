#include "ModuleWriteMem.h"

#include "RcaGraphScene.h"
#include <QDebug>


ModuleWriteMem::ModuleWriteMem(int index, QGraphicsItem *parent) : BasePort (parent,PORT_FROM_WMEM)
    , paintPath(QPainterPath())
    , mapMemRect(QRectF())
    , mapMemLine(QLineF())
    , writeMemIndex(index)
    , inputType(InputPort_NULL)
    , inputIndex(0)
{
    unitId = WMEM_ID;
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
//    setFlag(QGraphicsItem::ItemIsMovable);
    isPartOfChildRect = false;
    isEnabledAdjustRect = false;
    realBoundingRect = MODULE_WRITE_MEM_DEFAULT_RECT;
    paintRect = MODULE_WRITE_MEM_DEFAULT_PAINT_RECT;

    mapMemRect = QRectF(paintRect.left(),paintRect.bottom()-MAP_MEM_DEFAULT_HEIGHT,
                         MAP_MEM_DEFAULT_WIDTH,MAP_MEM_DEFAULT_HEIGHT);
    mapMemLine = QLineF(QPointF(paintRect.left()+paintRect.width()/2,paintRect.top()),
                       QPointF(paintRect.left()+paintRect.width()/2,paintRect.bottom() - MAP_MEM_DEFAULT_HEIGHT));


//    qreal ellipseDiameter = paintRect.width();
//    mapMemRect = QRectF(paintRect.left(),paintRect.bottom()-ellipseDiameter,
//                         ellipseDiameter,ellipseDiameter);
//    paintPath.moveTo(paintRect.left()+paintRect.width()/2,
//                     paintRect.top());
//    paintPath.lineTo(paintRect.left()+paintRect.width()/2,
//                     paintRect.bottom() - ellipseDiameter);
//    paintPath.moveTo(paintRect.left()+paintRect.width()/2,
//                     paintRect.bottom() - ellipseDiameter);
//    paintPath.lineTo(paintRect.left(),
//                     paintRect.bottom() - 2*ellipseDiameter);
//    paintPath.moveTo(paintRect.left()+paintRect.width()/2,
//                     paintRect.bottom() - ellipseDiameter);
//    paintPath.lineTo(paintRect.right(),
//                     paintRect.bottom() - 2*ellipseDiameter);
//    paintPath.addEllipse(mapMemRect);

}

ModuleWriteMem::~ModuleWriteMem()
{
//    QL_DEBUG<<"~ModuleWriteMem()";
}

//QPainterPath ModuleWriteMem::shape() const
//{
//    return paintPath;
//}

void ModuleWriteMem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setRenderHints(QPainter::Antialiasing|
                           QPainter::TextAntialiasing);
    if(isSelected())
    {
        painter->setBrush(MODULE_MEM_FOCUSED_BRUSH_FILLIN);
        painter->setPen(MODULE_MEM_BOUNDING_PEN);
        painter->fillRect(realBoundingRect,MODULE_MEM_FOCUSED_BRUSH_FILLIN);
        painter->drawRect(realBoundingRect);
    }
    painter->setBrush(MODULE_WRITE_MEM_BRUSH);
//    painter->setPen(MODULE_WRITE_MEM_LINE_PEN);
    painter->setPen(MODULE_WRITE_MEM_PEN);
    painter->drawLine(mapMemLine);

    painter->setPen(MODULE_WRITE_MEM_PEN);
    painter->drawRect(mapMemRect);

    QFont font;
    font.setPixelSize(MODULE_MEM_TEXT_PIXELSIZE);
    painter->setFont(font);
    painter->setPen(MODULE_MEM_TEXT_PEN);
    painter->drawText(realBoundingRect, Qt::AlignBottom | Qt::AlignHCenter, QString::number(writeMemIndex));

}

bool ModuleWriteMem::setWriteMemIndex(int index)
{
    if(index == this->writeMemIndex) return true;
    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        if(parentItem())
        {
            ElementPort* parentPort = dynamic_cast<ElementPort*>(parentItem());
            if(parentPort && parentPort->parentItem())
            {
                BaseArithUnit* parentBaseArith = dynamic_cast<BaseArithUnit*>(parentPort->parentItem());
                ModuleRcu* findedRcu = rcaScene->findRcuFromScene(parentBaseArith->getIndexBcu(),parentBaseArith->getIndexRcu());
                if(findedRcu)
                {
                    if(findedRcu->isWMemFulled) return false;   //add write mem failed when write mem fulled.
                    for(int i=0;i<findedRcu->wMemExistList.size();i++)
                    {
                        if(index == findedRcu->wMemExistList.at(i))
                        {
                            return false; //已经存在此mem
                        }
                    }
                    QL_DEBUG << findedRcu->wMemExistList;
                    findedRcu->wMemExistList.removeOne(writeMemIndex);
                    emit rcaScene->deleteWriteMemSuccess(findedRcu->getIndexBcu(),findedRcu->getIndexRcu(),this);
                    writeMemIndex = index;

                    int insertIndex = findedRcu->wMemExistList.size();
                    for(int i=0;i<findedRcu->wMemExistList.size();i++)
                    {
                        if(writeMemIndex < findedRcu->wMemExistList.at(i))
                        {
                            insertIndex = i;
                            break;
                        }
                    }

                    findedRcu->wMemExistList.insert(insertIndex,writeMemIndex);
                    QL_DEBUG << findedRcu->wMemExistList;
                    return true;
                }
            }
        }
    }
    return false;
}

QString ModuleWriteMem::canSetWriteMemIndex(int index)
{
    if(index == this->writeMemIndex) return QString(tr(u8"超过mem的最大值"));

    RcaGraphScene* rcaScene= qobject_cast<RcaGraphScene*>(scene());
    if(rcaScene)
    {
        if(parentItem())
        {
            ElementPort* parentPort = dynamic_cast<ElementPort*>(parentItem());
            if(parentPort && parentPort->parentItem())
            {
                BaseArithUnit* parentBaseArith = dynamic_cast<BaseArithUnit*>(parentPort->parentItem());
                ModuleRcu* findedRcu = rcaScene->findRcuFromScene(parentBaseArith->getIndexBcu(),parentBaseArith->getIndexRcu());
                if(findedRcu)
                {
                    if(findedRcu->isWMemFulled) return QString(tr(u8"mem已存满"));   //add write mem failed when write mem fulled.
                    for(int i=0;i<findedRcu->wMemExistList.size();i++)
                    {
                        if(index == findedRcu->wMemExistList.at(i))
                        {
                            return QString(tr(u8"已经存在此mem")); //已经存在此mem
                        }
                    }
                    return QString(tr(u8""));
                }
            }
        }
    }
    return QString(tr(u8"没有scene"));
}

QVariant ModuleWriteMem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    return QGraphicsItem::itemChange(change, value);
}

InputPortType ModuleWriteMem::getInputType()
{
    return inputType;
}

void ModuleWriteMem::setInputType(InputPortType type)
{
    inputType = type;
}

int ModuleWriteMem::getInputIndex()
{
     return inputIndex;
}

void ModuleWriteMem::setInputIndex(int index)
{
    inputIndex = index;
}
