#ifndef GDWIDGET_H
#define GDWIDGET_H
#include <QMainWindow>
#include <QStatusBar>
#include <QMenu>
#include <QList>

//flags



class GDWidget : public QMainWindow
{
    Q_OBJECT
public:
    GDWidget(int id){this->id = id;}
    virtual void undo(){}
    virtual void redo(){}
    virtual void save(){}
    virtual void doCut(){}
    virtual void doCopy(){}
    virtual void doPaste(){}
    virtual void doDelete(){}
    virtual int getID(){return id;}
    virtual QList<QMenu*> getMenus(){QList<QMenu*> ret;return ret;}
    virtual QList<QToolBar*> getToolBars(){QList<QToolBar*> ret;return ret;}
    int id;
signals:
    void setUndoAvailable(int id, bool);
    void setStatusLabel(QString);

};


#endif // GDWIDGET_H
