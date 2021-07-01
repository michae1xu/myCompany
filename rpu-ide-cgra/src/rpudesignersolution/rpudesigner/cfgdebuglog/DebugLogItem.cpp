#include "DebugLogItem.h"
#include "Form/ItemDataForm.h"

#include <QFont>
#include <QDebug>

#define ITEM_HORIZONTAL_COUNT       3
#define ITEM_TEXT_SPACE             " "
const QColor g_inputTextColor       = QColor(0,0,255,255);
const QColor g_outputTextColor      = QColor(255,0,0,255);
const QColor g_textNormalBGColor    = QColor(Qt::transparent);
const QColor g_textChangedBGColor   = QColor(Qt::lightGray);
const QColor g_itemBoundingColor    = QColor(Qt::darkGray);
const QColor g_elementColor         = QColor(Qt::black);
const QColor g_itemNormalBGColor    = QColor(Qt::white);
const QColor g_itemFocusedBGColor   = QColor(Qt::yellow);
const QColor g_textBoundingColor    = QColor(Qt::transparent);

const QColor g_unsetBreakPointColor = QColor(Qt::transparent);
const QColor g_setBreakPointColor   = QColor(200,80,80,100);

const int g_breakPointRadiiScale    = 2;    //断点半径比例

ElementInfo::ElementInfo()
{
}

ElementInfo::~ElementInfo()
{
}

DebugLogItem::DebugLogItem(ElementInfo *elementInfo,QGraphicsItem *parent): QGraphicsObject(parent)
    ,inputTextCount(0)
    ,outputTextCount(0)
    ,inputTextRect(NULL)
    ,outputTextRect(NULL)
{
    Q_ASSERT(elementInfo);
    elementItemInfo = elementInfo;
    itemBoundingRect = QRect(0,0,0,0);
    if(elementItemInfo)
    {
        inputTextCount = elementItemInfo->inputList.size();
        outputTextCount = elementItemInfo->outputList.size();
        itemBoundingRect = QRect(0,0,(inputTextCount>outputTextCount?inputTextCount:outputTextCount)*80,60);
        inputTextRect = new QRectF[inputTextCount]();
        outputTextRect = new QRectF[outputTextCount]();
    }

    setFlag(QGraphicsItem::ItemIsFocusable);
//    setFlag(QGraphicsItem::ItemIsMovable);
    setAcceptHoverEvents(true);
}

DebugLogItem::~DebugLogItem()
{
    if(inputTextRect)
    {
        delete[] inputTextRect;
        inputTextRect = NULL;
    }
    if(outputTextRect)
    {
        delete[] outputTextRect;
        outputTextRect = NULL;
    }
}

QRect DebugLogItem::getItemBounding()
{
    return itemBoundingRect;
}

QRectF DebugLogItem::boundingRect() const
{
    qreal adjust = 0.5;
    return QRectF(itemBoundingRect.x() - adjust, itemBoundingRect.y() - adjust,
                  itemBoundingRect.width() + adjust, itemBoundingRect.height() + adjust);
}

void DebugLogItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing,true);
    QFont font;
    font.setPixelSize(14);
    painter->setFont(font);

    if(elementItemInfo)
    {
        if(hasFocus()) {
            painter->setBrush(g_itemFocusedBGColor);
            painter->setPen(g_itemBoundingColor);
        } else {
            painter->setBrush(g_itemNormalBGColor);
            painter->setPen(g_itemBoundingColor);
        }
        painter->drawRect(itemBoundingRect);

        if(!elementItemInfo->elementId.isNull())
        {
            QString nameTemp = QString(elementItemInfo->elementId + ":%1").arg(elementItemInfo->isEnabled);
            QRectF nameTextRect = painter->boundingRect(itemBoundingRect, Qt::AlignCenter, nameTemp+ITEM_TEXT_SPACE);

            //画出数据的背景颜色
            painter->setBrush(elementItemInfo->isEnableChanged  ? g_textChangedBGColor : g_textNormalBGColor);
            painter->setPen(g_textBoundingColor);
            painter->drawRect(nameTextRect);

            //画出id数据
            painter->setPen(g_elementColor);
            painter->drawText(nameTextRect, Qt::AlignCenter, nameTemp);
        }
        if(outputTextCount > 0)
        {
            int outputX = itemBoundingRect.x();
            int outputY = itemBoundingRect.y() + itemBoundingRect.height()/ITEM_HORIZONTAL_COUNT*(ITEM_HORIZONTAL_COUNT-1);
            int outputWidth = itemBoundingRect.width()/outputTextCount;
            int outputHight = itemBoundingRect.height()/ITEM_HORIZONTAL_COUNT;
            for(int i=0; i<outputTextCount; i++)
            {
                QRectF outputRect = QRect(outputX+outputWidth*i,outputY,outputWidth,outputHight);
                outputTextRect[i] = painter->boundingRect(outputRect, Qt::AlignCenter, elementItemInfo->outputList.at(i)+ITEM_TEXT_SPACE);

                //画出断点圆圈符号
                painter->setBrush(elementItemInfo->isOutputPointBreak.at(i) ? g_setBreakPointColor : g_unsetBreakPointColor);
                painter->setPen(elementItemInfo->isOutputPointBreak.at(i) ? g_setBreakPointColor : g_unsetBreakPointColor);
                painter->drawEllipse(outputTextRect[i].center(),outputTextRect[i].height()/g_breakPointRadiiScale,outputTextRect[i].height()/g_breakPointRadiiScale);

                //画出数据的背景颜色
                painter->setBrush(elementItemInfo->isOutputTextChaged.at(i) ? g_textChangedBGColor : g_textNormalBGColor);
                painter->setPen(g_textBoundingColor);
                painter->drawRect(outputTextRect[i]);

                //画出output数据
                painter->setPen(g_outputTextColor);
                painter->drawText(outputTextRect[i], Qt::AlignCenter, elementItemInfo->outputList.at(i));
            }
        }
        if(inputTextCount >  0)
        {
            int inputX = itemBoundingRect.x();
            int inputY = itemBoundingRect.y();
            int inputWidth = itemBoundingRect.width()/inputTextCount;
            int inputHight = itemBoundingRect.height()/ITEM_HORIZONTAL_COUNT;
            for(int i=0; i<inputTextCount; i++)
            {
                QRectF inputRect = QRect(inputX+inputWidth*i,inputY,inputWidth,inputHight);
                inputTextRect[i] = painter->boundingRect(inputRect, Qt::AlignCenter, elementItemInfo->inputList.at(i)+ITEM_TEXT_SPACE);

                //画出断点圆圈符号
                painter->setBrush(elementItemInfo->isInputPointBreak.at(i) ? g_setBreakPointColor : g_unsetBreakPointColor);
                painter->setPen(elementItemInfo->isInputPointBreak.at(i) ? g_setBreakPointColor : g_unsetBreakPointColor);
                painter->drawEllipse(inputTextRect[i].center(),inputTextRect[i].height()/g_breakPointRadiiScale,inputTextRect[i].height()/g_breakPointRadiiScale);

                //画出数据的背景颜色
                painter->setBrush(elementItemInfo->isInputTextChaged.at(i) ? g_textChangedBGColor : g_textNormalBGColor);
                painter->setPen(g_textBoundingColor);
                painter->drawRect(inputTextRect[i]);

                //画出input数据
                painter->setPen(g_inputTextColor);
                painter->drawText(inputTextRect[i], Qt::AlignCenter, elementItemInfo->inputList.at(i));
            }
        }
    }
}

bool DebugLogItem::setBreakInfo(bool isIn, int portIndex, bool isEn)
{
    if(isIn)
    {
        if(portIndex < elementItemInfo->isInputPointBreak.count())
        {
            elementItemInfo->isInputPointBreak.replace(portIndex,isEn);
            update();
            return true;
        }
        return false;
    }
    else
    {
        if(portIndex < elementItemInfo->isOutputPointBreak.count())
        {
            elementItemInfo->isOutputPointBreak.replace(portIndex,isEn);
            update();
            return true;
        }
        return false;
    }
    return false;
}

void DebugLogItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    bool isSetPointBreak = false;
    QString sendStr = QString("%1,%2,%3,%4").arg(elementItemInfo->typeId).arg(elementItemInfo->bcu) \
                                            .arg(elementItemInfo->rcu).arg(elementItemInfo->id);
    QStringList isInputBreakList;
    QStringList isOutputBreakList;
    isInputBreakList.clear();
    isOutputBreakList.clear();
    for(int i=0;i<inputTextCount;i++)
    {
        if(inputTextRect[i].contains(event->pos().toPoint()))
        {
            isSetPointBreak = true;
            elementItemInfo->isInputPointBreak.replace(i,!elementItemInfo->isInputPointBreak.at(i));
            if(elementItemInfo->isInputPointBreak.at(i))
            {
                //add break point
                emit addBreakPoint(sendStr,QString(tr(u8"input=%1")).arg(i));
            }
            else
            {
                //delete break point
                emit deleteBreakPoint(sendStr,QString(tr(u8"input=%1")).arg(i));
            }
            update();
        }
        isInputBreakList.append(QString::number(elementItemInfo->isInputPointBreak.at(i)));
    }

    for(int i=0;i<outputTextCount;i++)
    {
        if(outputTextRect[i].contains(event->pos().toPoint()))
        {
            isSetPointBreak = true;
            elementItemInfo->isOutputPointBreak.replace(i,!elementItemInfo->isOutputPointBreak.at(i));
            if(elementItemInfo->isOutputPointBreak.at(i))
            {
                //add break point
                emit addBreakPoint(sendStr,QString(tr(u8"output=%1")).arg(i));
            }
            else
            {
                //delete break point
                emit deleteBreakPoint(sendStr,QString(tr(u8"output=%1")).arg(i));
            }
            update();
        }
        isOutputBreakList.append(QString::number(elementItemInfo->isOutputPointBreak.at(i)));
    }

    if(!isSetPointBreak)
    {
        ItemDataForm dataForm((ElementTypeId)elementItemInfo->typeId,elementItemInfo->inputList,elementItemInfo->outputList);
        if(dataForm.exec() == QDialog::Accepted)
        {
//            elementItemInfo->inputList = dataForm.getInputStrList();
            elementItemInfo->outputList = dataForm.getOutputStrList();
            update();
            emit sendChangedElementInfo(sendStr,elementItemInfo->inputList,elementItemInfo->outputList);
        }
        else
        {
        }
    }
}
