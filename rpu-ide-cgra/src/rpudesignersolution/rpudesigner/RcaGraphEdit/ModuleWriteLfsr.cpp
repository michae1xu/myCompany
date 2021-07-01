#include "ModuleWriteLfsr.h"

#include "RcaGraphScene.h"
#include <QDebug>


ModuleWriteLfsr::ModuleWriteLfsr(int index, QGraphicsItem *parent) : BasePort (parent,PORT_FORM_WLFSR)
    , paintPath(QPainterPath())
    , mapRchRect(QRectF())
    , mapRchLine(QLineF())
    , writeLfsrIndex(index)
    , inputType(InputPort_NULL)
    , inputIndex(0)
{
    unitId = WLFSR_ID;
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
}

ModuleWriteLfsr::~ModuleWriteLfsr()
{
    QL_DEBUG<<"~ModuleWriteLfsr()";
}

void ModuleWriteLfsr::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
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
    painter->setBrush(MODULE_WRITE_LFSR_BRUSH);
    painter->setPen(MODULE_WRITE_RCH_PEN);
    painter->drawLine(mapRchLine);

    painter->setPen(MODULE_WRITE_RCH_PEN);
    painter->drawRect(mapRchRect);

    QFont font;
    font.setPixelSize(MODULE_RCH_TEXT_PIXELSIZE);
    painter->setFont(font);
    painter->setPen(MODULE_RCH_TEXT_PEN);
    painter->drawText(realBoundingRect, Qt::AlignBottom | Qt::AlignHCenter, QString("R:%1").arg(writeLfsrIndex));

}

bool ModuleWriteLfsr::setWriteLfsrIndex(int index)
{
    if(index == this->writeLfsrIndex) return true;
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
                    if(findedRcu->isWLfsrFulled) return false;   //add write rch failed when write rch fulled.
//                    int rcuIndex = findedRcu->getIndexRcu();
//                    if(index < rcuIndex*RCU_WRITE_RCH_MAX_COUNT ||
//                            index >= (rcuIndex+1)*RCU_WRITE_RCH_MAX_COUNT)
//                    {
//                        return false;
//                    }

                    for(int i=0;i<findedRcu->wLfsrExistList.size();i++)
                    {
                        if(index == findedRcu->wLfsrExistList.at(i))
                        {
                            return false; //已经存在此rch
                        }
                    }

                    QL_DEBUG << findedRcu->wLfsrExistList;
                    findedRcu->wLfsrExistList.removeOne(writeLfsrIndex);

                    writeLfsrIndex = index;

                    int insertIndex = findedRcu->wLfsrExistList.size();
                    for(int i=0;i<findedRcu->wLfsrExistList.size();i++)
                    {
                        if(writeLfsrIndex < findedRcu->wLfsrExistList.at(i))
                        {
                            insertIndex = i;
                            break;
                        }
                    }

                    findedRcu->wLfsrExistList.insert(insertIndex,writeLfsrIndex);
                    QL_DEBUG << findedRcu->wLfsrExistList;
                    return true;
                }
            }
        }
    }
    return false;
}

QString ModuleWriteLfsr::canSetWriteLfsrIndex(int index)
{
    if(index == this->writeLfsrIndex) return QString(tr(u8""));

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
                    if(findedRcu->isWLfsrFulled) return QString(tr(u8"rch已存满"));   //add write rch failed when write rch fulled.
//                    int bcuIndex = findedRcu->getIndexBcu();
//                    if(index < bcuIndex*RCU_WRITE_RCH_MAX_COUNT ||
//                            index >= (bcuIndex+1)*RCU_WRITE_RCH_MAX_COUNT)
//                    {
//                        return QString(tr(u8"rch序号不符合规定"));
//                    }

                    for(int i=0;i<findedRcu->wLfsrExistList.size();i++)
                    {
                        if(index == findedRcu->wLfsrExistList.at(i))
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

QVariant ModuleWriteLfsr::itemChange(GraphicsItemChange change, const QVariant &value)
{
    return QGraphicsItem::itemChange(change, value);
}

InputPortType ModuleWriteLfsr::getInputType()
{
    return inputType;
}

void ModuleWriteLfsr::setInputType(InputPortType type)
{
    inputType = type;
}

int ModuleWriteLfsr::getInputIndex()
{
     return inputIndex;
}

void ModuleWriteLfsr::setInputIndex(int index)
{
    inputIndex = index;
}


