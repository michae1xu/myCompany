#ifndef ELEMENTCUSTOMPE_H
#define ELEMENTCUSTOMPE_H

#include "BaseArithUnit.h"
#include "ElementPort.h"
#include "datafile.h"
#include <QGraphicsObject>
#include <QGraphicsItem>
#include <QPainterPath>
#include <QRectF>
#include <QPainter>
#include <QVariant>
#include <QGraphicsSceneMouseEvent>
#include <QList>

class ElementCustomPE : public BaseArithUnit
{
    Q_OBJECT
public:
    ElementCustomPE(int bcu, int rcu, int index, QGraphicsItem* parent=0);
    ElementCustomPE(const ElementCustomPE& copyCustomPe, QGraphicsItem* parent=0);
    virtual ~ElementCustomPE();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget = 0) Q_DECL_OVERRIDE;
    QPainterPath shape() const Q_DECL_OVERRIDE;

    void changeInputPort(int portNum);
    void changeOutputPort(int portNum);
protected slots:
    void handlePortDeleted(QObject* delPort);
public:
    std::vector<ElementPort*> m_inputPortVector;
    std::vector<ElementPort*> m_outputPortVector;
private:
    QPainterPath paintPath;

    int m_inputNum = 3;
    int m_outputNum = 2;

public:
    BfuFuncIndex funcIndex;//need to changed, but not now
    QString funcExp;
    int funcAuModIndex;
    int funcMuModIndex;
    int bypassIndex;
    QString bypassExp;


    int inputNum() const;
    void setInputNum(int inputNum);

    int outputNum() const;
    void setOutputNum(int outputNum);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

#endif // ELEMENTCUSTOMPE_H
