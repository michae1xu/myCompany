#ifndef ATRRIBUTETREEWIDGET_H
#define ATRRIBUTETREEWIDGET_H

#include <QTreeWidget>
#include "typedef.h"
#include <QListWidgetItem>
#include <QTextEdit>
#include <QLineEdit>
#include <QMap>
#include <QToolButton>
#include <QFileDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QMouseEvent>
#include <QKeyEvent>
#include "fileresolver\xmlresolver.h"
#include "modulebody.h"
#include "modulewire.h"

class HexSpinBox : public QSpinBox
{
//    Q_OBJECT
public:
    QValidator *validator;
    HexSpinBox(int max, QWidget *parent = 0) :
        QSpinBox(parent)
    {
        setRange(0,max);
        validator = new QRegExpValidator(QRegExp("[0-9A-Fa-f]{1,8}"),this);
    }
    ~HexSpinBox(){delete validator;}
    QValidator::State validate(QString &text, int &pos) const
    {
        return validator->validate(text, pos);
    }

    QString textFromValue(int value) const
    {
        return QString::number(value, 16).toUpper();
    }

    int valueFromText(const QString &text) const
    {
        bool ok;
        return text.toInt(&ok, 16);
    }
//    void keyPressEvent(QKeyEvent *event)
//    {
//        if(event->key() == Qt::Key_Enter)
//        {
//            emit saveValue(value());
//        }
//    }
//signals:
//    void saveValue(int);
};

class AttrTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    AttrTreeWidget(QString filename, XmlResolver *xml);
    ~AttrTreeWidget();

    QString filename;
    XmlResolver *xml;

    ElementType currentElement;
    int currentBCU;
    int currentRCU;
    int currentID;

    QComboBox *currentComBox;
    QSpinBox *currentSpinBox;
    AttributeID currentAttributeID;



private:
    void updateMemRDTh(int index);
    void updateMemWTTh(int index);
    void getFuncAndUpdate(QString s, QString b);

    QMap<AttributeID, QTreeWidgetItem*> attributeItemMap;

    void initAttributeItemMap();//将每个属性对应的item存放在attributeNameMap
    void initAttributeValue(ElementType element, int bcu, int rcu, int id);
signals:
    void updateUI();

public slots:
    void setInputAttribute(ModuleBody *firstBody, int firstWireDot, ModuleBody *secondBody, int secondWireDot);
    void showAttribue(ElementType element, int bcu, int rcu, int id);
    void setPosAttribute(QPointF pos);


private slots:
    void itemClickedSlot(QTreeWidgetItem *item, int column);
    void comboxSelected(QString s);
    void comboxSelected(int index);
    void textEdit();
    void onHexSpinboxValueChanged(int v);
    void onSpinboxValueChanged(int v);
    void onToolButtonClick();
};

#endif // ATRRIBUTETREEWIDGET_H
