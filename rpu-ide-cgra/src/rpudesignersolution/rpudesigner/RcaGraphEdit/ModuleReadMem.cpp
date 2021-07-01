#include "ModuleReadMem.h"

#include "ElementPort.h"
#include <QDebug>


ModuleReadMem::ModuleReadMem(int index, QGraphicsItem *parent) : BasePort (parent,PORT_FROM_RMEM)
    , paintPath(QPainterPath())
    , mapMemRect(QRectF())
    , mapMemLine(QLineF())
    , readMemIndex(index)
{
    unitId = RMEM_ID;
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
//    setFlag(QGraphicsItem::ItemIsMovable);
    isPartOfChildRect = false;
    isEnabledAdjustRect = false;
    realBoundingRect = MODULE_READ_MEM_DEFAULT_RECT;
    paintRect = MODULE_READ_MEM_DEFAULT_PAINT_RECT;

    mapMemRect = QRectF(paintRect.left(),paintRect.top(),
                         MAP_MEM_DEFAULT_WIDTH,MAP_MEM_DEFAULT_HEIGHT);
    mapMemLine = QLineF(QPointF(paintRect.left()+paintRect.width()/2,paintRect.top()+MAP_MEM_DEFAULT_HEIGHT),
                       QPointF(paintRect.left()+paintRect.width()/2,paintRect.bottom()));


//    qreal ellipseDiameter = paintRect.width();
//    mapMemRect = QRectF(paintRect.left(),paintRect.top(),
//                         ellipseDiameter,ellipseDiameter);
//    paintPath.addEllipse(mapMemRect);
//    paintPath.moveTo(paintRect.left()+paintRect.width()/2,
//                     paintRect.top()+ellipseDiameter);
//    paintPath.lineTo(paintRect.left()+paintRect.width()/2,
//                     paintRect.bottom());
//    paintPath.moveTo(paintRect.left()+paintRect.width()/2,
//                     paintRect.bottom());
//    paintPath.lineTo(paintRect.left(),
//                     paintRect.bottom() - ellipseDiameter);
//    paintPath.moveTo(paintRect.left()+paintRect.width()/2,
//                     paintRect.bottom());
//    paintPath.lineTo(paintRect.right(),
//                     paintRect.bottom() - ellipseDiameter);

}

ModuleReadMem::~ModuleReadMem()
{
//    QL_DEBUG<<"~ModuleReadMem()";
}

//QPainterPath ModuleReadMem::shape() const
//{
//    return paintPath;
//}

void ModuleReadMem::setReadMemIndex(int index)
{
    if(index >= READ_MEM_MAX_COUNT)  return;

    if(parentItem())
    {
        ElementPort* parentPort = dynamic_cast<ElementPort*>(parentItem());
        if(parentPort)
        {
            parentPort->setInputIndex(index);
            readMemIndex = index;
        }
    }
}

bool ModuleReadMem::canSetReadMemIndex(int index)
{
    if(index >= READ_MEM_MAX_COUNT)  return false;

    if(parentItem())
    {
        ElementPort* parentPort = dynamic_cast<ElementPort*>(parentItem());
        if(parentPort)
        {
            return true;
        }
    }
    return false;
}

void ModuleReadMem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setRenderHints(QPainter::Antialiasing|
                           QPainter::TextAntialiasing);
    if(isSelected())
    {
        painter->setBrush(MODULE_MEM_FOCUSED_BRUSH_FILLIN);
        painter->setPen(MODULE_MEM_BOUNDING_PEN);
        painter->fillRect(realBoundingRect,BCU_COLLIDED_BRUSH_FILLIN);
        painter->fillRect(realBoundingRect,MODULE_MEM_FOCUSED_BRUSH_FILLIN);
        painter->drawRect(realBoundingRect);
    }
    painter->setBrush(MODULE_READ_MEM_BRUSH);
    painter->setPen(MODULE_READ_MEM_PEN);
    painter->drawRect(mapMemRect);

//    painter->setPen(MODULE_READ_MEM_LINE_PEN);
    painter->setPen(MODULE_READ_MEM_PEN);
    painter->drawLine(mapMemLine);

    QFont font;
    font.setPixelSize(MODULE_MEM_TEXT_PIXELSIZE);
    painter->setFont(font);
    painter->setPen(MODULE_MEM_TEXT_PEN);
    painter->drawText(realBoundingRect, Qt::AlignTop | Qt::AlignHCenter, QString::number(readMemIndex));
}

QVariant ModuleReadMem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    return QGraphicsItem::itemChange(change, value);
}
