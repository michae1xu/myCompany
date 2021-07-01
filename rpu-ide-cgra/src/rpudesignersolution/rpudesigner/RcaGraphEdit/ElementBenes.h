#ifndef ELEMENTBENES_H
#define ELEMENTBENES_H

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


class ElementBenes : public BaseArithUnit
{
    Q_OBJECT
public:
    explicit ElementBenes(int bcu,int rcu,int index,QGraphicsItem* parent=0);
    explicit ElementBenes(const ElementBenes& copyBenes,QGraphicsItem* parent=0);
    virtual ~ElementBenes();
    QPainterPath shape() const Q_DECL_OVERRIDE;

protected slots:
    void handlePortDeleted(QObject* delPort);

public:
    QString srcConfig;
    ElementPort * inPort0;
    ElementPort * inPort1;
    ElementPort * inPort2;
    ElementPort * inPort3;

    ElementPort * outPort0;
    ElementPort * outPort1;
    ElementPort * outPort2;
    ElementPort * outPort3;

    void setElementInfo(ElementInfo_del *elementInfo);
private:
    QPainterPath paintPath;
signals:
    void sendChangedElementInfo(QString str,QStringList outputList);

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

};

#endif // ELEMENTBENES_H
