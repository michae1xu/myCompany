#ifndef ELEMENTSBOX_H
#define ELEMENTSBOX_H

#include "BaseArithUnit.h"
#include "ElementPort.h"
#include "ModuleWriteMem.h"

#include <QGraphicsObject>
#include <QGraphicsItem>
#include <QPainterPath>
#include <QRectF>
#include <QPainter>
#include <QVariant>
#include <QGraphicsSceneMouseEvent>
#include <QList>


class ElementSbox : public BaseArithUnit
{
    Q_OBJECT
public:
    explicit ElementSbox(int bcu,int rcu,int index,QGraphicsItem* parent=0);
    explicit ElementSbox(const ElementSbox& copySbox,QGraphicsItem* parent=0);
    virtual ~ElementSbox();
    QPainterPath shape() const Q_DECL_OVERRIDE;

protected slots:
    void handlePortDeleted(QObject* delPort);

public:
    QString srcConfig;
    int sboxMode;
    int sboxIncreaseMode = 0;
    int sboxGroup;
    int sboxByteSel;

    SboxGroupValue groupCnt0;
    SboxGroupValue groupCnt1;
    SboxGroupValue groupCnt2;
    SboxGroupValue groupCnt3;
    ElementPort * inPort0;

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

#endif // ELEMENTSBOX_H
