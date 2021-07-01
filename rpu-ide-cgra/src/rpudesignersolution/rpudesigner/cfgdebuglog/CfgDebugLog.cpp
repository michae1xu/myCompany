#include "CfgDebugLog.h"
#include "common/debug.h"
#include <QList>
#include <QTimer>
#include <QDateTime>
#include <QDebug>
#include <QTimer>
#include <QCursor>
#include <QApplication>

#define DEBUG_SCENE_WIDTH       5000
#define DEBUG_SCENE_HEIGHT      3000
#define ITEM_DISTANCE_WIDTH     30
#define ITEM_DISTANCE_HEIGHT    30
#define DEFAULT_KEY_MAP_ELEMENT -1
#define ITEM_TEXT_INIT          "00000000"

#ifndef LK_NO_DEBUG_OUTPUT   //rpudesigner.pro中注释后可开启日志
#define QL_DEBUG qDebug() << QString("%1 > %2 > %3 >").arg(__FILE__).arg(__FUNCTION__).arg(__LINE__).toLocal8Bit().data()
#else
#define QL_DEBUG noDebug()
#endif

CfgDebugLog::CfgDebugLog():
    logScene(NULL)
    ,logView(NULL)
    ,itemList(QList<DebugLogItem*>())
    ,listElementInfo(QList<ElementInfo*>())
    ,isElementItemInited(false)
    ,breakPointList(QList<DebugBreakPoint>())
{
//    logTextIn = NULL;
//    logFile = NULL;

    mapElementItemList.clear();
    logScene = new DebugLogScene();
    logView = new DebugLogView();

    logScene->setSceneRect(0, 0, DEBUG_SCENE_WIDTH, DEBUG_SCENE_HEIGHT);
    logView->resize(400, 300);
    logView->setScene(logScene);
    logView->centerOn(0,0);
    logView->setBackgroundBrush(QBrush(QColor("white")));
    logView->setDragMode(QGraphicsView::ScrollHandDrag);

    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));

    connect(logScene,SIGNAL(mouseClickedItem(QGraphicsItem*)),this,SLOT(handleClickedItem(QGraphicsItem*)));

}

CfgDebugLog::~CfgDebugLog()
{
    endDebugMode();
    if(logScene)
    {
        delete logScene;
        logScene = NULL;
    }
    if(logView)
    {
        delete logView;
        logView = NULL;
    }
}

DebugLogView* CfgDebugLog::getDebugLogView()
{
    return logView ? logView : 0;
}

QList<DebugBreakPoint> *CfgDebugLog::getBreakPointList()
{
    return &breakPointList;
}

bool CfgDebugLog::isIntoBreakpoint()
{
    return intoBreakpoint;
}

bool CfgDebugLog::setItemBreakInfo(QString str,bool isEnabled)
{
    Q_UNUSED(isEnabled);
    QStringList strList = str.split(",");
    if(strList.count() != 5) return false;

    QStringList inputStrList = QString(strList.at(4)).split("=");
    if(inputStrList.count() != 2) return false;

    int typeId = QString(strList.at(0)).toInt();
    int bcuId = QString(strList.at(1)).toInt();
    int rcuId = QString(strList.at(2)).toInt();
    int id = QString(strList.at(3)).toInt();
    int portId = QString(inputStrList.at(1)).toInt();
    bool isIn = QString(inputStrList.at(0)).startsWith("input") ? true : false;
    bool isEnable = true;
    return logScene->setItemBreakInfo(typeId,bcuId,rcuId,id,portId,isIn,isEnable);
}


#define ELE_INFO_SPACE_NUM 3    //数据切割后列表数量
void CfgDebugLog::setAllElementInfo(const QStringList &infoList)
{
    if(!isElementItemInited) return;
    if(infoList.size() <= 0) return;
    intoBreakpoint = false;

    for(int index=0; index<infoList.size();index++)
    {
        QString textLine = infoList.at(index);
        QStringList textLineList = textLine.replace("]","").replace(" ","").split("[");
        ElementInfo *eleTemp;
        // read line info to element;
        if(textLineList.size() >= ELE_INFO_SPACE_NUM)
        {
            QStringList tempId = QString(textLineList.at(0)).split(":");
            if(tempId.size() < 2) continue;

            QString strElementIdTemp = tempId.at(0);
            int elementKey = mapElementItemList.key(strElementIdTemp,DEFAULT_KEY_MAP_ELEMENT);
            if(DEFAULT_KEY_MAP_ELEMENT != elementKey)
            {
                //该算子已经创建，并存在map中。
                if(listElementInfo.size() > elementKey)
                {
                    eleTemp = listElementInfo.at(elementKey);
                    if(eleTemp->elementId != tempId.at(0)) continue;
//                    QStringList inputListTemp = QString(textLineList.at(1)).replace("[","").replace("]","").split(",");
//                    QStringList outputListTemp = QString(textLineList.at(2)).replace("[","").replace("]","").split(",");
                    QStringList inputListTemp = QString(textLineList.at(1)).split(",");
                    QStringList outputListTemp = QString(textLineList.at(2)).split(",");
                    if(eleTemp->inputList.size() > inputListTemp.size()) continue;
                    if(eleTemp->outputList.size() > outputListTemp.size()) continue;

                    eleTemp->isEnableChanged = (eleTemp->isEnabled != QString(tempId.at(1)).toInt());
                    eleTemp->isEnabled = QString(tempId.at(1)).toInt();

                    for(int i=0;i<eleTemp->inputList.size();i++)
                    {
                        eleTemp->isInputTextChaged.replace(i,QString::compare(eleTemp->inputList.at(i),inputListTemp.at(i)));
                        eleTemp->inputList.replace(i,inputListTemp.at(i));
                        if(eleTemp->isInputPointBreak.at(i) && eleTemp->isInputTextChaged.at(i))
                        {
                            intoBreakpoint = true;
                        }
                    }
                    for(int i=0;i<eleTemp->outputList.size();i++)
                    {
                        eleTemp->isOutputTextChaged.replace(i,QString::compare(eleTemp->outputList.at(i),outputListTemp.at(i)));
                        eleTemp->outputList.replace(i,outputListTemp.at(i));
                        if(eleTemp->isOutputPointBreak.at(i) && eleTemp->isOutputTextChaged.at(i))
                        {
                            intoBreakpoint = true;
                        }
                    }
                }

            }
        }
    }
    logView->viewport()->update();
}

void CfgDebugLog::startDebugMode(const QStringList &infoList)
{
    endDebugMode(); //clear
//    openFile();
//    QStringList list = readClkFromFile(1);
    elementItemInit(infoList);
}

void CfgDebugLog::endDebugMode()
{
//    closeFile();
    if(!this)
        return;
    if(!logView || !logScene)
        return;
    logView->centerOn(0,0);
    logScene->clear();
    itemList.clear();
    isElementItemInited = false;
    while(!listElementInfo.isEmpty())
    {
        delete listElementInfo.last();
        listElementInfo.removeLast();
    }
    listElementInfo.clear();
    mapElementItemList.clear();
    qApp->restoreOverrideCursor();
}

void CfgDebugLog::elementItemInit(const QStringList &infoList)
{
    if(infoList.size() <= 0)    return;
    if(isElementItemInited)     return;

    qreal xTemp = 0;
    qreal yTemp = 0;
    DebugLogItem *item = NULL;
    int maxItemHeightInLine = 0;
    int rcuBak = -1;
    int elementCount = 0;
    for(int index=0; index<infoList.size();index++)
    {
        QString textLine = infoList.at(index);
        QStringList textLineList = textLine.replace("]","").replace(" ","").split("[");
        ElementInfo *eleTemp = NULL;
        // read line info to element;

        if(textLineList.size() >= ELE_INFO_SPACE_NUM)
        {
            QStringList tempId = QString(textLineList.at(0)).split(":");//tempId[0] = BFU  (2,2,1); tempId[1] = 0 / 1;
            if(tempId.size() < 2) continue;

            QStringList strListTemp = QString(tempId.at(0)).replace(")","").split("("); //strListTemp[0] = BFU; strListTemp[1] = 2,2,1;
            if(strListTemp.size() < 2) continue;

            QString eleName = strListTemp.at(0);
            QStringList indexListTemp = QString(strListTemp.at(1)).split(",");
            if(indexListTemp.size() < 3) continue;
            int indexBcu = QString(indexListTemp.at(0)).toInt();
            int indexRcu = QString(indexListTemp.at(1)).toInt();
            int indexId  = QString(indexListTemp.at(2)).toInt();

            int indexEle = mapElementName.key(eleName,DEFAULT_KEY_MAP_ELEMENT);
            if (indexEle == DEFAULT_KEY_MAP_ELEMENT) continue;

//            QStringList inputListTemp = QString(textLineList.at(1)).replace("[","").replace("]","").split(",");
//            QStringList outputListTemp = QString(textLineList.at(2)).replace("[","").replace("]","").split(",");
            QStringList inputListTemp = QString(textLineList.at(1)).split(",");
            QStringList outputListTemp = QString(textLineList.at(2)).split(",");
            if(mapElementInput[indexEle] != inputListTemp.size()) continue;
            if(mapElementOutput[indexEle] != outputListTemp.size()) continue;

            QString strElementIdTemp = tempId.at(0);
            int elementKey = mapElementItemList.key(strElementIdTemp,DEFAULT_KEY_MAP_ELEMENT);
            if(DEFAULT_KEY_MAP_ELEMENT == elementKey)
            {
                // 算子还没初始化
                eleTemp = new ElementInfo();
                eleTemp->elementId = tempId.at(0);
                eleTemp->typeId = indexEle;
                eleTemp->bcu = indexBcu;
                eleTemp->rcu = indexRcu;
                eleTemp->id = indexId;
                eleTemp->isEnabled = QString(tempId.at(1)).toInt();
                eleTemp->isEnableChanged = eleTemp->isEnabled;

                eleTemp->inputList = inputListTemp;
                eleTemp->outputList = outputListTemp;

                for(int i=0;i<eleTemp->inputList.size();i++)
                {
                    eleTemp->isInputTextChaged.append(QString::compare(eleTemp->inputList.at(i),QString(ITEM_TEXT_INIT)));
                    eleTemp->isInputPointBreak.append(false);
                }
                for(int i=0;i<eleTemp->outputList.size();i++)
                {
                    eleTemp->isOutputTextChaged.append(QString::compare(eleTemp->outputList.at(i),QString(ITEM_TEXT_INIT)));
                    eleTemp->isOutputPointBreak.append(false);
                }

                if(rcuBak != eleTemp->rcu)
                {
                    rcuBak = eleTemp->rcu;
                    xTemp = ITEM_DISTANCE_WIDTH;
                    yTemp += maxItemHeightInLine+ITEM_DISTANCE_HEIGHT;
                }
                item = new DebugLogItem(eleTemp);
                item->setPos(xTemp,yTemp);
                logScene->addItem(item);
                xTemp += (item->getItemBounding().width()+50);

                if(maxItemHeightInLine < item->getItemBounding().height())
                {
                    maxItemHeightInLine = item->getItemBounding().height();
                }
                connect(item,SIGNAL(sendChangedElementInfo(QString,QStringList,QStringList)),this,SLOT(handleChangedElementInfo(QString,QStringList,QStringList)));
                connect(item,SIGNAL(addBreakPoint(QString,QString)),this,SLOT(handleAddBreakPoint(QString,QString)));
                connect(item,SIGNAL(deleteBreakPoint(QString,QString)),this,SLOT(handleDeleteBreakPoint(QString,QString)));
                listElementInfo.insert(elementCount,eleTemp);
                itemList.insert(elementCount,item);
                mapElementItemList.insert(elementCount,eleTemp->elementId);
                elementCount++;
            }
        }
    }
    isElementItemInited = true;
}

void CfgDebugLog::handleClickedItem(QGraphicsItem* clickedItem)
{
    DebugLogItem* itemTemp = dynamic_cast<DebugLogItem*>(clickedItem);
    int itemKey = itemList.indexOf(itemTemp,0);
    if(itemKey != -1)
    {
        emit sendClickedItem(itemList.at(itemKey)->elementItemInfo->elementId);
    }
}

void CfgDebugLog::handleClickedFromXml(QString elementId)
{
    int elementKey = mapElementItemList.key(elementId,DEFAULT_KEY_MAP_ELEMENT);
    if(elementKey != DEFAULT_KEY_MAP_ELEMENT)
    {
        logView->centerOn(itemList.at(elementKey));
        logScene->clearFocus();
        logScene->setFocusItem(itemList.at(elementKey));
    }
}

void CfgDebugLog::handleChangedElementInfo(QString str, QStringList inputList, QStringList outputList)
{
    QL_DEBUG << str << inputList << outputList;
    emit sendChangedElementInfo(str,inputList,outputList);
}

void CfgDebugLog::handleAddBreakPoint(QString str,QString portInfo)
{
    QL_DEBUG << str << portInfo;
    DebugBreakPoint breakPoint;
    breakPoint.pointStr = str + "," + portInfo;
    breakPoint.strName = str + "," + portInfo;
    for(int i=0;i<breakPointList.count();i++)
    {
        if(breakPointList.at(i).pointStr == breakPoint.pointStr)
        {
            return;
        }
    }
    breakPointList.append(breakPoint);
    QL_DEBUG << breakPointList.count();
    emit addBreakPoint(str,portInfo);
}

void CfgDebugLog::handleDeleteBreakPoint(QString str, QString portInfo)
{
    QL_DEBUG << str << portInfo;
    DebugBreakPoint breakPoint;
    breakPoint.pointStr = str + "," + portInfo;
    for(int i=0;i<breakPointList.count();i++)
    {
        if(breakPointList.at(i).pointStr == breakPoint.pointStr)
        {
            breakPointList.removeAt(i);
            break;
        }
    }
    QL_DEBUG << breakPointList.count();
    emit deleteBreakPoint(str,portInfo);
}
