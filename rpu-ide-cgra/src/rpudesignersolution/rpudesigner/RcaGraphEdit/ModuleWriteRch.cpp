#include "ModuleWriteRch.h"

#include "RcaGraphScene.h"
#include <QDebug>


ModuleWriteRch::ModuleWriteRch(int index, QGraphicsItem *parent) : BasePort (parent,PORT_FROM_WRCH)
    , paintPath(QPainterPath())
    , mapRchRect(QRectF())
    , mapRchLine(QLineF())
    , writeRchIndex(index)
    , writeAddress(0)
//    , indexRcu(rcuid)
//    , indexBcu(bcuid)
    , inputType(InputPort_NULL)
    , inputIndex(0)
{
    unitId = WRCH_ID;
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
//    setFlag(QGraphicsItem::ItemIsMovable);
    isPartOfChildRect = false;
    isEnabledAdjustRect = false;
    realBoundingRect = MODULE_WRITE_RCH_DEFAULT_RECT;
    paintRect = MODULE_WRITE_RCH_DEFAULT_PAINT_RECT;

    mapRchRect = QRectF(paintRect.left(),paintRect.bottom()-MAP_RCH_DEFAULT_HEIGHT,
                         MAP_RCH_DEFAULT_WIDTH,MAP_RCH_DEFAULT_HEIGHT);
    mapRchLine = QLineF(QPointF(paintRect.left()+paintRect.width()/2,paintRect.top()),
                       QPointF(paintRect.left()+paintRect.width()/2,paintRect.bottom() - MAP_RCH_DEFAULT_HEIGHT));


//    qreal ellipseDiameter = paintRect.width();
//    mapRchRect = QRectF(paintRect.left(),paintRect.bottom()-ellipseDiameter,
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
//    paintPath.addEllipse(mapRchRect);

}

ModuleWriteRch::~ModuleWriteRch()
{
    QL_DEBUG<<"~ModuleWriteRch()";
}

//QPainterPath ModuleWriteRch::shape() const
//{
//    return paintPath;
//}

void ModuleWriteRch::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setRenderHints(QPainter::Antialiasing|
                           QPainter::TextAntialiasing);
    if(isSelected())
    {
        painter->setBrush(MODULE_RCH_FOCUSED_BRUSH_FILLIN);
        painter->setPen(MODULE_RCH_BOUNDING_PEN);
        painter->fillRect(realBoundingRect,MODULE_RCH_FOCUSED_BRUSH_FILLIN);
        painter->drawRect(realBoundingRect);
    }
    painter->setBrush(MODULE_WRITE_RCH_BRUSH);
//    painter->setPen(MODULE_WRITE_RCH_LINE_PEN);
    painter->setPen(MODULE_WRITE_RCH_PEN);
    painter->drawLine(mapRchLine);

    painter->setPen(MODULE_WRITE_RCH_PEN);
    painter->drawRect(mapRchRect);

    QFont font;
    font.setPixelSize(MODULE_RCH_TEXT_PIXELSIZE);
    painter->setFont(font);
    painter->setPen(MODULE_RCH_TEXT_PEN);
    painter->drawText(realBoundingRect, Qt::AlignBottom | Qt::AlignHCenter, QString("R:%1").arg(writeRchIndex));

}

bool ModuleWriteRch::setWriteRchIndex(int index)
{
    if(index == this->writeRchIndex) return true;
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
                    if(findedRcu->isWRchFulled) return false;   //add write rch failed when write rch fulled.
//                    int rcuIndex = findedRcu->getIndexRcu();
//                    if(index < rcuIndex*RCU_WRITE_RCH_MAX_COUNT ||
//                            index >= (rcuIndex+1)*RCU_WRITE_RCH_MAX_COUNT)
//                    {
//                        return false;
//                    }

                    for(int i=0;i<findedRcu->wRchExistList.size();i++)
                    {
                        if(index == findedRcu->wRchExistList.at(i))
                        {
                            return false; //已经存在此rch
                        }
                    }

                    QL_DEBUG << findedRcu->wRchExistList;
                    findedRcu->wRchExistList.removeOne(writeRchIndex);

                    writeRchIndex = index;

                    int insertIndex = findedRcu->wRchExistList.size();
                    for(int i=0;i<findedRcu->wRchExistList.size();i++)
                    {
                        if(writeRchIndex < findedRcu->wRchExistList.at(i))
                        {
                            insertIndex = i;
                            break;
                        }
                    }

                    findedRcu->wRchExistList.insert(insertIndex,writeRchIndex);
                    QL_DEBUG << findedRcu->wRchExistList;
                    return true;
                }
            }
        }
    }
    return false;
}

QString ModuleWriteRch::canSetWriteRchIndex(int index)
{
    if(index == this->writeRchIndex) return QString(tr(u8""));

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
                    if(findedRcu->isWRchFulled) return QString(tr(u8"rch已存满"));   //add write rch failed when write rch fulled.
//                    int bcuIndex = findedRcu->getIndexBcu();
//                    if(index < bcuIndex*RCU_WRITE_RCH_MAX_COUNT ||
//                            index >= (bcuIndex+1)*RCU_WRITE_RCH_MAX_COUNT)
//                    {
//                        return QString(tr(u8"rch序号不符合规定"));
//                    }

                    for(int i=0;i<findedRcu->wRchExistList.size();i++)
                    {
                        if(index == findedRcu->wRchExistList.at(i))
                        {
                            return QString(tr(u8"已经存在此rch")); //已经存在此rch
                        }
                    }
                    return QString(tr(u8""));
                }
            }
        }
    }
    return QString(tr(u8"没有scene"));
}

QVariant ModuleWriteRch::itemChange(GraphicsItemChange change, const QVariant &value)
{
    return QGraphicsItem::itemChange(change, value);
}

InputPortType ModuleWriteRch::getInputType()
{
    return inputType;
}

void ModuleWriteRch::setInputType(InputPortType type)
{
    inputType = type;
}

int ModuleWriteRch::getInputIndex()
{
     return inputIndex;
}

void ModuleWriteRch::setInputIndex(int index)
{
    inputIndex = index;
}

int ModuleWriteRch::getWriteAddress() const
{
    return writeAddress;
}

void ModuleWriteRch::setWriteAddress(int value)
{
    writeAddress = value;
}

//int ModuleWriteRch::getRcuIndex()
//{
//    if(parentItem())
//    {
//        ElementPort* parentPort = dynamic_cast<ElementPort*>(parentItem());
//        if(parentPort && parentPort->parentItem())
//        {
//            BaseArithUnit* parentBaseArith = dynamic_cast<BaseArithUnit*>(parentPort->parentItem());
//            if(parentBaseArith)
//            {
//                return parentBaseArith->getIndexRcu();
//            }

//        }
//    }
//    return 0;
//}

//void ModuleWriteRch::setIndexRcu(int value)
//{
//    indexRcu = value;
//}

//int ModuleWriteRch::getIndexBcu() const
//{
//    return indexBcu;
//}
