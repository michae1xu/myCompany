#ifndef ELEMENTBFU_H
#define ELEMENTBFU_H

#include "BaseArithUnit.h"
#include "ElementPort.h"

#include <QGraphicsObject>
#include <QGraphicsItem>
#include <QPainterPath>
#include <QRectF>
#include <QPainter>
#include <QVariant>
#include <QGraphicsSceneMouseEvent>
#include <QList>

class ElementBfu : public BaseArithUnit
{
    Q_OBJECT
public:
    explicit ElementBfu(int bcu,int rcu,int index,QGraphicsItem* parent=0);
    explicit ElementBfu(const ElementBfu& copyBfu,QGraphicsItem* parent=0);
    virtual ~ElementBfu();
    QPainterPath shape() const Q_DECL_OVERRIDE;

protected slots:
    void handlePortDeleted(QObject* delPort);
public:
    ElementPort * inPortA;
    ElementPort * inPortB;
    ElementPort * inPortT;

    ElementPort * outPortX;
    ElementPort * outPortY;

private:
    QPainterPath paintPath;

public:
    BfuFuncIndex funcIndex;
    QString funcExp;
    int funcAuModIndex;
    bool funcAuCarry = false;
    int funcMuModIndex = 0;
    int bypassIndex;
    QString bypassExp;

    void setElementInfo(ElementInfo_del *value);

signals:
    void sendChangedElementInfo(QString str,QStringList outputList);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // ELEMENTRFU_H
