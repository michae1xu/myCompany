#ifndef GUIVIEWER_H
#define GUIVIEWER_H

#include <QWidget>
#include <QVariant>
#include <QMouseEvent>
#include <QTreeWidgetItem>
#include <QScrollBar>
#include <QScrollArea>
#include <QMessageBox>
#include <QApplication>
#include <QPainter>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QFrame>
#include <QResizeEvent>
#include <QScrollBar>
#include <QPen>
#include "fileresolver\xmlresolver.h"
#include "modulebody.h"
#include "modulewire.h"
#include "typedef.h"
#include "form\bodyiddialog.h"

class AttrTreeWidget;

enum Status //状态机
{
    NORMAL,
    ADDWIRE1,
    ADDWIRE2,
    ADDWIREFINISH,
    ADDBODY,
    ADDBODYFINISH,
    DRAGBODY,

    CHOOSEWIRE,
    PRECHANGEWIREHEIGHT,
    CHANGEWIREHEIGHT,
    PRECHANGEWIRETOP,
    CHOOSEBODY,
    CHANGEWIRE,
    CHANGEWIREFAIL,
    CHANGEWIREFINISH,

    BODYMENU,
    WIREMENU

};

typedef struct ExistElement
{
    int bcu;
    int rcu;
    ElementType element;
    int id;
    bool en;
    ModuleBody *body;
}EElement;

class GuiViewer;


class GuiViewer : public /*QFrame*/QWidget
{
    Q_OBJECT
public:
    GuiViewer(/*QString filename, XmlResolver* xml, */QScrollArea *scroll, QWidget *parent = 0);
    ~GuiViewer();


    EElement* EElements[BCUMAX][RCUMAX][BFUMAX+SBOXMAX+BENESMAX];
    //
    QMap<ElementType,ModuleBody *>lastBodyMap;

    QScrollArea *m_scrollArea;
    double rate;   //缩放比例
    bool m_isModified;

    QList<ModuleBody*> moduleBodyList;
    QList<ModuleWire*> moduleWireList;
    ModuleBody *moduleInMemery;
    ModuleBody *moduleInfifo;
    ModuleBody *moduleOutMemery;
    ModuleBody *moduleOutfifo;
    ModuleWire *tempWire;   //临时动态线

    QPointF spos;
    QPointF currentPos; //当前鼠标游走的位置
    int touchedBodyIndex;
    int touchedWireIndex;
    int touchPointIndex;
    int currentBcuId;
    int currentRcuId;
    ElementType currentElementType;
    int currentId;
    Status status;  //状态
    bool press;



    void changeStatus(Status s);

    QAction *getAction();
    QString getCurFile();
    XmlResolver *getXml();
    AttrTreeWidget *getAttrTreeWidget();
    bool getShowAllWire();
//    bool getIsProject();
//    void setIsProject(bool isProject);
    bool getIsModified();
    bool getIsUntitled();

    //[start] modified by zhangjun at 2016/8/27
    void newFile();
    bool open(QString fileName = 0);
    //    static GuiViewer *openFile(const QString &fileName, QScrollArea *scroll = 0);
//    QString stpToNm(const QString &fullFileName);
    bool save();
    bool saveAs();
    bool saveFile();
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);

    //[end] modified by zhangjun at 2016/8/27

    void mousePressEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
    void wheelEvent(QWheelEvent *e);
    void keyPressEvent(QKeyEvent *e);

    //状态机需要的变量
    int firstWireDot;
    ModuleBody *firstBody;
    QPoint firstPos;
    int secondWireDot;
    ModuleBody *secondBody;
    QPoint secondPos;


    ElementType preAddElement;
    int preAddBcu;
    int preAddRcu;
    int preAddId;


    QList<int> touchedBodyIndexList;

    void releseAll();
    bool okToContinue();  //判断是否已保存

public slots:
    void resizeSlot();
    void addElement(ElementType element);
    void changeID(ModuleBody *body, int bcu, int rcu, int id);
    void updateUI();
    void setShowAllWire(bool en);

signals:
    void setInputAttribute(ModuleBody *firstBody,int firstWireDot,ModuleBody* secondBody,int secondWireDot);
    void showAttribue(ElementType element, int bcu, int rcu, int id);
    void closeAttribute();
    void setPosAttribute(QPointF pos);
    void setStatus(QString);
    void addElementFinished();
    void copyAvailable(bool); //是否可以复制
    void activeWindowChanged(); //激活的subwindow改变
    void guiViewerClose(QString);

private:

    QString m_curFile;
    XmlResolver *m_xml;
    AttrTreeWidget *m_attrTreeWidget;
    bool m_isUntitled;
//    bool m_isProject;

    QAction *m_action;
    bool showAllWire;
    QList<int> showWireIndexList;


    void recordCurrentTouchedBody(int temp_body);
    void recordCurrentTouchedWire(int temp_wire);   //记录当前选中的实体并显示右边的属性

    void initUI();
//    void adjustUI();    //智能调整UI
    bool getFitID(ElementType element, int &bcu, int &rcu, int &id, bool loop = true);

    void getInputSource(int type, int index, ModuleBody *dest, int sourceBcu, ModuleBody *&source, int &posIndex);
    void addBody(int bcu, int rcu, int id, ModuleBody *body);
    void removeBody(ModuleBody *body);
    void addWire(ModuleBody *source, int sourcePosIndex, ModuleBody *dest, int destPosIndex, ModuleWire *wire);
    void removeWire(ModuleWire *wire);

    bool pressFirstWireDot(QPoint pos, bool start = true);
    bool pressSecondWireDot(QPoint pos);
    bool isEnableToConnect(int sbcu, int srcu, int dbcu, int drcu);

    void setElementExsit(int bcu, int rcu, ElementType ele, int id, bool en);
    bool getElementExsit(int bcu, int rcu, ElementType ele, int id);

    void updateShowWireIndexList();
    int findBodyIndex(int bcu, int rcu, ElementType ele, int id);

    void closeEvent(QCloseEvent *event);



private slots:
    void onScrolBarValueChanged(int);
};

class ScrollArea : public QScrollArea
{
public:
    ScrollArea(){
        this->setWindowState(Qt::WindowMaximized);
        setAttribute(Qt::WA_DeleteOnClose);
    }
    ~ScrollArea(){qDebug() << "~ScrollArea()";}
    void setGUI(GuiViewer *gui){this->gui = gui;setWidget(gui);}
    void resizeEvent(QResizeEvent *){gui->resizeSlot();}
    GuiViewer *gui;

private:
    void closeEvent(QCloseEvent *event)
    {
        if(gui->close())
            event->accept();
        else
            event->ignore();
    }
};
#endif // GUIVIEWER_H
