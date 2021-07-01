#include "ModuleReadLfsr.h"

#include "ElementPort.h"
#include <QDebug>
#include "ElementBfu.h"
#include "ModuleRcu.h"

ModuleReadLfsr::ModuleReadLfsr(int index, QGraphicsItem *parent) : BasePort (parent,PORT_FROM_RRCH)
    , paintPath(QPainterPath())
    , mapRchRect(QRectF())
    , mapRchLine(QLineF())
    , readLfsrIndex(index)
{
    unitId = RLFSR_ID;
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
//    setFlag(QGraphicsItem::ItemIsMovable);
    isPartOfChildRect = false;
    isEnabledAdjustRect = false;
    realBoundingRect = MODULE_READ_RCH_DEFAULT_RECT;
    paintRect = MODULE_READ_RCH_DEFAULT_PAINT_RECT;

    mapRchRect = QRectF(paintRect.left(),paintRect.top(),
                         MAP_RCH_DEFAULT_WIDTH,MAP_RCH_DEFAULT_HEIGHT);
    mapRchLine = QLineF(QPointF(paintRect.left()+paintRect.width()/2,paintRect.top()+MAP_RCH_DEFAULT_HEIGHT),
                       QPointF(paintRect.left()+paintRect.width()/2,paintRect.bottom()));
}

ModuleReadLfsr::~ModuleReadLfsr()
{
//    QL_DEBUG<<"~ModuleReadRch()";
}

void ModuleReadLfsr::setReadLfsrIndex(int index)
{
    if(index >= READ_LFSR_MAX_COUNT)  return;

    if(parentItem())
    {
        ElementPort* parentPort = dynamic_cast<ElementPort*>(parentItem());
        if(parentPort)
        {
            parentPort->setInputIndex(index);
            readLfsrIndex = index;
            ElementBfu* parentBfu = dynamic_cast<ElementBfu*>(parentPort->parentItem());
            if(parentBfu)
            {
                ModuleRcu* parentRcu = dynamic_cast<ModuleRcu*>(parentBfu->parentItem());
                if(parentRcu)
                {
                    parentRcu->reaetRchChannelId(readLfsrIndex/READ_RCH_MAX_COUNT_ONE_GROUP);
                }
            }
        }
    }
}

bool ModuleReadLfsr::canSetReadLfsrIndex(int index)
{
    if(index >= READ_LFSR_MAX_COUNT)  return false;

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

void ModuleReadLfsr::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setRenderHints(QPainter::Antialiasing|
                           QPainter::TextAntialiasing);
    if(isSelected())
    {
        painter->setBrush(MODULE_RCH_FOCUSED_BRUSH_FILLIN);
        painter->setPen(MODULE_RCH_BOUNDING_PEN);
        painter->fillRect(realBoundingRect,BCU_COLLIDED_BRUSH_FILLIN);
        painter->fillRect(realBoundingRect,MODULE_RCH_FOCUSED_BRUSH_FILLIN);
        painter->drawRect(realBoundingRect);
    }
    painter->setBrush(MODULE_READ_LFSR_BRUSH);
    painter->setPen(MODULE_READ_RCH_PEN);
    painter->drawRect(mapRchRect);

    painter->setPen(MODULE_READ_RCH_PEN);
    painter->drawLine(mapRchLine);

    QFont font;
    font.setPixelSize(MODULE_RCH_TEXT_PIXELSIZE);
    painter->setFont(font);
    painter->setPen(MODULE_RCH_TEXT_PEN);
    painter->drawText(realBoundingRect, Qt::AlignTop | Qt::AlignHCenter, QString("R:%1").arg(readLfsrIndex));
}

QVariant ModuleReadLfsr::itemChange(GraphicsItemChange change, const QVariant &value)
{
    return QGraphicsItem::itemChange(change, value);
}

