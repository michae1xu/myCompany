#include "ModuleReadImd.h"

#include "ElementPort.h"
#include <QDebug>


ModuleReadImd::ModuleReadImd(int index, QGraphicsItem *parent) : BasePort (parent,PORT_FROM_RIMD)
    , paintPath(QPainterPath())
    , mapImdRect(QRectF())
    , mapImdLine(QLineF())
    , readImdIndex(index)
{
    unitId = RIMD_ID;
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
//    setFlag(QGraphicsItem::ItemIsMovable);
    isPartOfChildRect = false;
    isEnabledAdjustRect = false;
    realBoundingRect = MODULE_READ_IMD_DEFAULT_RECT;
    paintRect = MODULE_READ_IMD_DEFAULT_PAINT_RECT;

    mapImdRect = QRectF(paintRect.left(),paintRect.top(),
                         MAP_IMD_DEFAULT_WIDTH,MAP_IMD_DEFAULT_HEIGHT);
    mapImdLine = QLineF(QPointF(paintRect.left()+paintRect.width()/2,paintRect.top()+MAP_IMD_DEFAULT_HEIGHT),
                       QPointF(paintRect.left()+paintRect.width()/2,paintRect.bottom()));
}

ModuleReadImd::~ModuleReadImd()
{
//    QL_DEBUG<<"~ModuleReadImd()";
}

//QPainterPath ModuleReadImd::shape() const
//{
//    return paintPath;
//}

void ModuleReadImd::setReadImdIndex(int index)
{
    if(index >= READ_IMD_MAX_COUNT)  return;

    if(parentItem())
    {
        ElementPort* parentPort = dynamic_cast<ElementPort*>(parentItem());
        if(parentPort)
        {
            parentPort->setInputIndex(index);
            readImdIndex = index;
        }
    }
}

bool ModuleReadImd::canSetReadImdIndex(int index)
{
    if(index >= READ_IMD_MAX_COUNT)  return false;

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

void ModuleReadImd::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setRenderHints(QPainter::Antialiasing|
                           QPainter::TextAntialiasing);
    if(isSelected())
    {
        painter->setBrush(MODULE_IMD_FOCUSED_BRUSH_FILLIN);
        painter->setPen(MODULE_IMD_BOUNDING_PEN);
        painter->fillRect(realBoundingRect,BCU_COLLIDED_BRUSH_FILLIN);
        painter->fillRect(realBoundingRect,MODULE_IMD_FOCUSED_BRUSH_FILLIN);
        painter->drawRect(realBoundingRect);
    }
    painter->setBrush(MODULE_READ_IMD_BRUSH);
    painter->setPen(MODULE_READ_IMD_PEN);
    painter->drawRect(mapImdRect);

//    painter->setPen(MODULE_READ_IMD_LINE_PEN);
    painter->setPen(MODULE_READ_IMD_PEN);
    painter->drawLine(mapImdLine);

    QFont font;
    font.setPixelSize(MODULE_IMD_TEXT_PIXELSIZE);
    painter->setFont(font);
    painter->setPen(MODULE_IMD_TEXT_PEN);
    painter->drawText(realBoundingRect, Qt::AlignTop | Qt::AlignHCenter, QString("I:%1").arg(readImdIndex));
}

QVariant ModuleReadImd::itemChange(GraphicsItemChange change, const QVariant &value)
{
    return QGraphicsItem::itemChange(change, value);
}
