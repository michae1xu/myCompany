#ifndef DEBUGLOGITEM_H
#define DEBUGLOGITEM_H

#include <QGraphicsItem>
#include <QString>
#include <QStringList>
#include <QPainter>
#include <QRect>
#include <QColor>
#include <QList>
#include <QMap>
#include <QObject>
#include <QGraphicsObject>
#include <QGraphicsSceneMouseEvent>

#define DEBUG_BFU_INPUTCOUNT      3
#define DEBUG_SBOX_INPUTCOUNT     1
#define DEBUG_BENES_INPUTCOUNT    4

#define DEBUG_BFU_OUTPUTCOUNT     2
#define DEBUG_SBOX_OUTPUTCOUNT    4
#define DEBUG_BENES_OUTPUTCOUNT   4

enum ElementTypeId{
    DEBUG_BFU = 0,
    DEBUG_SBOX,
    DEBUG_BENES
};

const QMap<int,QString> mapElementName ={
    {DEBUG_BFU,"BFU"},
    {DEBUG_SBOX,"SBOX"},
    {DEBUG_BENES,"BENES"},
};

const QMap<int,int> mapElementInput ={
    {DEBUG_BFU,DEBUG_BFU_INPUTCOUNT},
    {DEBUG_SBOX,DEBUG_SBOX_INPUTCOUNT},
    {DEBUG_BENES,DEBUG_BENES_INPUTCOUNT}
};

const QMap<int,int> mapElementOutput ={
    {DEBUG_BFU,DEBUG_BFU_OUTPUTCOUNT},
    {DEBUG_SBOX,DEBUG_SBOX_OUTPUTCOUNT},
    {DEBUG_BENES,DEBUG_BENES_OUTPUTCOUNT}
};

class ElementInfo{
public:
    QString elementId;
    int typeId;
    int bcu;
    int rcu;
    int id;
    int isEnabled;
    QStringList inputList;
    QStringList outputList;
    QList<bool> isInputTextChaged;  //前后周期数据更改标志位。
    QList<bool> isOutputTextChaged; //前后周期数据更改标志位。
    QList<bool> isInputPointBreak;
    QList<bool> isOutputPointBreak;
    bool isEnableChanged;
    ElementInfo();
    ~ElementInfo();
};

class DebugLogItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit DebugLogItem(ElementInfo *elementInfo , QGraphicsItem *parent = 0);
    ~DebugLogItem();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                  QWidget *widget = 0);


    ElementInfo* elementItemInfo;
    bool setBreakInfo(bool isIn,int portIndex,bool isEn);
public:
    QRect getItemBounding();
signals:
    void sendChangedElementInfo(QString str,QStringList inputList,QStringList outputList);
    void addBreakPoint(QString str,QString portInfo);
    void deleteBreakPoint(QString str,QString portInfo);

protected:
//    void keyPressEvent(QKeyEvent *event);
//    void mousePressEvent(QGraphicsSceneMouseEvent *event);

//    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
//    void hoverLeaveEvent (QGraphicsSceneHoverEvent * event);
//    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
//    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
private:

    QRect itemBoundingRect;
    int inputTextCount;
    int outputTextCount;
    QRectF *inputTextRect;
    QRectF *outputTextRect;

};

#endif // DEBUGLOGITEM_H
