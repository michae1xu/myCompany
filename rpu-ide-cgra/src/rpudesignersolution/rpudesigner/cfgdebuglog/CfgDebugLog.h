#ifndef CFGDEBUGLOG_H
#define CFGDEBUGLOG_H

#include "DebugLogItem.h"
#include "DebugLogScene.h"
#include "DebugLogView.h"
#include <QList>
#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QMap>

typedef struct _tagDebugBreakPoint{
    QString pointStr;
    bool isEnabled;
    QString strName;
    _tagDebugBreakPoint(){
        pointStr.clear();
        isEnabled = true;
    }
}DebugBreakPoint;

class CfgDebugLog : public QObject
{
    Q_OBJECT
public:
    CfgDebugLog();
    ~CfgDebugLog();

//    //test for new function start
//    int getCurrentClk(){ return currentClk; }
//    int getTotalClk() { return 100; }
//    void setLogFileName(QString fileName){ logFileName = fileName; }
//    QString getLofFileName() { return logFileName; }
//    //end test

    DebugLogView* getDebugLogView();
    QList<DebugBreakPoint> *getBreakPointList();
    bool isIntoBreakpoint();
    bool setItemBreakInfo(QString str,bool isEnabled);
private:
    DebugLogScene *logScene;
    DebugLogView *logView;
    QList<DebugLogItem*> itemList;
    QList<ElementInfo*> listElementInfo;// all element info
    bool isElementItemInited;
    QMap<int,QString> mapElementItemList;
    QList<DebugBreakPoint> breakPointList;
    bool intoBreakpoint;

//    //test start
//    QFile *logFile;
//    QString logFileName;
//    QTextStream *logTextIn;
//    int currentClk;
//    void openFile();
//    void closeFile();
//    QStringList readClkFromFile(int clkNum);
//    //test end

    void elementItemInit(const QStringList &infoList);

signals:
    void sendErrorMsg(QString errMsg);
    void sendClickedItem(QString elementId);
    void sendChangedElementInfo(QString str,QStringList inputList,QStringList outputList);
    void addBreakPoint(QString str,QString portInfo);
    void deleteBreakPoint(QString str,QString portInfo);

public slots:

//    //test start
//    void updateToNextClk();
//    void updateClkToPos(int clkNum);
//    //test end

    void setAllElementInfo(const QStringList &infoList);
    void startDebugMode(const QStringList &infoList);
    void endDebugMode();
    void handleClickedItem(QGraphicsItem* clickedItem);
    void handleClickedFromXml(QString elementId);
    void handleChangedElementInfo(QString str,QStringList inputList,QStringList outputList);
    void handleAddBreakPoint(QString str, QString portInfo);
    void handleDeleteBreakPoint(QString str,QString portInfo);

};

#endif // CFGDEBUGLOG_H
