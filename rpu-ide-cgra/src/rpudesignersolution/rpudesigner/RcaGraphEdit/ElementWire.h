#ifndef ELEMENTWIRE_H
#define ELEMENTWIRE_H

#include <QGraphicsObject>
#include <QGraphicsItem>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QLineF>
#include <QPointF>
#include <QPainter>
#include <QList>
#include <QVariant>

#include "BaseItem.h"
#include "ElementPort.h"
#include "RcaGraphDef.h"

using namespace rca_space;

class QUndoCommand;

class ElementWire : public QObject, public QGraphicsPathItem
{
    Q_OBJECT
public:
    explicit ElementWire(QPointF posAtScene,ElementPort* inSignal,QBrush penBrush,qreal penWidth,QGraphicsItem *parent = 0);
    virtual ~ElementWire();
//    void addSubMovePointF(QPointF posAtScene);
    void addPointF(QPointF posAtScene);
    void movedPointF(QPointF posAtScene);
    QPainterPath shape() const Q_DECL_OVERRIDE;
//    void commitPath();
    bool commitPath(ElementPort *endPort,QPointF posAtScene);
    void addEndPortFromXml(ElementPort *endPort,QPointF endPosAtScene);
    void addEndPortBySet(ElementPort *endPort, QPointF endPosAtScene);
    void paintWireByUndoCmd(QList<QPointF> pressedList,QList<QPointF> infelctList);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget = 0) Q_DECL_OVERRIDE;
    QPointF translateToHVLine(QPointF posTrs,QPointF posSrc); //改变两点的线为横线或者竖线。
    QPainterPath autoPath(QPointF newPos,QPainterPath srcPath); //添加某个点，改为自动布线
    QPainterPath getIntersect(const ElementWire* eleWire, const QPainterPath &line);
    bool isCoincide(const ElementWire* eleWire, const QPainterPath &line);  //判断有没有重合
    ElementPort* getInputSignal() {return inputSignal;}
    ElementPort* getOutPort() { return outPort;}
    void setWirePenWidth(qreal width);
    void setWirePenBrush(QBrush brush);
    void setWireVisibleForUndo(bool visable);
    void setWindowToModified();
    void appendPortToConnectList(ElementPort* portItem);
    void removePortFromConnectedList(ElementPort* portItem);
protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;

    QList<ElementPort*> getInPortCollidedWithPath(QPainterPath checkPath); //获取path碰撞到的inport的list。
    QList<ElementPort*> getOutPortCollidedWithPath(QPainterPath checkPath); //获取path碰撞到的outport的list。
    bool judgePathIsValid(QPainterPath checkPath);//判断path是否是有效的。如果path中含有两个inport就是不合法。如果合法，设定collidedInPort。
    void resetConnectCollidedPorts();
    void setMovetoCollidedPorts();
    bool isCollidedWithPorts();
    void deleteCollidedPortsConnect();
    bool setCollidedOtherWire();
    QList<QPainterPath> translateToLineList(const QPainterPath &originPath);
    void setHoverCursor(QPointF pos);

private slots:
    void disconnectWithPort(ElementPort* disPort);
    void connectWithPort(ElementPort* conPort);
    void handlePortBeChanged(ElementPort* port);

signals:
    void clearConnectPortInSignal();
    void setConnectPortInSignal(BasePort *inPort);

private:
    WireDirection judgePathIsLine(const QPainterPath &originPath);
    bool isContainsPoint(const QPainterPath &originPath,QPointF point);
    void adjustedPath(QPointF pos);
    bool resetInSignal(ElementPort* inPort);//inSignal为空则返回false。
    bool clearInSignal();
    bool isHadOtherInSignals(QPainterPath checkPath); //在path连接的port中是否还有其他的insignal信号。
    void resetInfletionPosByPath(QPainterPath posPath);
    QList<QPointF> removeBeforeNeedlessPoint(QList<QPointF> listSrc,QList<QPointF> listReference);
    QList<QPointF> removeAfterNeedlessPoint(QList<QPointF> listSrc,QList<QPointF> listReference);
    QList<QPointF> removeBothNeedlessPoint(QList<QPointF> listSrc,QList<QPointF> listReference);
    void setPosIn2Out();//设置posIn2Out的关系。
    QList<QPointF> mapPosListFromPort(ElementPort* port,QList<QPointF> posList);
private:
    QList<QPainterPath> shapePathList;
    QPainterPath wirePath;
    QPainterPath wirePathPainting; //正在描画的曲线
    QPointF pressedPonitF;

    bool isCollidedPorts;
//    bool isChangedColliedPorts;
    QList<ElementPort*> hadConnectPorts;
    HoverPosAtItem posAtItem;
    int indexHoverAtPathLine;     //hover状态在path转为line之后的那个位置。
    ElementPort *inputSignal;   //每一个wire只能有一个inport。此时的inport中的方向属性(PortType)为out。
    ElementPort *outPort;   //每一个wire只能有一个outPort.
    QList<ElementPort*> collidedOutPortList;    //每一个wire可以有多个outport。此时outport中的方向属性(PortType)为in。
    bool isWireValid;
    QBrush wireUnselBrush;
    QPen wirePen;
    bool isWirePathChanged;
    QPointF posIn2Out;    //input 与output 的 pos关系。
public:
    bool isHadToShow;   //不管什么原因，必须show。//尤其是刚画的线，必须show，只有当全部隐藏线段后，才不必须show。
    QUndoCommand* lastCmd;
    QList<QPointF> pressedPosList;  //相对scene的点的坐标信息。
    QList<QPointF> infletionPosList;

private:

};

#endif // ELEMENTWIRE_H
