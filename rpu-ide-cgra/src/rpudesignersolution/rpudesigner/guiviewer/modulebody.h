#ifndef MODULEBODY_H
#define MODULEBODY_H
#include <QRectF>
#include <QPointF>
#include <QMap>
#include <QBrush>
#include "modulewire.h"
#include "typedef.h"
#define MODULEHEIGHT 30
#define GAPEACHIN 30
#define GAPEACHOUT 30
#define GAP 4
class ModuleBody
{
public:
    ModuleBody(QPointF pos, ElementType element, int bcuid, int rcuid, int id);
    ~ModuleBody();
    QString name;
    QString elementName;
    int inputCount;
    int outputCount;
    int bcuID;
    int rcuID;
    int ID;
    ElementType element;
    QRectF rect;
//    float xrate,yrate,wrate,hrate;
//    double rate;

    QRectF touchPoses[8];
    QMap<QRectF*,ModuleWire*> inputPosMap;
    QMap<QRectF*,QList<ModuleWire*>* > outputPosMap;
    QMap<QString,QPointF> labels;
    QBrush getBrush();
    QBrush getTouchBrush();
    void movePos(QPointF relatePos);
    void setPos(QPointF pos);
    void setRect(double x, double y, double w, double h);
    void changeSizeByPos(int index, QPointF relatePos);  //index是指改变大小使用的touchedpos的序号
    void update();
    bool isEnter(QPointF pos);
    int isEnterVertex(QPointF pos);
    int isEnterInput(QPointF pos);
    int isEnterOutput(QPointF pos);
    void setInput(int index, ModuleWire* wire);
    void addOutput(int index, ModuleWire* wire);

    void removeInputWire(ModuleWire* wire);
    void removeInputWire(ModuleWire* wire, int index);
    void removeOutputWire(ModuleWire* wire);
    void removeOutputWire(ModuleWire* wire, int index);
};

#endif // MODULEBODY_H
