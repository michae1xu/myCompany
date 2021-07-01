#ifndef MODULEWIRE_H
#define MODULEWIRE_H
#include <QMap>
#include <QRectF>
#include <QBrush>

class ModuleBody;
class ModuleWire
{
public:
    ModuleWire(QPointF spos, QPointF epos);
    ~ModuleWire();
    int height;

    ModuleBody *input;
    int inputPosIndex;
    ModuleBody *output;
    int outputPosIndex;

    QRectF touchPoses[4];

    bool isTouched;


    QPointF point[4];

    QBrush getBrush();
    QBrush getTouchBrush();

    void updateHeight();
    void setInput(ModuleBody *body, int posindex);
    void setOutput(ModuleBody *body, int posindex);
    void updateTempPos();
    void update();
    void movePoint(int index,QPointF pos);
    int changeHeight(QPointF pos);
    void setHeight(double height);
    void setPoint(int index,QPointF pos);
    void movePos(QPointF pos);
    bool isEnter(QPointF pos);
    int isEnterVertex(QPointF pos);
    int isEnterInput(QPointF pos);
    int isEnterOutput(QPointF pos);
};

#endif // MODULEWIRE_H
