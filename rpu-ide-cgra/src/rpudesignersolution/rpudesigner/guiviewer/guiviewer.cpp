/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    guiviewer.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include "guiviewer.h"
#include "attrtreewidget.h"
#include "capppathenv.h"
#include <QAction>
#include <QDir>
#include <QFileDialog>
#define ORIGINSIZEEDGE 100000
#define ZOOMRATE 1.25
#define ORIGINFONTSIZE 20

/**
 * GuiViewer::GuiViewer
 * \brief   constructor of GuiViewer
 * \param   scroll
 * \param   parent
 * \author  zhangjun
 * \date    2016-10-12
 */
GuiViewer::GuiViewer(QScrollArea *scroll, QWidget *parent):
    QWidget(parent)
{
    this->setWindowState(Qt::WindowMaximized);

    m_scrollArea = scroll;
    m_action = new QAction(this);
    m_action->setCheckable(true);
    connect(m_action, SIGNAL(triggered()), this, SLOT(show()));
    connect(m_action, SIGNAL(triggered()), this, SLOT(setFocus()));
    connect(m_action, SIGNAL(triggered()), this, SIGNAL(activeWindowChanged()));
    m_isUntitled = false;
    m_isModified = false;
//    m_isProject = false;
    m_scrollArea->setWindowTitle("[*]");
    setAttribute(Qt::WA_DeleteOnClose);


    //初始化
    for(int i = 0; i<BCUMAX; i++)
    {
        for(int j = 0; j<RCUMAX; j++)
        {
            int k = 0;
            for(k = 0; k<BFUMAX; k++)
            {
                EElements[i][j][k] = new EElement{i,j,ELEMENT_BFU,k,false,NULL};
            }
            for(; k<BFUMAX+SBOXMAX; k++)
            {
                EElements[i][j][k] = new EElement{i,j,ELEMENT_SBOX,k,false,NULL};
            }
            for(; k<BFUMAX+SBOXMAX+BENESMAX; k++)
            {
                EElements[i][j][k] = new EElement{i,j,ELEMENT_BENES,k,false,NULL};
            }
        }
    }
    spos.setX(0);
    spos.setY(0);
    currentPos.setX(0);
    currentPos.setY(0);
    rate = 1.0;
    changeStatus(NORMAL);
    //四个固定模块
    moduleInfifo = new ModuleBody(QPointF(0,0),ELEMENT_INFIFO,0,0,0);
    moduleInMemery = new ModuleBody(QPointF(geometry().width()/2,0),ELEMENT_INMEM,0,0,0);
    moduleOutfifo = new ModuleBody(QPointF(0,m_scrollArea->height()-MODULEHEIGHT-m_scrollArea->horizontalScrollBar()->geometry().height()),ELEMENT_OUTFIFO,0,0,0);
    moduleOutMemery = new ModuleBody(QPointF(geometry().width()/2,m_scrollArea->height()-MODULEHEIGHT-m_scrollArea->horizontalScrollBar()->geometry().height()),ELEMENT_OUTMEM,0,0,0);
    moduleBodyList << moduleInMemery;
    moduleBodyList << moduleInfifo;
    //    moduleBodyList << moduleOutMemery;
    //    moduleBodyList << moduleOutfifo;
    tempWire = NULL;

    //    //用xml解析器初始化界面
    //    initUI();
    //    //智能调整UI
    //    adjustUI();
    //初始化
    touchedBodyIndex = -1;
    touchedWireIndex = -1;
    touchPointIndex = -1;
    lastBodyMap.insert(ELEMENT_BFU,NULL);
    lastBodyMap.insert(ELEMENT_SBOX,NULL);
    lastBodyMap.insert(ELEMENT_BENES,NULL);
    press = false;
    //设置size
    setGeometry(0,0,ORIGINSIZEEDGE,ORIGINSIZEEDGE);
    setStyleSheet("background-color:white");
    //    m_scrollArea = scroll;

    //鼠标踪迹
    press = false;
    setMouseTracking(true);
    setFocusPolicy(Qt::WheelFocus);
    connect(m_scrollArea->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(onScrolBarValueChanged(int)));
    connect(m_scrollArea->horizontalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(onScrolBarValueChanged(int)));
    //显示连线
    showAllWire = false;
    showWireIndexList.clear();
}

/**
 * GuiViewer::~GuiViewer
 * \brief   destructor of GuiViewer
 * \author  zhangjun
 * \date    2016-10-12
 */
GuiViewer::~GuiViewer()
{

    delete m_xml;
    delete m_attrTreeWidget;
    delete m_action;

    qDeleteAll(moduleBodyList);
    qDeleteAll(moduleWireList);
//    for(int i = 0; i < moduleBodyList.count(); ++i)
//    {
//        delete moduleBodyList.at(i);
//    }
//    delete moduleInMemery;
//    delete moduleInfifo;
//    delete moduleOutMemery;
//    delete moduleOutfifo;
    delete tempWire;
    for(int i = 0; i<BCUMAX; i++)
    {
        for(int j = 0; j<RCUMAX; j++)
        {
            for(int k = 0; k<BFUMAX; k++)
            {
                delete EElements[i][j][k];
            }
        }
    }
    qDebug() << "~GuiViewer()";
}

/**
 * GuiViewer::newFile
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::newFile()
{
    static int documentNumber = 1;

    m_curFile = CAppPathEnv::getTmpDirPath() + "/" + tr(u8"untitled%1.xml").arg(documentNumber);
    m_scrollArea->setWindowTitle(CAppPathEnv::stpToNm(m_curFile) + "[*]");
    m_action->setText(m_curFile);

    m_xml = new XmlResolver(m_curFile);  //xml文件解析

    m_attrTreeWidget = new AttrTreeWidget(m_curFile, m_xml);
    ++documentNumber;
    m_isUntitled = true;
    m_isModified = true;
    m_scrollArea->setWindowModified(m_isModified);

    //用xml解析器初始化界面
    initUI();
    //智能调整UI
//    adjustUI();
}

/**
 * GuiViewer::open
 * \brief   
 * \param   fileName
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool GuiViewer::open(QString fileName)
{
    setCurrentFile(fileName);

    m_xml = new XmlResolver(m_curFile);  //xml文件解析

    m_attrTreeWidget = new AttrTreeWidget(m_curFile,m_xml);

    //用xml解析器初始化界面
    initUI();
    //智能调整UI
//    adjustUI();

    m_isModified = false;
    m_scrollArea->setWindowModified(m_isModified);
    return true;
}

/**
 * GuiViewer::stpToNm
 * \brief   
 * \param   fullFileName
 * \return  QString
 * \author  zhangjun
 * \date    2016-10-12
 */
//QString GuiViewer::stpToNm(const QString &fullFileName)
//{
//    return QFileInfo(fullFileName).fileName();
//}

/**
 * GuiViewer::getXml
 * \brief   
 * \return  XmlResolver*
 * \author  zhangjun
 * \date    2016-10-12
 */
XmlResolver* GuiViewer::getXml()
{
    return m_xml;
}

/**
 * GuiViewer::getAttrTreeWidget
 * \brief   
 * \return  AttrTreeWidget *
 * \author  zhangjun
 * \date    2016-10-12
 */
AttrTreeWidget *GuiViewer::getAttrTreeWidget()
{
    return m_attrTreeWidget;
}

/**
 * GuiViewer::getShowAllWire
 * \brief   
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool GuiViewer::getShowAllWire()
{
    return showAllWire;
}

/**
 * GuiViewer::getIsProject
 * \brief   
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
//bool GuiViewer::getIsProject()
//{
//    return m_isProject;
//}

/**
 * GuiViewer::setIsProject
 * \brief   
 * \param   isProject
 * \author  zhangjun
 * \date    2016-10-12
 */
//void GuiViewer::setIsProject(bool isProject)
//{
//    m_isProject = isProject;
//}

bool GuiViewer::getIsModified()
{
    return m_isModified;
}

bool GuiViewer::getIsUntitled()
{
    return m_isUntitled;
}

/**
 * GuiViewer::closeEvent
 * \brief   
 * \param   event
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::closeEvent(QCloseEvent *event)
{
    if (okToContinue()) {
        emit guiViewerClose(m_curFile);
        event->accept();
    } else {
        event->ignore();
    }
}

/**
 * GuiViewer::okToContinue
 * \brief   
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool GuiViewer::okToContinue()
{
    if (m_isModified) {
        int ret = QMessageBox::warning(0, tr(u8"RCA Sim"),
                                       tr(u8"是否保存对 “%1” 的修改？")
                                       .arg(CAppPathEnv::stpToNm(m_curFile)),
                                       QMessageBox::Yes | QMessageBox::No
                                       | QMessageBox::Cancel);
        if (ret == QMessageBox::Yes) {
            return save();
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

/**
 * GuiViewer::save
 * \brief   
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool GuiViewer::save()
{
    if (m_isUntitled) {
        return saveAs();
    } else {
        return saveFile();
    }
}

/**
 * GuiViewer::saveAs
 * \brief   
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool GuiViewer::saveAs()
{
//    QString str = CAppPathEnv::getXmlOutDirPath() + "/" + CAppPathEnv::stpToNm(m_curFile);
    QString fileName = QFileDialog::getSaveFileName(this, tr(u8"另存为"), ""/*str*/, tr(u8"XML 文件 (*.xml)"));
    if (fileName.isEmpty())
    {
        return false;
    }

    return saveFile(fileName);
}

/**
 * GuiViewer::saveFile
 * \brief   
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool GuiViewer::saveFile()
{
    if (m_xml->save()) {
        setCurrentFile(m_curFile);
        return true;
    } else {
        return false;
    }
}

/**
 * GuiViewer::saveFile
 * \brief   
 * \param   fileName
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool GuiViewer::saveFile(const QString &fileName)
{
    if (m_xml->saveAs(fileName)) {
        setCurrentFile(fileName);
        return true;
    } else {
        return false;
    }
}

/**
 * GuiViewer::setCurrentFile
 * \brief   
 * \param   fileName
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::setCurrentFile(const QString &fileName)
{
    m_curFile = fileName;
    m_isUntitled = false;
    m_action->setText(CAppPathEnv::stpToNm(m_curFile));
    m_isModified = false;
    m_scrollArea->setWindowModified(m_isModified);
    m_scrollArea->setWindowTitle(CAppPathEnv::stpToNm(m_curFile) + "[*]");
}

/**
 * GuiViewer::onScrolBarValueChanged
 * \brief   
 * \param   int
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::onScrolBarValueChanged(int)
{
    resizeSlot();
}

/**
 * GuiViewer::updateUI
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::updateUI()
{
    for(int i = 0; i<moduleWireList.count(); i++)
    {
        ModuleWire *wire = moduleWireList.at(i);
        if(wire)
        {
            if(wire->input)wire->input->removeOutputWire(wire,wire->inputPosIndex);
            if(wire->output)wire->output->removeInputWire(wire,wire->outputPosIndex);
            delete wire;
        }
    }
    for(int i = 2; i<moduleBodyList.count(); i++)
    {
        ModuleBody *body = moduleBodyList.at(i);
        delete body;
    }
    for(int i = 0; i<BCUMAX; i++)
    {
        for(int j = 0; j<RCUMAX; j++)
        {
            for(int k = 0; k<BFUMAX+SBOXMAX+BENESMAX; k++)
            {
                EElements[i][j][k]->body = NULL;
                EElements[i][j][k]->en = false;
            }
        }
    }
    lastBodyMap.insert(ELEMENT_BFU,NULL);
    lastBodyMap.insert(ELEMENT_SBOX,NULL);
    lastBodyMap.insert(ELEMENT_BENES,NULL);
    moduleBodyList.clear();
    moduleWireList.clear();
    moduleBodyList << moduleInMemery;
    moduleBodyList << moduleInfifo;
    changeStatus(NORMAL);
    initUI();
    updateShowWireIndexList();
}
/**
 * GuiViewer::setShowAllWire
 * \brief   
 * \param   en
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::setShowAllWire(bool en)
{
    this->showAllWire = en;
    update();
}

/**
 * GuiViewer::resizeSlot
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::resizeSlot()
{
    moduleInfifo->setRect(m_scrollArea->horizontalScrollBar()->value()/rate,
                          m_scrollArea->verticalScrollBar()->value()/rate,
                          m_scrollArea->geometry().width()/2/rate,moduleInfifo->rect.height());
    moduleInMemery->setRect(m_scrollArea->horizontalScrollBar()->value()/rate+m_scrollArea->geometry().width()/2/rate,
                            m_scrollArea->verticalScrollBar()->value()/rate,
                            m_scrollArea->geometry().width()/2/rate,
                            moduleInMemery->rect.height()
                            );
    moduleOutfifo->setRect(m_scrollArea->horizontalScrollBar()->value()/rate,
                           (m_scrollArea->verticalScrollBar()->value()+m_scrollArea->height()-moduleOutfifo->rect.height()*rate-m_scrollArea->horizontalScrollBar()->geometry().height())/rate,
                           m_scrollArea->geometry().width()/2/rate,moduleOutfifo->rect.height());
    moduleOutMemery->setRect(m_scrollArea->horizontalScrollBar()->value()/rate+m_scrollArea->geometry().width()/2/rate,
                             (m_scrollArea->verticalScrollBar()->value()+m_scrollArea->height()-moduleOutMemery->rect.height()*rate-m_scrollArea->horizontalScrollBar()->geometry().height())/rate,
                             m_scrollArea->geometry().width()/2/rate,
                             moduleOutMemery->rect.height()
                             );
    this->update();
}


/**
 * GuiViewer::pressFirstWireDot
 * \brief   
 * \param   pos
 * \param   start
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool GuiViewer::pressFirstWireDot(QPoint pos, bool start)
{
    for(int i = 0; i<moduleBodyList.count(); i++)
    {
        firstWireDot = moduleBodyList.at(i)->isEnterOutput(pos);
        if(firstWireDot >= 0)
        {
            if(!start)
            {
                int FirstBcu = moduleBodyList.at(i)->bcuID;
                int FirstRcu = moduleBodyList.at(i)->rcuID;
                if(isEnableToConnect(FirstBcu,FirstRcu,secondBody->bcuID,secondBody->rcuID) ||
                        moduleBodyList.at(i)->element == ELEMENT_INFIFO ||
                        moduleBodyList.at(i)->element == ELEMENT_INMEM)
                {
                    firstBody = moduleBodyList.at(i);
                    firstPos = pos;
                    return true;
                }
                else firstWireDot = -1;
            }
            else
            {
                firstBody = moduleBodyList.at(i);
                firstPos = pos;
                return true;
            }
        }
        else firstWireDot = -1;
    }
    return false;
}

/**
 * GuiViewer::findBodyIndex
 * \brief   
 * \param   bcu
 * \param   rcu
 * \param   ele
 * \param   id
 * \return  int
 * \author  zhangjun
 * \date    2016-10-12
 */
int GuiViewer::findBodyIndex(int bcu, int rcu, ElementType ele, int id)
{
    for(QList<ModuleBody*>::iterator i = moduleBodyList.begin(); i!=moduleBodyList.end(); i++)
    {
        if((*i)->bcuID == bcu && (*i)->rcuID == rcu && (*i)->element == ele && (*i)->ID == id)return moduleBodyList.indexOf(*i);
    }
    return -1;
}

/**
 * GuiViewer::updateShowWireIndexList
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::updateShowWireIndexList()
{
    showWireIndexList.clear();
    for(int i = 0; i<touchedBodyIndexList.count(); i++)
    {
        ModuleBody *body = moduleBodyList.at(touchedBodyIndexList.at(i));
        for(int j = 0; j<body->inputCount; j++)
        {
            ModuleWire *wire = body->inputPosMap.values().at(j);
            int index = moduleWireList.indexOf(wire);
            if(index>=0 && !showWireIndexList.contains(index))
                showWireIndexList << index;
        }
        for(int j = 0; j<body->outputCount; j++)
        {
            for(int k = 0; k<body->outputPosMap.values().at(j)->count(); k++)
            {
                ModuleWire *wire = body->outputPosMap.values().at(j)->at(k);
                int index = moduleWireList.indexOf(wire);
                if(index>=0 && !showWireIndexList.contains(index))
                    showWireIndexList << index;
            }
        }
    }
}

/**
 * GuiViewer::setElementExsit
 * \brief   
 * \param   bcu
 * \param   rcu
 * \param   ele
 * \param   id
 * \param   en
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::setElementExsit(int bcu, int rcu, ElementType ele, int id, bool en)
{
    int index = (ele==ELEMENT_BFU)?id:
                                   (ele==ELEMENT_SBOX)?(id+BFUMAX):
                                                       (ele==ELEMENT_BENES)?(id+BFUMAX+SBOXMAX):-1;
    if(index != -1)
        this->EElements[bcu][rcu][index]->en = en;
    if(!en)this->EElements[bcu][rcu][index]->body = NULL;
}

/**
 * GuiViewer::getElementExsit
 * \brief   
 * \param   bcu
 * \param   rcu
 * \param   ele
 * \param   id
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool GuiViewer::getElementExsit(int bcu, int rcu, ElementType ele, int id)
{
    int index = (ele==ELEMENT_BFU)?id:
                                   (ele==ELEMENT_SBOX)?(id+BFUMAX):
                                                       (ele==ELEMENT_BENES)?(id+BFUMAX+SBOXMAX):-1;
    return (index == -1) || this->EElements[bcu][rcu][index]->en;
}

/**
 * GuiViewer::isEnableToConnect
 * \brief   
 * \param   sbcu
 * \param   srcu
 * \param   dbcu
 * \param   drcu
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool GuiViewer::isEnableToConnect(int sbcu, int srcu, int dbcu, int drcu)
{
    if(sbcu == dbcu)
    {
        if(srcu == drcu || srcu+1 == drcu)return true;
        return false;
    }
    else
    {
        if(srcu == RCUMAX-1 && drcu == 0)return true;
        return false;
    }
}

/**
 * GuiViewer::pressSecondWireDot
 * \brief   
 * \param   pos
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool GuiViewer::pressSecondWireDot(QPoint pos)
{
    for(int i = 0; i<moduleBodyList.count(); i++)
    {
        secondWireDot = moduleBodyList.at(i)->isEnterInput(pos);
        if(secondWireDot >= 0)
        {
            if(firstBody->element == ELEMENT_INFIFO || firstBody->element == ELEMENT_INMEM)
            {
                secondBody = moduleBodyList.at(i);
                secondPos = pos;
                return true;
            }
            int WireBcu = moduleBodyList.at(i)->bcuID;
            int WireRcu = moduleBodyList.at(i)->rcuID;
            if(isEnableToConnect(firstBody->bcuID,firstBody->rcuID,WireBcu,WireRcu))
            {
                secondBody = moduleBodyList.at(i);
                secondPos = pos;
                return true;
            }
            else secondWireDot = -1;
        }
        else
        {
            secondWireDot = -1;
        }
    }
    return false;
}

/**
 * GuiViewer::initUI
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::initUI()
{
    for(int i = 0; i<BCUMAX; i++)
    {
        for(int j = 0; j<RCUMAX; j++)
        {
            for(int k = 0; k<BFUMAX; k++)
            {
                if(m_xml->isElementExist(i,j,ELEMENT_BFU,k))
                {
                    //bfu
                    int x = m_xml->getElementAttributeValue(i,j,ELEMENT_BFU,k,PosX,0);
                    int y = m_xml->getElementAttributeValue(i,j,ELEMENT_BFU,k,PosY,0);
                    ModuleBody *bfu = new ModuleBody(QPointF(x,y),ELEMENT_BFU,i,j,k);
                    addBody(i,j,k,bfu);
                }
            }
            for(int k = 0; k<SBOXMAX; k++)
            {
                if(m_xml->isElementExist(i,j,ELEMENT_SBOX,k))
                {
                    int x = m_xml->getElementAttributeValue(i,j,ELEMENT_SBOX,k,PosX,0);
                    int y = m_xml->getElementAttributeValue(i,j,ELEMENT_SBOX,k,PosY,0);
                    ModuleBody *sbox = new ModuleBody(QPointF(x,y),ELEMENT_SBOX,i,j,k);
                    addBody(i,j,k,sbox);
                }
            }
            for(int k = 0; k<BENESMAX; k++)
            {
                if(m_xml->isElementExist(i,j,ELEMENT_BENES,k))
                {
                    int x = m_xml->getElementAttributeValue(i,j,ELEMENT_BENES,k,PosX,0);
                    int y = m_xml->getElementAttributeValue(i,j,ELEMENT_BENES,k,PosY,0);
                    ModuleBody *benes = new ModuleBody(QPointF(x,y),ELEMENT_BENES,i,j,k);
                    addBody(i,j,k,benes);
                }
            }
        }
    }
    //wire

    for(int i = 0; i<moduleBodyList.count(); i++)
    {
        ModuleBody *body = moduleBodyList.at(i);
        int bcu = body->bcuID;
        int rcu = body->rcuID;
        ElementType element = body->element;
        int id = body->ID;
        switch(element)
        {
        case ELEMENT_BFU:
        {
            for(int i = 0; i<3; i++)
            {
                int type = m_xml->getElementAttributeValue(bcu,rcu,ELEMENT_BFU,id,(AttributeID)(BfuInputA_Type+i));
                int index = m_xml->getElementAttributeValue(bcu,rcu,ELEMENT_BFU,id,(AttributeID)(BfuInputA_Index+i));
                int sourceBcu = m_xml->getElementAttributeValue(bcu,0,ELEMENT_BFU,0,BCUROUTER);
                ModuleBody *source = NULL;
                int posIndex = 0;
                getInputSource(type,index,body,sourceBcu,source,posIndex);
                if(source)
                {
                    ModuleWire *wire = new ModuleWire(QPointF(source->outputPosMap.keys().at(posIndex)->x(),
                                                              source->outputPosMap.keys().at(posIndex)->y()),
                                                      QPointF(body->inputPosMap.keys().at(i)->x(),
                                                              body->inputPosMap.keys().at(i)->y()));
                    addWire(source,posIndex,body,i,wire);
                }
            }

            break;
        }
        case ELEMENT_SBOX:
        {
            int type = m_xml->getElementAttributeValue(bcu,rcu,ELEMENT_SBOX,id,SboxInput_Type);
            int index = m_xml->getElementAttributeValue(bcu,rcu,ELEMENT_SBOX,id,SboxInput_Index);
            int sourceBcu = m_xml->getElementAttributeValue(bcu,0,ELEMENT_BFU,0,BCUROUTER);
            ModuleBody *source = NULL;
            int posIndex = 0;
            getInputSource(type,index,body,sourceBcu,source,posIndex);
            if(source)
            {
                ModuleWire *wire = new ModuleWire(QPointF(source->outputPosMap.keys().at(posIndex)->x(),
                                                          source->outputPosMap.keys().at(posIndex)->y()),
                                                  QPointF(body->inputPosMap.keys().at(0)->x(),
                                                          body->inputPosMap.keys().at(0)->y()));
                addWire(source,posIndex,body,0,wire);
            }
            break;
        }
        case ELEMENT_BENES:
        {
            for(int i = 0; i<4; i++)
            {
                int type = m_xml->getElementAttributeValue(bcu,rcu,ELEMENT_BENES,id,(AttributeID)(BenesInput0_Type+i));
                int index = m_xml->getElementAttributeValue(bcu,rcu,ELEMENT_BENES,id,(AttributeID)(BenesInput0_Index+i));
                int sourceBcu = m_xml->getElementAttributeValue(bcu,0,ELEMENT_BFU,0,BCUROUTER);
                ModuleBody *source = NULL;
                int posIndex = 0;
                getInputSource(type,index,body,sourceBcu,source,posIndex);
                if(source)
                {
                    ModuleWire *wire = new ModuleWire(QPointF(source->outputPosMap.keys().at(posIndex)->x(),
                                                              source->outputPosMap.keys().at(posIndex)->y()),
                                                      QPointF(body->inputPosMap.keys().at(i)->x(),
                                                              body->inputPosMap.keys().at(i)->y()));
                    addWire(source,posIndex,body,i,wire);
                }
            }
            break;
        }
        default:break;
        }


    }
    update();

}

/**
 * GuiViewer::adjustUI
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
//void GuiViewer::adjustUI()
//{

//}

/**
 * GuiViewer::addWire
 * \brief   
 * \param   source
 * \param   sourcePosIndex
 * \param   dest
 * \param   destPosIndex
 * \param   wire
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::addWire(ModuleBody *source, int sourcePosIndex, ModuleBody *dest, int destPosIndex, ModuleWire *wire)
{
    wire->setInput(source,sourcePosIndex);
    wire->setOutput(dest,destPosIndex);
    source->addOutput(sourcePosIndex,wire);
    dest->setInput(destPosIndex,wire);
    moduleWireList << wire;
    source->movePos(QPoint(0,0));
    dest->movePos(QPoint(0,0));
    //    wire->updateHeight();
    //读取高度
    AttributeID heightID = ID;
    if(dest->element==ELEMENT_BFU)heightID=(AttributeID)(BfuInputA_Height+destPosIndex);
    else if(dest->element==ELEMENT_SBOX)heightID=SboxInput_Height;
    else if(dest->element==ELEMENT_BENES)heightID=(AttributeID)(BenesInput0_Height+wire->outputPosIndex);
    int h = m_xml->getElementAttributeValue(dest->bcuID,dest->rcuID,dest->element,dest->ID,heightID);
    h>0?(wire->setHeight(h)):void();

    m_isModified = true; //modified by zhangjun at 2016/8/27
    m_scrollArea->setWindowModified(m_isModified);
}

/**
 * GuiViewer::addBody
 * \brief   
 * \param   bcu
 * \param   rcu
 * \param   id
 * \param   body
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::addBody(int bcu, int rcu, int id, ModuleBody *body)
{
    ElementType ele = body->element;
    int index = (ele==ELEMENT_BFU)?id:
                                   (ele==ELEMENT_SBOX)?(id+BFUMAX):
                                                       (ele==ELEMENT_BENES)?(id+BFUMAX+SBOXMAX):-1;
    if(index != -1)
    {
        EElements[bcu][rcu][index]->body = body;
        EElements[bcu][rcu][index]->en = true;
        moduleBodyList << body;
        lastBodyMap.insert(body->element,body);
    }

    m_isModified = true; //modified by zhangjun at 2016/8/27
    m_scrollArea->setWindowModified(m_isModified);

}
/**
 * GuiViewer::removeBody
 * \brief   
 * \param   body
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::removeBody(ModuleBody *body)
{
    if(body == NULL)return;
    ElementType ele = body->element;
    int index = (ele==ELEMENT_BFU)?body->ID:
                                   (ele==ELEMENT_SBOX)?(body->ID+BFUMAX):
                                                       (ele==ELEMENT_BENES)?(body->ID+BFUMAX+SBOXMAX):-1;
    if(index != -1)
    {
        EElements[body->bcuID][body->rcuID][index]->body = NULL;
        EElements[body->bcuID][body->rcuID][index]->en = false;
        moduleBodyList.removeOne(body);
        m_xml->removeElement(body->bcuID,body->rcuID,body->element,body->ID);
        delete body;
    }

    m_isModified = true; //modified by zhangjun at 2016/8/27
    m_scrollArea->setWindowModified(m_isModified);
}

/**
 * GuiViewer::getInputSource
 * \brief   
 * \param   type
 * \param   index
 * \param   dest
 * \param   sourceBcu
 * \param   source
 * \param   posIndex
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::getInputSource(int type, int index, ModuleBody *dest, int sourceBcu, ModuleBody *&source, int &posIndex)
{
    Q_UNUSED(sourceBcu)

#define RCUSS \
    rcu--;\
    if(rcu < 0)\
    {\
    rcu=3;\
    bcu=sourceBcu;\
    if(bcu<0)\
    {\
    source=NULL;\
    break;\
}\
}

    int bcu = dest->bcuID;
    int rcu = dest->rcuID;
    int id = 0;
    ElementType element;
    Q_UNUSED(element)
    switch(type)
    {
    case 5:{RCUSS}
    case 1: //curbfux
    {
        element = ELEMENT_BFU;
        id = index;
        posIndex = 0;
        source = EElements[bcu][rcu][id]->body;
        if(source==NULL)
        {
            ModuleBody *body = new ModuleBody(QPointF(m_scrollArea->horizontalScrollBar()->value()/rate+m_scrollArea->width()/2,
                                                      m_scrollArea->verticalScrollBar()->value()/rate+m_scrollArea->height()/2),ELEMENT_BFU,bcu,rcu,id);
            addBody(bcu,rcu,id,body);
            source = body;
        }
        break;
    }
    case 6:{RCUSS}
    case 2: //curbfuy
    {
        element = ELEMENT_BFU;
        id = index;
        posIndex = 1;
        source = EElements[bcu][rcu][id]->body;
        if(source==NULL)
        {
            ModuleBody *body = new ModuleBody(QPointF(m_scrollArea->horizontalScrollBar()->value()/rate+m_scrollArea->width()/2,
                                                      m_scrollArea->verticalScrollBar()->value()/rate+m_scrollArea->height()/2),ELEMENT_BFU,bcu,rcu,id);
            addBody(bcu,rcu,id,body);
            source = body;
        }
        break;
    }
    case 7:{RCUSS}
    case 3: //cursbox
    {
        element = ELEMENT_SBOX;
        id = index/SBOXMAX;
        posIndex = index%4;
        source = EElements[bcu][rcu][id+BFUMAX]->body;
        if(source==NULL)
        {
            ModuleBody *body = new ModuleBody(QPointF(m_scrollArea->horizontalScrollBar()->value()/rate+m_scrollArea->width()/2,
                                                      m_scrollArea->verticalScrollBar()->value()/rate+m_scrollArea->height()/2),ELEMENT_SBOX,bcu,rcu,id);
            addBody(bcu,rcu,id,body);
            source = body;
        }
        break;
    }
    case 8:{RCUSS}
    case 4: //curbenes
    {
        element = ELEMENT_BENES;
        id = 0;
        posIndex = index%4;
        source = EElements[bcu][rcu][id+BFUMAX+SBOXMAX]->body;
        if(source==NULL)
        {
            ModuleBody *body = new ModuleBody(QPointF(m_scrollArea->horizontalScrollBar()->value()/rate+m_scrollArea->width()/2,
                                                      m_scrollArea->verticalScrollBar()->value()/rate+m_scrollArea->height()/2),ELEMENT_BENES,bcu,rcu,id);
            addBody(bcu,rcu,id,body);
            source = body;
        }
        break;
    }
    case 9: //inputfifo
    {
        element = ELEMENT_INFIFO;
        bcu = 0;
        rcu = 0;
        id = 0;
        posIndex = index;
        source = moduleInfifo;
        break;
    }
    case 10:
    {
        element = ELEMENT_INMEM;
        bcu = 0;
        rcu = 0;
        id = 0;
        posIndex = index;
        source = moduleInMemery;
        break;
    }
    default:
    {
        source = NULL;
        break;
    }
    }
}

/**
 * GuiViewer::getFitID
 * \brief   
 * \param   element
 * \param   bcu
 * \param   rcu
 * \param   id
 * \param   loop
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool GuiViewer::getFitID(ElementType element, int &bcu, int &rcu, int &id, bool loop)
{
    if(loop && lastBodyMap.value(element))
    {
        bcu = lastBodyMap.value(element)->bcuID;
        rcu = lastBodyMap.value(element)->rcuID;
        id = lastBodyMap.value(element)->ID;
    }
    else
    {
        bcu = 0;
        rcu = 0;
        id = 0;
    }
    switch(element)
    {
    case ELEMENT_BFU:
    {
        while(EElements[bcu][rcu][id]->en)
        {
            id++;
            if(id == BFUMAX)
            {
                id = 0;
                rcu ++;
                if(rcu == RCUMAX)
                {
                    rcu = 0;
                    bcu ++;
                    if(bcu == BCUMAX)
                    {
                        if(loop && lastBodyMap.value(element))return getFitID(element,bcu,rcu,id,false);
                        else return false;
                    }
                }
            }
        }
        return true;
    }
    case ELEMENT_SBOX:
    {
        if(id>=SBOXMAX)id=0;
        while(EElements[bcu][rcu][id+BFUMAX]->en)
        {
            id++;
            if(id == SBOXMAX)
            {
                id = 0;
                rcu ++;
                if(rcu == RCUMAX)
                {
                    rcu = 0;
                    bcu ++;
                    if(bcu == BCUMAX){
                        if(loop && lastBodyMap.value(element))return getFitID(element,bcu,rcu,id,false);
                        else return false;
                    }
                }
            }
        }
        return true;
    }
    case ELEMENT_BENES:
    {
        if(id>=BENESMAX)id=0;
        while(EElements[bcu][rcu][id+BFUMAX+SBOXMAX]->en)
        {
            id++;
            if(id == BENESMAX)
            {
                id = 0;
                rcu ++;
                if(rcu == RCUMAX)
                {
                    rcu = 0;
                    bcu ++;
                    if(bcu == BCUMAX){
                        if(loop && lastBodyMap.value(element))return getFitID(element,bcu,rcu,id,false);
                        else return false;
                    }
                }
            }
        }
        return true;
    }
    default:return false;
    }


}

/**
 * GuiViewer::changeID
 * \brief   
 * \param   body
 * \param   bcu
 * \param   rcu
 * \param   id
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::changeID(ModuleBody *body, int bcu, int rcu, int id)
{
    if(getElementExsit(bcu,rcu,body->element,id))
    {
        //comment out by zhangjun at 2016/8/29
        //        QMessageBox *box = new QMessageBox(QMessageBox::Warning,QApplication::applicationName(),trUtf8("该元素ID已经存在。"));
        //        box->exec();
        QMessageBox::warning(this, qApp->applicationName(), tr(u8"该元素ID已经存在！"));//add by zhangjun at 2016/8/29
        return;
    }
    //xml属性值
    QDomElement ele_old = m_xml->findElement(body->bcuID,body->rcuID,body->element,body->ID);
    if(body->bcuID == bcu && body->rcuID == rcu)
    {
        ele_old.setAttribute("id",id);
    }
    else
    {
        QDomElement ele_new = m_xml->findElement(bcu,rcu,body->element,id);
        QDomElement parent_old = ele_old.parentNode().toElement();
        //如果是当前行则直接改变id值

        //    QDomElement parent_new = m_xml->findRCUELement(bcu,rcu);

        QList<QDomElement> toAddElementList;
        QDomElement temp_ele = ele_old.firstChild().toElement();
        while(!temp_ele.isNull())
        {
            toAddElementList << temp_ele;
            temp_ele = temp_ele.nextSibling().toElement();
        }
        for(int i = 0; i<toAddElementList.count(); i++)
        {
            ele_new.appendChild(toAddElementList.at(i));
        }
        for(int i = 0; i<ele_old.attributes().count(); i++)
        {
            ele_new.setAttribute(ele_old.attributes().item(i).nodeName(),ele_old.attributes().item(i).nodeValue());
        }
        parent_old.removeChild(ele_old);
        ele_new.setAttribute("id",id);
    }

    //    parent_new.appendChild(ele);
    //显示UI
    setElementExsit(body->bcuID,body->rcuID,body->element,body->ID,false);
    setElementExsit(bcu,rcu,body->element,id,true);
    body->bcuID = bcu;
    body->rcuID = rcu;
    body->ID = id;
    //用于获取下一个自动添加的id
    touchedBodyIndexList.clear();
    lastBodyMap.insert(body->element,body);
    body->update();
    showAttribue(body->element,bcu,rcu,id);
    updateUI();
    touchedBodyIndex = findBodyIndex(bcu,rcu,body->element,id);
    touchedBodyIndexList << touchedBodyIndex;
    update();
    //    updateShowWireIndexList();

    m_isModified = true; //modified by zhangjun at 2016/8/27
    m_scrollArea->setWindowModified(m_isModified);
}

/**
 * GuiViewer::addElement
 * \brief   
 * \param   element
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::addElement(ElementType element)
{
    if(element == ELEMENT_WIRE)
    {
        changeStatus(ADDWIRE1);
        return;
    }
    int bcu = 0;
    int rcu = 0;
    int id = 0;
    if(getFitID(element,bcu,rcu,id))
    {
        preAddElement = element;
        preAddBcu = bcu;
        preAddRcu = rcu;
        preAddId = id;
        changeStatus(ADDBODY);
    }
    else
    {
        //comment out by zhangjun at 2016/8/29
        //        QMessageBox *box = new QMessageBox(QMessageBox::Warning,"cfg",trUtf8("当前元素数量已经达到最大值！"),QMessageBox::Yes);
        //        box->exec();
        QMessageBox::warning(this, qApp->applicationName(), tr(u8"当前元素数量已经达到最大值！"));//add by zhangjun at 2016/8/29
    }

    //    m_isModified = true; //modified by zhangjun at 2016/8/27
    //    m_scrollArea->setWindowModified(m_isModified);
}




///*如果来源为NULL则赋值为0*/
//void GuiViewer::setInputAttribute(ModuleBody *firstBody, int firstWireDot, ModuleBody *secondBody, int secondWireDot)
//{
//    if(secondBody == NULL)return;

//    ElementType element = secondBody->element;
//    AttributeID attr_type = (element == ELEMENT_BFU)?(AttributeID)(BfuInputA_Type+secondWireDot):
//                            (element == ELEMENT_SBOX)?(SboxInput_Type):
//                            (element == ELEMENT_BENES)?(AttributeID)(BenesInput0_Type+secondWireDot):
//                            (element == ELEMENT_OUTMEM)?(AttributeID)(RCUMEM0_Type+secondWireDot):(AttributeID)(BCUOUTFIFO0_Type+secondWireDot);
//    AttributeID attr_index = (element == ELEMENT_BFU)?(AttributeID)(BfuInputA_Index+secondWireDot):
//                             (element == ELEMENT_SBOX)?(SboxInput_Index):
//                             (element == ELEMENT_BENES)?(AttributeID)(BenesInput0_Index+secondWireDot):
//                             (element == ELEMENT_OUTMEM)?(AttributeID)(RCUMEM0_Index+secondWireDot):(AttributeID)(BCUOUTFIFO0_Index+secondWireDot);

//    if(firstBody == NULL)
//    {
//        m_xml->setElementAttribute(secondBody->bcuID,secondBody->rcuID,secondBody->element,
//                                 secondBody->ID,attr_type,0);
//        m_xml->setElementAttribute(secondBody->bcuID,secondBody->rcuID,secondBody->element,
//                                 secondBody->ID,attr_index,0);
//        emit showAttribue(element,secondBody->bcuID,secondBody->rcuID,secondBody->ID);
//        return;
//    }


//    ElementType lelement = firstBody->element;


//    int type_value = (lelement==ELEMENT_BFU&&firstBody->rcuID==secondBody->rcuID&&firstWireDot%2==0)?(1):
//                     (lelement==ELEMENT_BFU&&firstBody->rcuID==secondBody->rcuID&&firstWireDot%2==1)?(2):
//                     (lelement==ELEMENT_BFU&&firstBody->rcuID==secondBody->rcuID-1&&firstWireDot%2==0)?(5):
//                 (lelement==ELEMENT_BFU&&firstBody->rcuID==secondBody->rcuID-1&&firstWireDot%2==1)?(6):
//                 (lelement==ELEMENT_SBOX&&firstBody->rcuID==secondBody->rcuID)?(3):
//                 (lelement==ELEMENT_SBOX&&firstBody->rcuID==secondBody->rcuID-1)?(7):
//                 (lelement==ELEMENT_BENES&&firstBody->rcuID==secondBody->rcuID)?(4):
//                 (lelement==ELEMENT_BENES&&firstBody->rcuID==secondBody->rcuID-1)?(8):
//                 (lelement==ELEMENT_INFIFO)?9:10;
//    int index_value = 0;

//    m_xml->setElementAttribute(secondBody->bcuID,secondBody->rcuID,secondBody->element,
//                             secondBody->ID,attr_type,type_value);
//    m_xml->setElementAttribute(secondBody->bcuID,secondBody->rcuID,secondBody->element,
//                             secondBody->ID,attr_index,index_value);

//    emit showAttribue(element,secondBody->bcuID,secondBody->rcuID,secondBody->ID);
//}

/**
 * GuiViewer::recordCurrentTouchedWire
 * \brief   
 * \param   temp_wire
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::recordCurrentTouchedWire(int temp_wire)
{
    Q_UNUSED(temp_wire)

}

/**
 * GuiViewer::releseAll
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::releseAll()
{
    touchedBodyIndexList.clear();
    updateShowWireIndexList();
    touchedBodyIndex = -1;
    touchedWireIndex = -1;
    touchPointIndex = -1;
    status = NORMAL;
}

/**
 * GuiViewer::recordCurrentTouchedBody
 * \brief   
 * \param   temp_body
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::recordCurrentTouchedBody(int temp_body)
{
    //显示数据
    currentBcuId = moduleBodyList.at(temp_body)->bcuID;
    currentRcuId = moduleBodyList.at(temp_body)->rcuID;
    currentElementType = moduleBodyList.at(temp_body)->element;
    currentId = moduleBodyList.at(temp_body)->ID;
    showAttribue(currentElementType,currentBcuId,currentRcuId,currentId);
}

/**
 * GuiViewer::removeWire
 * \brief   
 * \param   wire
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::removeWire(ModuleWire *wire)
{
    if(wire == NULL)return;
    setInputAttribute(NULL,0,wire->output,wire->outputPosIndex);

    if(wire->input)wire->input->removeOutputWire(wire);
    if(wire->output)wire->output->removeInputWire(wire,wire->outputPosIndex);
    moduleWireList.removeOne(wire);
    delete wire;

    m_isModified = true; //modified by zhangjun at 2016/8/27
    m_scrollArea->setWindowModified(m_isModified);
}

/**
 * GuiViewer::keyPressEvent
 * \brief   
 * \param   e
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::keyPressEvent(QKeyEvent *e)
{
    if(e->modifiers() == Qt::CTRL)
    {
        if(e->key() == Qt::Key_S)
        {
            m_xml->save();
            emit setStatus(trUtf8("保存成功"));
        }
        else if(e->key() >= Qt::Key_1 && e->key() <= Qt::Key_3)
        {
            ElementType element = (ElementType)(e->key()-Qt::Key_1+ELEMENT_BFU);
            addElement(element);

            QMouseEvent *e = new QMouseEvent(QMouseEvent::MouseButtonPress,
                                             currentPos,
                                             Qt::LeftButton,Qt::NoButton,Qt::NoModifier);
            mousePressEvent(e);
            press = false;
            delete e;
        }
        else if(e->key() == Qt::Key_W)
        {
            addElement(ELEMENT_WIRE);
        }
        else if(e->key() == Qt::Key_A)  //select all
        {
            if(touchedBodyIndexList.count() != moduleBodyList.count()-2)//不包含mem和infifo
            {
                touchedBodyIndexList.clear();
                for(int i = 2; i<moduleBodyList.count(); i++)
                {
                    touchedBodyIndexList << i;
                }
                updateShowWireIndexList();
                status = CHOOSEBODY;
                touchedBodyIndex = touchedBodyIndexList.last();
            }
            else
            {
                touchedBodyIndexList.clear();
                updateShowWireIndexList();
            }
            update();
        }
        //        else if(e->key() == Qt::Key_L)
        //        {
        //            setShowAllWire(!showAllWire);
        //        }
        else e->ignore();
        return;
    }
    switch(status)
    {
    case CHOOSEWIRE:
    {
        if(e->key() == Qt::Key_Delete && touchedWireIndex >= 0)
        {
            ModuleWire *wire = moduleWireList.at(touchedWireIndex);
            //属性
            removeWire(wire);
            updateShowWireIndexList();
            touchedWireIndex = -1;
            changeStatus(NORMAL);
            update();
        }
        break;
    }
    case CHOOSEBODY:
    {
        if(e->key() == Qt::Key_Delete && touchedBodyIndexList.count() > 0)
        {
            QList<ModuleBody*> toRemoveBodyList;
            for(int t = 0; t<touchedBodyIndexList.count(); t++)
            {
                ModuleBody *body = moduleBodyList.at(touchedBodyIndexList.at(t));
                for(int i = 0; i<body->inputCount; i++)
                {
                    removeWire(body->inputPosMap.values().at(i));
                }
                for(int i = 0; i<body->outputCount; i++)
                {
                    for(int j = 0; j<body->outputPosMap.values().at(i)->count(); j++)
                    {
                        removeWire(body->outputPosMap.values().at(i)->at(j));
                    }
                }
                toRemoveBodyList << body;
                lastBodyMap.insert(body->element,NULL);
            }
            for(int i = 0; i<toRemoveBodyList.count(); i++)
            {
                removeBody(toRemoveBodyList.at(i));
            }
            touchedBodyIndexList.clear();
            updateShowWireIndexList();
            touchedBodyIndex = -1;
            //属性
            changeStatus(NORMAL);
            update();
        }
        break;
    }
    default:break;
    }
    //    m_xml->save();  //modified by zhangjun
}

/**
 * GuiViewer::mouseDoubleClickEvent
 * \brief   
 * \param   e
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    switch(status)
    {
    case CHOOSEBODY:
    {
        //选择了实体
        if(touchedBodyIndex >= 0)
        {
            status = NORMAL;
            bodyIdDialog *dialog = new bodyIdDialog(moduleBodyList.at(touchedBodyIndex));
            connect(dialog,SIGNAL(changeID(ModuleBody*,int,int,int)),this,SLOT(changeID(ModuleBody*,int,int,int)));
            dialog->exec();
            delete dialog;  //modified by zhangjun at 2016/9/7
        }
        break;
    }
    default:
        break;
    }
}

/**
 * GuiViewer::mousePressEvent
 * \brief   
 * \param   e
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::mousePressEvent(QMouseEvent *e)
{
    press = true;
    currentPos = e->pos();
    qDebug() << "status:" << status;
    switch(status)
    {
    case NORMAL:
    {
        if(e->button() == Qt::RightButton)  //如果点击右键则全部取消选择
        {
            touchedBodyIndexList.clear();
            updateShowWireIndexList();
            update();
            break;
        }
        touchedBodyIndexList.clear();
        touchedWireIndex = -1;
        touchedBodyIndex = -1;

        //先判断显示的线
        if(!showAllWire)
        {
            for(int i = 0; i<showWireIndexList.count(); i++)
            {
                if(showWireIndexList.at(i) >= moduleWireList.count())continue;
                if(moduleWireList.at(showWireIndexList.at(i))->isEnter(e->pos()/rate))
                {
                    touchedWireIndex = showWireIndexList.at(i);
                }
            }
        }
        else //再判断所有线
        {

            for(int i = 0; i<moduleWireList.count(); i++)
            {
                if(moduleWireList.at(i)->isEnter(e->pos()/rate))
                {
                    touchedWireIndex = i;
                }
            }
        }
        if(touchedWireIndex >= 0)  //选中线
        {
            changeStatus(CHOOSEWIRE);
            ModuleWire *wire = moduleWireList.at(touchedWireIndex);
            ModuleBody *outBody = wire->output;
            if(outBody)
            {
                touchedBodyIndexList << moduleBodyList.indexOf(outBody);
                showAttribue(outBody->element,outBody->bcuID,outBody->rcuID,outBody->ID);
            }
            ModuleBody *inBody = wire->input;
            if(inBody)
            {
                touchedBodyIndexList << moduleBodyList.indexOf(inBody);
            }
            updateShowWireIndexList();
        }
        else    //什么都没选中
        {
            //判断进入了哪个实体
            for(int i = 0; i<moduleBodyList.count(); i++)
            {
                if(moduleBodyList.at(i)->isEnter(e->pos()/rate))
                {
                    touchedBodyIndex = i;
                }
            }
            //选择了实体
            if(touchedBodyIndex >= 0)
            {
                //存储
                touchedBodyIndexList.clear();
                touchedBodyIndexList << touchedBodyIndex;
                updateShowWireIndexList();
                recordCurrentTouchedBody(touchedBodyIndex);
                spos = e->pos()/rate;
                changeStatus(CHOOSEBODY);
            }
        }
        break;
    }
    case CHOOSEBODY:
    {
        if(e->button() == Qt::RightButton)  //如果点击右键则全部取消选择
        {
            touchedBodyIndexList.clear();
            updateShowWireIndexList();
            changeStatus(NORMAL);
            update();
            break;
        }
        //判断进入了哪个实体
        touchedBodyIndex = -1;
        for(int i = 0; i<moduleBodyList.count(); i++)
        {
            if(moduleBodyList.at(i)->isEnter(e->pos()/rate))
            {
                touchedBodyIndex = i;
            }
        }
        if(touchedBodyIndex >= 0)
        {
            //是否按下了ctrl， 按下ctrl则往后添加没有则清理添加
            if(e->modifiers() == Qt::CTRL)
            {
                if(touchedBodyIndexList.contains(touchedBodyIndex))
                {
                    touchedBodyIndexList.removeOne(touchedBodyIndex);
                    updateShowWireIndexList();
                    touchedBodyIndex = -1;
                }
                else
                {
                    touchedBodyIndexList << touchedBodyIndex;
                    updateShowWireIndexList();
                    recordCurrentTouchedBody(touchedBodyIndex);
                    spos = e->pos()/rate;
                }
            }
            else if(e->modifiers() == Qt::ALT)
            {
                spos = e->pos()/rate;
            }
            else
            {
                touchedBodyIndexList.clear();
                touchedBodyIndexList << touchedBodyIndex;
                updateShowWireIndexList();
                recordCurrentTouchedBody(touchedBodyIndex);
                spos = e->pos()/rate;
            }
        }
        else
        {
            if(e->modifiers() != Qt::CTRL)
            {
                touchedBodyIndexList.clear();
                changeStatus(NORMAL);
                mousePressEvent(e);
            }
        }
        update();
        break;
    }
    case CHOOSEWIRE:
    {
        if(e->button() == Qt::RightButton || touchedWireIndex < 0)
        {
            touchedWireIndex = -1;
            touchPointIndex = -1;
            changeStatus(NORMAL);
            update();
            break;
        }
        //判断是否点击到
        touchPointIndex = moduleWireList.at(touchedWireIndex)->isEnterVertex(e->pos()/rate);
        if(touchPointIndex == 0 || touchPointIndex == 3)    //线头
        {
            ModuleWire *wire = moduleWireList.at(touchedWireIndex);
            firstBody = wire->input;
            firstWireDot = wire->inputPosIndex;
            secondBody = wire->output;
            secondWireDot = wire->outputPosIndex;
            changeStatus(PRECHANGEWIRETOP);
            spos = e->pos()/rate;
        }
        else if(touchPointIndex == 1 || touchPointIndex == 2)   //线高
        {
            changeStatus(PRECHANGEWIREHEIGHT);
            spos = e->pos()/rate;
        }
        else
        {
            changeStatus(NORMAL);
            mousePressEvent(e);
        }
        break;
    }
    case ADDWIRE1:
    {
        //判断的函数中会给需要的变量赋值
        if(e->button() == Qt::LeftButton)
        {
            if(pressFirstWireDot(e->pos()/rate))  //modified by zhangjun at 2016/8/28
                changeStatus(ADDWIRE2);
        }
        else
        {
            changeStatus(NORMAL);
            emit addElementFinished();
        }

        break;
    }
    case ADDWIRE2:
    {
        if(e->button() == Qt::LeftButton && pressSecondWireDot(e->pos()/rate))
        {

            //添加线
            ModuleWire *wire = new ModuleWire(QPointF(firstBody->outputPosMap.keys().at(firstWireDot)->x(),
                                                      firstBody->outputPosMap.keys().at(firstWireDot)->y()),
                                              QPointF(secondBody->inputPosMap.keys().at(secondWireDot)->x(),
                                                      secondBody->inputPosMap.keys().at(secondWireDot)->y()));
            //如果原位置有input线的话删除
            ModuleWire *temp = secondBody->inputPosMap.values().at(secondWireDot);
            if(temp)
            {
                temp->input->removeOutputWire(temp,temp->inputPosIndex);
                temp->output->removeInputWire(temp,temp->outputPosIndex);
                moduleWireList.removeOne(temp);
                delete temp;
            }
            addWire(firstBody,firstWireDot,secondBody,secondWireDot,wire);
            touchedBodyIndex = moduleBodyList.indexOf(secondBody);
            touchedBodyIndexList << touchedBodyIndex;
            updateShowWireIndexList();
            //改变属性
            emit setInputAttribute(firstBody,firstWireDot,secondBody,secondWireDot);
            //状态转换
            changeStatus(ADDWIREFINISH);

            //            m_xml->save();
        }
        else
        {
            changeStatus(NORMAL);
        }
        if(tempWire)
        {
            delete tempWire;
            tempWire = NULL;
        }
        this->addElement(ELEMENT_WIRE);
        break;
    }
    case ADDBODY:
    {
        if(e->button() == Qt::RightButton)
        {
            status = NORMAL;
            emit addElementFinished();
            break;
        }
        //如果左键点击到合法位置
        ModuleBody *ele = new ModuleBody(e->pos()/rate,preAddElement,preAddBcu,preAddRcu,preAddId);
        addBody(preAddBcu,preAddRcu,preAddId,ele);
        m_xml->setElementAttribute(preAddBcu,preAddRcu,preAddElement,preAddId,PosX,e->pos().x()/rate);
        m_xml->setElementAttribute(preAddBcu,preAddRcu,preAddElement,preAddId,PosY,e->pos().y()/rate);
        update();
        changeStatus(ADDBODYFINISH);
        changeStatus(NORMAL);
        this->addElement(preAddElement);  //modified by zhangjun at 2016/8/28
        //        m_xml->save();
        break;
    }
    case DRAGBODY:
    {
        break;
    }
    default:break;
    }
    update();
}

/**
 * GuiViewer::mouseMoveEvent
 * \brief   
 * \param   e
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::mouseMoveEvent(QMouseEvent *e)
{
    currentPos = e->pos();
    switch (status)
    {
    case ADDWIRE2:
    {
        //        changeStatus(NORMAL);
        if(!tempWire)tempWire = new ModuleWire(firstPos,e->pos()/rate);
        else tempWire->setPoint(3,e->pos()/rate);


        break;
    }
    case CHOOSEBODY:
    {
        if(!press || e->modifiers()==Qt::CTRL)break;
        ElementType element = moduleBodyList.at(touchedBodyIndex)->element;
        if(element >= ELEMENT_INFIFO && element <= ELEMENT_OUTFIFO)break;
        //移动所以被选择的实体
        for(int i = 0; i<touchedBodyIndexList.count(); i++)
        {
            ModuleBody *ele = moduleBodyList.at(touchedBodyIndexList.at(i));
            ele->movePos(e->pos()/rate-spos);
            m_xml->setElementAttribute(ele->bcuID,ele->rcuID,ele->element,ele->ID,PosX,ele->rect.x());
            m_xml->setElementAttribute(ele->bcuID,ele->rcuID,ele->element,ele->ID,PosY,ele->rect.y());
        }
        if(touchedBodyIndex >= 0)recordCurrentTouchedBody(touchedBodyIndex);
        else if(!touchedBodyIndexList.isEmpty())recordCurrentTouchedBody(touchedBodyIndexList.last());
        spos = e->pos()/rate;

        m_isModified = true; //modified by zhangjun at 2016/9/5
        m_scrollArea->setWindowModified(m_isModified);
        break;
    }
    case PRECHANGEWIREHEIGHT:
    {
        if(touchedWireIndex < 0)break;
        ModuleWire *wire = moduleWireList.at(touchedWireIndex);
        ModuleBody *dest = wire->output;
        int height = wire->changeHeight(e->pos()/rate-spos);
        spos = e->pos()/rate;
        //存储
        AttributeID heightID;
        if(dest->element==ELEMENT_BFU)heightID=(AttributeID)(BfuInputA_Height+wire->outputPosIndex);
        else if(dest->element==ELEMENT_SBOX)heightID=SboxInput_Height;
        else if(dest->element==ELEMENT_BENES)heightID=(AttributeID)(BenesInput0_Height+wire->outputPosIndex);
        else break;
        m_xml->setElementAttribute(dest->bcuID,dest->rcuID,dest->element,dest->ID,heightID,height);

        m_isModified = true; //modified by zhangjun at 2016/9/5
        m_scrollArea->setWindowModified(m_isModified);
        break;
    }
    case PRECHANGEWIRETOP:
    {
        if(touchedWireIndex < 0)break;

        changeStatus(CHANGEWIRE);
        break;
    }
    case CHANGEWIRE:
    {
        ModuleWire *wire = moduleWireList.at(touchedWireIndex);
        if(!tempWire)
        {
            if(touchPointIndex == 0)tempWire = new ModuleWire(e->pos()/rate,wire->point[3]);
            else if(touchPointIndex == 3)tempWire = new ModuleWire(wire->point[0],e->pos()/rate);
        }
        else
        {
            if(touchPointIndex == 0)tempWire->setPoint(0,e->pos()/rate);
            else if(touchPointIndex == 3)tempWire->setPoint(3,e->pos()/rate);
        }

        //        m_isModified = true; //modified by zhangjun at 2016/9/5
        //        m_scrollArea->setWindowModified(m_isModified);
        break;
    }
    default:
        break;
    }

    update();
}

/**
 * GuiViewer::mouseReleaseEvent
 * \brief   
 * \param   e
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::mouseReleaseEvent(QMouseEvent *e)
{
    press = false;
    switch(status)
    {
    case ADDWIREFINISH:
    case ADDBODYFINISH:
    {
        changeStatus(NORMAL);
        break;
    }
    case PRECHANGEWIREHEIGHT:
    case CHANGEWIREHEIGHT:
    {
        changeStatus(CHOOSEWIRE);
        break;
    }
    case PRECHANGEWIRETOP:
    {
        if(touchedWireIndex >= 0)
        {
            changeStatus(CHOOSEWIRE);
        }
        break;
    }
    case CHANGEWIRE:
    {
        ModuleWire *wire_selected = moduleWireList.at(touchedWireIndex);
        if(touchPointIndex == 0)
        {
            secondBody = wire_selected->output;
            secondWireDot = wire_selected->outputPosIndex;
            if(pressFirstWireDot(e->pos()/rate,false))
            {
                ModuleBody *body_old =  wire_selected->input;
                ModuleBody *body_new = firstBody;
                if(body_new == body_old && wire_selected->inputPosIndex == firstWireDot)
                {
                    changeStatus(CHOOSEWIRE);
                    if(tempWire)
                    {
                        delete tempWire;
                        tempWire = NULL;
                    }
                    wire_selected->updateHeight();
                    update();
                    break;
                }
                //改变属性
                setInputAttribute(firstBody,firstWireDot,secondBody,secondWireDot);

                body_old->removeOutputWire(wire_selected);
                body_new->addOutput(firstWireDot,wire_selected);
                wire_selected->setInput(body_new,firstWireDot);
                wire_selected->updateHeight();
            }
        }
        else if(touchPointIndex == 3)
        {
            firstBody = wire_selected->input;
            firstWireDot = wire_selected->inputPosIndex;
            if(pressSecondWireDot(e->pos()/rate))
            {
                ModuleBody *body_old =  wire_selected->output;
                ModuleBody *body_new = secondBody;
                if(body_new == body_old && wire_selected->outputPosIndex == secondWireDot)
                {
                    changeStatus(CHOOSEWIRE);
                    if(tempWire)
                    {
                        delete tempWire;
                        tempWire = NULL;
                    }
                    wire_selected->updateHeight();
                    update();
                    break;
                }
                //改变属性
                setInputAttribute(NULL,0,wire_selected->output,wire_selected->outputPosIndex);
                setInputAttribute(wire_selected->input,wire_selected->inputPosIndex,secondBody,secondWireDot);
                //改变界面
                //先踢掉之前的那根线,删除
                ModuleWire *temp = body_new->inputPosMap.values().at(secondWireDot);
                if(temp)
                {
                    temp->input->removeOutputWire(temp,temp->inputPosIndex);
                    body_new->removeInputWire(temp,secondWireDot);
                    moduleWireList.removeOne(temp);
                    delete temp;
                }

                //
                body_old->removeInputWire(wire_selected,wire_selected->outputPosIndex);
                body_new->setInput(secondWireDot,wire_selected);
                wire_selected->setOutput(body_new,secondWireDot);
                wire_selected->updateHeight();
            }


        }
        changeStatus(CHOOSEWIRE);
        if(tempWire)
        {
            delete tempWire;
            tempWire = NULL;
        }

        m_isModified = true; //modified by zhangjun at 2016/9/5
        m_scrollArea->setWindowModified(m_isModified);
        break;
    }
    default:break;
    }
    //    m_xml->save();
    update();
}

/**
 * GuiViewer::wheelEvent
 * \brief   
 * \param   e
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::wheelEvent(QWheelEvent *e)
{
    currentPos = e->pos();
    if(e->modifiers() == Qt::CTRL)
    {
        if(e->delta() > 0)
        {
            rate *= ZOOMRATE;
            setGeometry(geometry().x(),geometry().y(),ORIGINSIZEEDGE*rate,ORIGINSIZEEDGE*rate);
        }
        else
        {
            rate /= ZOOMRATE;
            setGeometry(geometry().x(),geometry().y(),ORIGINSIZEEDGE*rate,ORIGINSIZEEDGE*rate);
        }
        setStatus(trUtf8("比例：")+QString::number(rate*100)+"%");
        resizeSlot();
    }
    else
    {
        QApplication::sendEvent((QScrollArea*)parent(), e);
    }
    this->update();
}

/**
 * GuiViewer::paintEvent
 * \brief   
 * \param   e
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
    QPainter painter(this);
    QFont f;
    f.setPixelSize(ORIGINFONTSIZE*rate);
    painter.setFont(f);
    for(int i = 0; i<moduleBodyList.count(); i++)
    {
        ModuleBody *body = moduleBodyList.at(i);
        if(body == NULL)continue;
        //如果超出范围就不画
        if(abs(body->rect.y()*rate-m_scrollArea->verticalScrollBar()->value()) > m_scrollArea->height() ||
                abs(body->rect.x()*rate-m_scrollArea->horizontalScrollBar()->value()) > m_scrollArea->width())continue;

        painter.setBrush(body->getBrush());

        painter.drawRect(body->rect.x()*rate,body->rect.y()*rate,body->rect.width()*rate,body->rect.height()*rate);
        painter.setBrush(QBrush(QColor(151, 158, 255)));
        for(int j = 0; j<body->inputCount; j++) //input点
        {
            painter.drawRect(body->inputPosMap.keys().at(j)->x()*rate,
                             body->inputPosMap.keys().at(j)->y()*rate,
                             body->inputPosMap.keys().at(j)->width()*rate,
                             body->inputPosMap.keys().at(j)->height()*rate);
        }
        for(int j = 0; j<body->outputCount; j++)
        {
            painter.drawRect(body->outputPosMap.keys().at(j)->x()*rate,
                             body->outputPosMap.keys().at(j)->y()*rate,
                             body->outputPosMap.keys().at(j)->width()*rate,
                             body->outputPosMap.keys().at(j)->height()*rate);
        }
        for(int j = 0; j<body->labels.count(); j++)
        {
            painter.drawText(body->labels.values().at(j)*rate,
                             body->labels.keys().at(j));
        }
    }
    for(int i = 0; i<touchedBodyIndexList.count(); i++)
    {
        if(touchedBodyIndexList.at(i) >= moduleBodyList.count())
        {
            touchedBodyIndexList.clear();
            break;
        }
        for(int j = 0; j<8; j++)
        {
            painter.drawRect(moduleBodyList.at(touchedBodyIndexList.at(i))->touchPoses[j].x()*rate,
                             moduleBodyList.at(touchedBodyIndexList.at(i))->touchPoses[j].y()*rate,
                             moduleBodyList.at(touchedBodyIndexList.at(i))->touchPoses[j].width()*rate,
                             moduleBodyList.at(touchedBodyIndexList.at(i))->touchPoses[j].height()*rate);
        }
    }
    //wire
#define ARROWEDGE 5
    if(showAllWire) //如果显示所有的连线就显示所有连线
    {
        for(int i = 0; i<moduleWireList.count(); i++)
        {
            ModuleWire *wire = moduleWireList.at(i);
            //强调
            painter.setBrush(QBrush(Qt::blue));
            painter.drawRect(wire->touchPoses[0].x()*rate,
                    wire->touchPoses[0].y()*rate,
                    wire->touchPoses[0].width()*rate,
                    wire->touchPoses[0].height()*rate);
            painter.setBrush(QBrush(Qt::green));
            painter.drawRect(wire->touchPoses[3].x()*rate,
                    wire->touchPoses[3].y()*rate,
                    wire->touchPoses[3].width()*rate,
                    wire->touchPoses[3].height()*rate);
            //如果线太长就不画了
            if((wire->input == moduleInfifo || wire->input == moduleInMemery || wire->output == moduleOutMemery || wire->output == moduleOutfifo) &&
                    abs(wire->point[0].y()-wire->point[3].y())*rate > m_scrollArea->height())continue;
            painter.setBrush(wire->getBrush());
            for(int j = 0; j<3; j++)
            {
                painter.drawLine(wire->point[j]*rate,wire->point[j+1]*rate);
            }
            painter.drawLine(wire->point[3]*rate,wire->point[3]*rate+QPointF(-ARROWEDGE*rate,-ARROWEDGE*rate));
            painter.drawLine(wire->point[3]*rate,wire->point[3]*rate+QPointF(ARROWEDGE*rate,-ARROWEDGE*rate));
        }
    }
    else //否则显示
    {
        for(int i = 0; i<showWireIndexList.count(); i++)
        {
            if(showWireIndexList.at(i) >= moduleWireList.count())
            {
                changeStatus(NORMAL);
                continue;
            }
            ModuleWire *wire = moduleWireList.at(showWireIndexList.at(i));
            //强调
            painter.setBrush(QBrush(Qt::blue));
            painter.drawRect(wire->touchPoses[0].x()*rate,
                    wire->touchPoses[0].y()*rate,
                    wire->touchPoses[0].width()*rate,
                    wire->touchPoses[0].height()*rate);
            painter.setBrush(QBrush(Qt::green));
            painter.drawRect(wire->touchPoses[3].x()*rate,
                    wire->touchPoses[3].y()*rate,
                    wire->touchPoses[3].width()*rate,
                    wire->touchPoses[3].height()*rate);
            //如果线太长就不画了
            if((wire->input == moduleInfifo || wire->input == moduleInMemery || wire->output == moduleOutMemery || wire->output == moduleOutfifo) &&
                    abs(wire->point[0].y()-wire->point[3].y())*rate > m_scrollArea->height())continue;
            painter.setBrush(wire->getBrush());
            for(int j = 0; j<3; j++)
            {
                painter.drawLine(wire->point[j]*rate,wire->point[j+1]*rate);
            }
            painter.drawLine(wire->point[3]*rate,wire->point[3]*rate+QPointF(-ARROWEDGE*rate,-ARROWEDGE*rate));
            painter.drawLine(wire->point[3]*rate,wire->point[3]*rate+QPointF(ARROWEDGE*rate,-ARROWEDGE*rate));
        }
    }
    if(touchedWireIndex >= 0)
    {
        if(touchedWireIndex >= moduleWireList.count())
        {
            touchedWireIndex = -1;
        }
        else
        {
            painter.setBrush(moduleWireList.at(touchedWireIndex)->getTouchBrush());
            for(int j = 0; j<4; j++)
            {
                painter.drawRect(moduleWireList.at(touchedWireIndex)->touchPoses[j].x()*rate,
                                 moduleWireList.at(touchedWireIndex)->touchPoses[j].y()*rate,
                                 moduleWireList.at(touchedWireIndex)->touchPoses[j].width()*rate,
                                 moduleWireList.at(touchedWireIndex)->touchPoses[j].height()*rate);
            }
        }
    }

    //临时动态线
    if(tempWire)
    {
        QPen pen;
        pen.setStyle(Qt::SolidLine);
        pen.setColor(Qt::green);
        painter.setPen(pen);
        for(int j = 0; j<3; j++)
        {
            painter.drawLine(tempWire->point[j]*rate,tempWire->point[j+1]*rate);
        }
        painter.drawLine(tempWire->point[3]*rate,tempWire->point[3]*rate+QPointF(-ARROWEDGE*rate,-ARROWEDGE*rate));
        painter.drawLine(tempWire->point[3]*rate,tempWire->point[3]*rate+QPointF(ARROWEDGE*rate,-ARROWEDGE*rate));
    }

}
#define Enum2Str_begin \
    QString enum2str(Status s)\
{\
    switch(s)\
{
#define Enum2str_value(v) \
    case v:return #v;
#define Enum2str_end \
    default: return "0";\
    }\
    }

Enum2Str_begin
Enum2str_value(NORMAL)
Enum2str_value(ADDWIRE1)
Enum2str_value(ADDWIRE2)
Enum2str_value(ADDWIREFINISH)
Enum2str_value(ADDBODY)
Enum2str_value(ADDBODYFINISH)
Enum2str_value(DRAGBODY)

Enum2str_value(CHOOSEWIRE)
Enum2str_value(PRECHANGEWIREHEIGHT)
Enum2str_value(CHANGEWIREHEIGHT)
Enum2str_value(PRECHANGEWIRETOP)
Enum2str_value(CHOOSEBODY)
Enum2str_value(CHANGEWIRE)
Enum2str_value(CHANGEWIREFAIL)
Enum2str_value(CHANGEWIREFINISH)
Enum2str_value(BODYMENU)
Enum2str_value(WIREMENU)
Enum2str_end


/**
 * GuiViewer::changeStatus
 * \brief   
 * \param   s
 * \author  zhangjun
 * \date    2016-10-12
 */
void GuiViewer::changeStatus(Status s)
{
    this->status = s;
    setStatus(trUtf8("状态:")+enum2str(s));
}

/**
 * GuiViewer::getAction
 * \brief   
 * \return  QAction *
 * \author  zhangjun
 * \date    2016-10-12
 */
QAction *GuiViewer::getAction()
{
    return m_action;
}

/**
 * GuiViewer::getCurFile
 * \brief   
 * \return  QString
 * \author  zhangjun
 * \date    2016-10-12
 */
QString GuiViewer::getCurFile()
{
    return m_curFile;
}
