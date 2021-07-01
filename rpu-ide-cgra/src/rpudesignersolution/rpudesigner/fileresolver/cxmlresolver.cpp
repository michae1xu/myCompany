/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    xmlresolver.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include <QRectF>
#include <QPointF>
#include <QFile>
#include <QDomDocument>
#include <QTextStream>
#include <QCryptographicHash>
#include <code/CLfsrElement.h>
#include "common/debug.h"
#include "cxmlresolver.h"
#include "code/cbaseelement.h"
#include "code/cbcuelement.h"
#include "code/crcuelement.h"
#include "code/cbfuelement.h"
#include "code/csboxelement.h"
#include "code/cbeneselement.h"
#include "code/cmemoryelement.h"
#include "code/csregelement.h"
#include "code/coutfifoelement.h"
//#include "basepe.h"
//#include "CAppEnv.h"

const QString CURRENT_UI_VER ("3.0.0");

/**
 * CXmlResolver::CXmlResolver
 * \brief   constructor of CXmlResolver
 * \param   filename
 * \author  zhangjun
 * \date    2016-10-12
 */
CXmlResolver::CXmlResolver(const QString &filename):
      m_curFile(filename)
{
    initNeededMap();
    m_file = new QFile(m_curFile);
    if(!m_file->open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << m_file->errorString();
    }
    m_document = new QDomDocument();
    m_document->setContent(m_file);

    m_root = m_document->documentElement();
    if(m_root.isNull())
    {
        QDomElement element = m_document->createElement(TAGALGRITHM);
        element.setAttribute("name","");
        element.setAttribute("md5", "");
        m_document->appendChild(element);
        m_root = element;
    }
    else if(m_root.tagName() != TAGALGRITHM)
    {
    }

    QDomNode node = m_document->childNodes().at(0);
    if(node.toProcessingInstruction().isNull())
    {
        QDomProcessingInstruction instruction;
        instruction = m_document->createProcessingInstruction("xml", "version=\'1.0\' encoding=\'UTF-8\'");
        m_document->insertBefore(instruction, m_root);
    }
    else
    {
        /*QDomProcessingInstruction instruction = */node.toProcessingInstruction();
    }

    m_rca = m_root.firstChildElement("RCA");
    if(m_rca.isNull())
    {
        QDomElement element = m_document->createElement("RCA");
        element.setAttribute("id",0);
        m_root.appendChild(element);
        m_rca = element;
    }
    m_rsm = m_rca.nextSiblingElement("RSM");
    if(m_rsm.isNull())
    {
        QDomElement element = m_document->createElement("RSM");
        element.setAttribute("id",0);
        m_root.appendChild(element);
        m_rsm = element;
    }

    m_uiVer = m_rsm.nextSiblingElement("UIVER");
    isAutoResetPos = false;
    if(m_uiVer.isNull())
    {
        QDomElement element = m_document->createElement("UIVER");
        element.setAttribute("ver",CURRENT_UI_VER);
        isAutoResetPos = true;
        m_root.appendChild(element);
        m_uiVer = element;
    }
    else
    {
        if(m_uiVer.attribute("ver") != CURRENT_UI_VER)
        {
            isAutoResetPos = true;
        }
    }


    //命令行参数
    //    m_paraInfo = new SParaInfo;
    //    m_cmdWordInfo = new SCMDWordInfo;
    //    m_xmlParameter = new SXmlParameter;

    //    m_para = m_rca.nextSiblingElement("PARA");
    //    if(m_para.isNull())
    //    {
    //        QDomElement element = m_document->createElement("PARA");
    //        element.setAttribute("value", m_curFile);
    //        element.setAttribute("name", "");
    //        m_root.appendChild(element);
    //        m_para = element;
    //    }

    //    initParaInfo();
}

/**
 * CXmlResolver::~CXmlResolver
 * \brief   destructor of CXmlResolver
 * \author  zhangjun
 * \date    2016-10-12
 */
CXmlResolver::~CXmlResolver()
{
    if(m_file && m_file->isOpen())
    {
        m_file->close();
        //        m_file->deleteLater();
        delete m_file;
    }
    delete m_document;
    //    delete m_paraInfo;
    //    delete m_xmlParameter;
    //    delete m_cmdWordInfo;
    //    emit closeFinished(m_curFile);
    //    DBG << "~CXmlResolver()";
}

QString CXmlResolver::getCurFile() const
{
    return m_curFile;
}

///**
// * CXmlResolver::findCycleElement
// * \brief
// * \return  QDomElement
// * \author  zhangjun
// * \date    2016-10-12
// */
//QDomElement CXmlResolver::findCycleElement()
//{
//    QDomElement cycle = m_para.firstChild().toElement();
//    if(cycle.tagName() == "CYCLE")
//        return cycle;

//    //没找到 新建
//    QDomElement element = m_document->createElement("CYCLE");
//    element.setAttribute("value", QString::number(0));
//    m_para.appendChild(element);
//    return element;
//}

///**
// * CXmlResolver::findIntervalElement
// * \brief
// * \return  QDomElement
// * \author  zhangjun
// * \date    2016-10-12
// */
//QDomElement CXmlResolver::findIntervalElement()
//{
//    QDomNodeList list = m_para.childNodes();
//    for(int i = 0; i<list.count(); i++)
//    {
//        QDomElement interval = list.at(i).toElement();
//        if(interval.tagName() == "INTERVAL")
//            return interval;
//    }


//    //没找到 新建
//    QDomElement element = m_document->createElement("INTERVAL");
//    element.setAttribute("value", QString::number(0));
//    m_para.appendChild(element);
//    return element;
//}

///**
// * CXmlResolver::findFifoElement
// * \brief
// * \return  QDomElement
// * \author  zhangjun
// * \date    2016-10-12
// */
//QDomElement CXmlResolver::findFifoElement()
//{
//    QDomNodeList list = m_para.childNodes();
//    for(int i = 0; i<list.count(); i++)
//    {
//        QDomElement fifo = list.at(i).toElement();
//        if(fifo.tagName() == "FIFO")
//            return fifo;
//    }

//    //没找到 新建
//    QDomElement element = m_document->createElement("FIFO");
//    m_para.appendChild(element);
//    return element;
//}

///**
// * CXmlResolver::findFifoInput
// * \brief
// * \param   id
// * \return  QDomElement
// * \author  zhangjun
// * \date    2016-10-12
// */
//QDomElement CXmlResolver::findFifoInput(int id)
//{
//    QDomElement fifo = findFifoElement();
//    QDomNodeList list = fifo.childNodes();
//    for(int i = 0; i<list.count(); i++)
//    {
//        QDomElement input = list.at(i).toElement();
//        if(input.tagName() == "INPUT" && input.attribute("id").toInt() == id)
//            return input;
//    }
//    //没找到 新建
//    QDomElement element = m_document->createElement("INPUT");
//    element.setAttribute("id", QString::number(id));
//    element.setAttribute("value", "");
//    fifo.appendChild(element);
//    return element;
//}

////QDomElement CXmlResolver::findFifoOutput(int id)
////{
////    QDomElement fifo = findFifoElement();
////    QDomNodeList list = fifo.childNodes();
////    for(int i = 0; i<list.count(); i++)
////    {
////        QDomElement output = list.at(i).toElement();
////        if(output.tagName() == "OUTPUT" && output.attribute("id").toInt() == id)
////            return output;
////    }
////    //没找到 新建
////    QDomElement element = m_document->createElement("OUTPUT");
////    element.setAttribute("id", QString::number(id));
////    element.setAttribute("value", "");
////    fifo.appendChild(element);
////    return element;
////}

///**
// * CXmlResolver::findMemElement
// * \brief
// * \return  QDomElement
// * \author  zhangjun
// * \date    2016-10-12
// */
//QDomElement CXmlResolver::findMemElement()
//{
//    QDomNodeList list = m_para.childNodes();
//    for(int i = 0; i<list.count(); i++)
//    {
//        QDomElement mem = list.at(i).toElement();
//        if(mem.tagName() == "MEM")
//            return mem;
//    }

//    //没找到 新建
//    QDomElement element = m_document->createElement("MEM");
//    m_para.appendChild(element);
//    return element;
//}

///**
// * CXmlResolver::findMemInput
// * \brief
// * \param   id
// * \return  QDomElement
// * \author  zhangjun
// * \date    2016-10-12
// */
//QDomElement CXmlResolver::findMemInput(int id)
//{
//    QDomElement mem = findMemElement();
//    QDomNodeList list = mem.childNodes();
//    for(int i = 0; i<list.count(); i++)
//    {
//        QDomElement input = list.at(i).toElement();
//        if(input.tagName() == "INPUT" && input.attribute("id").toInt() == id)
//            return input;
//    }
//    //没找到 新建
//    QDomElement element = m_document->createElement("INPUT");
//    element.setAttribute("id", QString::number(id));
//    element.setAttribute("value", "");
//    mem.appendChild(element);
//    return element;
//}

///**
// * CXmlResolver::getCodeElement
// * \brief
// * \return  QDomElement
// * \author  zhangjun
// * \date    2016-10-12
// */
//QDomElement CXmlResolver::getCodeElement()
//{
//    QDomNodeList list = m_para.childNodes();
//    for(int i = 0; i<list.count(); i++)
//    {
//        QDomElement code = list.at(i).toElement();
//        if(code.tagName() == "CODE")
//            return code;
//    }


//    //没找到 新建
//    QDomElement element = m_document->createElement("CODE");
//    m_para.appendChild(element);
//    return element;
//}

///**
// * CXmlResolver::getCodeChildElement
// * \brief
// * \param   childName
// * \return  QDomElement
// * \author  zhangjun
// * \date    2016-10-12
// */
//QDomElement CXmlResolver::getCodeChildElement(QString childName)
//{
//    QDomElement code = getCodeElement();
//    QDomNodeList list = code.childNodes();
//    for(int i = 0; i<list.count(); i++)
//    {
//        QDomElement ele = list.at(i).toElement();
//        if(ele.tagName() == childName)
//            return ele;
//    }
//    //没找到 新建
//    QDomElement element = m_document->createElement(childName);
//    element.setAttribute("value", "0");
//    code.appendChild(element);
//    return element;
//}

///**
// * CXmlResolver::getCodeSourceElement
// * \brief
// * \return  QDomElement
// * \author  zhangjun
// * \date    2016-10-12
// */
//QDomElement CXmlResolver::getCodeSourceElement()
//{
//    QDomElement code = getCodeElement();
//    QDomNodeList list = code.childNodes();
//    for(int i = 0; i<list.count(); i++)
//    {
//        QDomElement ele = list.at(i).toElement();
//        if(ele.tagName() == "SOURCE")
//            return ele;
//    }
//    //没找到 新建
//    QDomElement element = m_document->createElement("SOURCE");
//    element.setAttribute("value", "MemoryPort0");
//    code.appendChild(element);
//    return element;
//}

///**
// * CXmlResolver::getCodeDestElement
// * \brief
// * \return  QDomElement
// * \author  zhangjun
// * \date    2016-10-12
// */
//QDomElement CXmlResolver::getCodeDestElement()
//{
//    QDomElement code = getCodeElement();
//    QDomNodeList list = code.childNodes();
//    for(int i = 0; i<list.count(); i++)
//    {
//        QDomElement ele = list.at(i).toElement();
//        if(ele.tagName() == "DEST")
//            return ele;
//    }
//    //没找到 新建
//    QDomElement element = m_document->createElement("DEST");
//    element.setAttribute("value", "MemoryPort0");
//    code.appendChild(element);
//    return element;
//}

//QDomElement CXmlResolver::findMemOutput(int id)
//{
//    QDomElement mem = findMemElement();
//    QDomNodeList list = mem.childNodes();
//    for(int i = 0; i<list.count(); i++)
//    {
//        QDomElement output = list.at(i).toElement();
//        if(output.tagName() == "OUTPUT" && output.attribute("id").toInt() == id)
//            return output;
//    }
//    //没找到 新建
//    QDomElement element = m_document->createElement("OUTPUT");
//    element.setAttribute("id", QString::number(id));
//    element.setAttribute("value", "");
//    mem.appendChild(element);
//    return element;

//}

///**
// * CXmlResolver::initParaInfo
// * \brief
// * \author  zhangjun
// * \date    2016-10-12
// */
//void CXmlResolver::initParaInfo()
//{
////    m_xmlParameter->xmlPath = m_curFile;
////    m_xmlParameter->inFifoPath0 = findFifoInput(0).attribute("value");
////    m_xmlParameter->inFifoPath1 = findFifoInput(1).attribute("value");
////    m_xmlParameter->inFifoPath2 = findFifoInput(2).attribute("value");
////    m_xmlParameter->inFifoPath3 = findFifoInput(3).attribute("value");
////    m_xmlParameter->inMemPath0 = findMemInput(0).attribute("value");
////    m_xmlParameter->inMemPath1 = findMemInput(1).attribute("value");
////    m_xmlParameter->inMemPath2 = findMemInput(2).attribute("value");
////    m_xmlParameter->inMemPath3 = findMemInput(3).attribute("value");
////    m_xmlParameter->inMemPath4 = findMemInput(4).attribute("value");
////    m_xmlParameter->selected = Qt::Unchecked;
////    m_xmlParameter->sort = -1;

//    m_paraInfo->xmlFNP = m_curFile;
//    m_paraInfo->cycle = findCycleElement().attribute("value").toInt();
//    m_paraInfo->triggerInterval = findIntervalElement().attribute("value").toInt();
//    m_paraInfo->inputFifoFNP0 = findFifoInput(0).attribute("value");
//    m_paraInfo->inputFifoFNP1 = findFifoInput(1).attribute("value");
//    m_paraInfo->inputFifoFNP2 = findFifoInput(2).attribute("value");
//    m_paraInfo->inputFifoFNP3 = findFifoInput(3).attribute("value");
////    m_paraInfo->outputFifoFNP0 = findFifoOutput(0).attribute("value");
////    m_paraInfo->outputFifoFNP1 = findFifoOutput(1).attribute("value");
////    m_paraInfo->outputFifoFNP2 = findFifoOutput(2).attribute("value");
////    m_paraInfo->outputFifoFNP3 = findFifoOutput(3).attribute("value");
//    m_paraInfo->inMemFNP0 = findMemInput(0).attribute("value");
//    m_paraInfo->inMemFNP1 = findMemInput(1).attribute("value");
//    m_paraInfo->inMemFNP2 = findMemInput(2).attribute("value");
//    m_paraInfo->inMemFNP3 = findMemInput(3).attribute("value");
//    m_paraInfo->inMemFNP4 = findMemInput(4).attribute("value");
////    m_paraInfo->outMemFNP0 = findMemOutput(0).attribute("value");
////    m_paraInfo->outMemFNP1 = findMemOutput(1).attribute("value");
////    m_paraInfo->outMemFNP2 = findMemOutput(2).attribute("value");
////    m_paraInfo->outMemFNP3 = findMemOutput(3).attribute("value");
////    m_paraInfo->outMemFNP4 = findMemOutput(4).attribute("value");
//    m_paraInfo->selected = Qt::Unchecked;
//    m_paraInfo->project = false;
//    m_paraInfo->sort = -1;

//    QString cycleout = CAppEnv::getCycleOutDirPath() + "/" + QFileInfo(m_curFile).baseName() + ".log";
//    m_paraInfo->logFNP = cycleout;
//    QString binout =CAppEnv::getBinOutDirPath() + "/" + QFileInfo(m_curFile).baseName() + ".bin.txt";
//    m_paraInfo->binFNP = binout;
//    QString outputFifo = CAppEnv::getFifoOutDirPath() + "/" + QFileInfo(m_curFile).baseName();
//    m_paraInfo->outputFifoFNP0 = outputFifo + "0.log";
//    m_paraInfo->outputFifoFNP1 = outputFifo + "1.log";
//    m_paraInfo->outputFifoFNP2 = outputFifo + "2.log";
//    m_paraInfo->outputFifoFNP3 = outputFifo + "3.log";
//    QString outMem = CAppEnv::getMemOutDirPath() + "/" + QFileInfo(m_curFile).baseName();
//    m_paraInfo->outMemFNP0 = outMem + "0.txt";
//    m_paraInfo->outMemFNP1 = outMem + "1.txt";
//    m_paraInfo->outMemFNP2 = outMem + "2.txt";
//    m_paraInfo->outMemFNP3 = outMem + "3.txt";
//    m_paraInfo->outMemFNP4 = outMem + "4.txt";

//    m_cmdWordInfo->xmlFile = m_curFile;
//    m_cmdWordInfo->onlySim = getCodeChildElement("ONLYSIM").attribute("value").toInt();
//    m_cmdWordInfo->rMode0 = getCodeChildElement("RMODE0").attribute("value").toInt();
//    m_cmdWordInfo->rMode1 = getCodeChildElement("RMODE1").attribute("value").toInt();
//    m_cmdWordInfo->gap = getCodeChildElement("GAP").attribute("value").toInt();
//    m_cmdWordInfo->source = getCodeSourceElement().attribute("value");
//    m_cmdWordInfo->dest = getCodeDestElement().attribute("value");
//    m_cmdWordInfo->burstNum = getCodeChildElement("BURSTNUM").attribute("value").toInt();
//    m_cmdWordInfo->loopNum = getCodeChildElement("LOOPNUM").attribute("value").toInt();
//    m_cmdWordInfo->repeatNum = getCodeChildElement("REPEATNUM").attribute("value").toInt();
//    m_cmdWordInfo->progress = 0;
//    m_cmdWordInfo->selected = Qt::Unchecked;
//    m_cmdWordInfo->status = tr("准备就绪");

//    m_paraInfo->cmdWordInfo = m_cmdWordInfo;
//}

///**
// * CXmlResolver::getParaInfo
// * \brief
// * \return  SParaInfo *
// * \author  zhangjun
// * \date    2016-10-12
// */
//SParaInfo *CXmlResolver::getParaInfo()
//{
//    return m_paraInfo;
//}

///**
// * CXmlResolver::getCmdWordInfo
// * \brief
// * \return  SCMDWordInfo *
// * \author  zhangjun
// * \date    2016-10-12
// */
//SCMDWordInfo *CXmlResolver::getCmdWordInfo()
//{
//    return m_cmdWordInfo;
//}

///**
// * CXmlResolver::setParaElementAttribute
// * \brief
// * \author  zhangjun
// * \date    2016-10-12
// */
//void CXmlResolver::setParaElementAttribute()
//{
//    QDomElement element = findCycleElement();
//    element.setAttribute("value", QString::number(m_paraInfo->cycle));

//    element = findIntervalElement();
//    element.setAttribute("value", QString::number(m_paraInfo->triggerInterval));

//    element = findFifoInput(0);
//    element.setAttribute("value", m_paraInfo->inputFifoFNP0);
//    element = findFifoInput(1);
//    element.setAttribute("value", m_paraInfo->inputFifoFNP1);
//    element = findFifoInput(2);
//    element.setAttribute("value", m_paraInfo->inputFifoFNP2);
//    element = findFifoInput(3);
//    element.setAttribute("value", m_paraInfo->inputFifoFNP3);

////    element = findFifoOutput(0);
////    element.setAttribute("value", m_paraInfo->outputFifoFNP0);
////    element = findFifoOutput(1);
////    element.setAttribute("value", m_paraInfo->outputFifoFNP1);
////    element = findFifoOutput(2);
////    element.setAttribute("value", m_paraInfo->outputFifoFNP2);
////    element = findFifoOutput(3);
////    element.setAttribute("value", m_paraInfo->outputFifoFNP3);

//    element = findMemInput(0);
//    element.setAttribute("value", m_paraInfo->inMemFNP0);
//    element = findMemInput(1);
//    element.setAttribute("value", m_paraInfo->inMemFNP1);
//    element = findMemInput(2);
//    element.setAttribute("value", m_paraInfo->inMemFNP2);
//    element = findMemInput(3);
//    element.setAttribute("value", m_paraInfo->inMemFNP3);
//    element = findMemInput(4);
//    element.setAttribute("value", m_paraInfo->inMemFNP4);

////    element = findMemOutput(0);
////    element.setAttribute("value", m_paraInfo->outMemFNP0);
////    element = findMemOutput(1);
////    element.setAttribute("value", m_paraInfo->outMemFNP1);
////    element = findMemOutput(2);
////    element.setAttribute("value", m_paraInfo->outMemFNP2);
////    element = findMemOutput(3);
////    element.setAttribute("value", m_paraInfo->outMemFNP3);
////    element = findMemOutput(4);
////    element.setAttribute("value", m_paraInfo->outMemFNP4);

//    QDomElement codeChild = getCodeChildElement("ONLYSIM");
//    codeChild.setAttribute("value", QString::number(m_cmdWordInfo->onlySim));
//    codeChild = getCodeChildElement("RMODE0");
//    codeChild.setAttribute("value", QString::number(m_cmdWordInfo->rMode0));
//    codeChild = getCodeChildElement("RMODE1");
//    codeChild.setAttribute("value", QString::number(m_cmdWordInfo->rMode1));
//    codeChild = getCodeChildElement("GAP");
//    codeChild.setAttribute("value", QString::number(m_cmdWordInfo->gap));
//    codeChild = getCodeSourceElement();
//    codeChild.setAttribute("value", m_cmdWordInfo->source);
//    codeChild = getCodeDestElement();
//    codeChild.setAttribute("value", m_cmdWordInfo->dest);
//    codeChild = getCodeChildElement("BURSTNUM");
//    codeChild.setAttribute("value", QString::number(m_cmdWordInfo->burstNum));
//    codeChild = getCodeChildElement("LOOPNUM");
//    codeChild.setAttribute("value", QString::number(m_cmdWordInfo->loopNum));
//    codeChild = getCodeChildElement("REPEATNUM");
//    codeChild.setAttribute("value", QString::number(m_cmdWordInfo->repeatNum));
//}

/**
 * CXmlResolver::findBCUElement
 * \brief
 * \param   bcuid
 * \return  QDomElement
 * \author  zhangjun
 * \date    2016-10-12
 */
QDomElement CXmlResolver::findBCUElement(int bcuid)
{
    if(bcuid >= BCUMAX)
    {
        QDomElement element;
        return element;
    }
    QDomNodeList list = m_rca.childNodes();
    for(int i = 0; i<list.count(); i++)
    {
        QDomElement bcu = list.at(i).toElement();
        if(bcu.tagName() == "BCU" && bcu.attribute("id").toInt() == bcuid)return bcu;
    }
    //没找到 新建
    QDomElement element = m_document->createElement("BCU");
    element.setAttribute("id",bcuid);
    m_rca.appendChild(element);
    return element;
}

QDomElement CXmlResolver::findRCULoopElement(int bcuid, int rcuid)
{
    if(bcuid >= BCUMAX)
    {
        QDomElement element;
        return element;
    }
    QDomElement rcu = findRCUElement(bcuid,rcuid);
    QDomNodeList list = rcu.childNodes();
    for(int i = 0; i<list.count(); i++)
    {
        QDomElement mem_read = list.at(i).toElement();
        if(mem_read.tagName() == "LOOP")return mem_read;
    }
    //没找到 新建
    QDomElement element = m_document->createElement("LOOP");
    rcu.appendChild(element);
    return element;
}
/**
 * CXmlResolver::findRCUMemWTElement
 * \brief
 * \param   bcuid
 * \param   rcuid
 * \return  QDomElement
 * \author  zhangjun
 * \date    2016-10-12
 */
QDomElement CXmlResolver::findRCUMemWTElement(int bcuid, int rcuid)
{
    if(bcuid >= BCUMAX)
    {
        QDomElement element;
        return element;
    }
    QDomElement rcu = findRCUElement(bcuid,rcuid);
    QDomNodeList list = rcu.childNodes();
    for(int i = 0; i<list.count(); i++)
    {
        QDomElement mem_read = list.at(i).toElement();
        if(mem_read.tagName() == "MEM_WRITE")return mem_read;
    }
    //没找到 新建
    QDomElement element = m_document->createElement("MEM_WRITE");
    rcu.appendChild(element);
    return element;
}

/**
 * CXmlResolver::findRCUMemRDElement
 * \brief
 * \param   bcuid
 * \param   rcuid
 * \return  QDomElement
 * \author  zhangjun
 * \date    2016-10-12
 */
QDomElement CXmlResolver::findRCUMemRDElement(int bcuid, int rcuid)
{
    if(bcuid >= BCUMAX)
    {
        QDomElement element;
        return element;
    }
    QDomElement rcu = findRCUElement(bcuid,rcuid);
    QDomNodeList list = rcu.childNodes();
    for(int i = 0; i<list.count(); i++)
    {
        QDomElement mem_write = list.at(i).toElement();
        if(mem_write.tagName() == "MEM_READ")return mem_write;
    }
    //没找到 新建
    QDomElement element = m_document->createElement("MEM_READ");
    rcu.appendChild(element);
    return element;
}

/**
 * CXmlResolver::findRCURchElement
 * \brief
 * \param   bcuid
 * \param   rcuid
 * \return  QDomElement
 * \author  zhangjun
 * \date    2021-01-02
 */
QDomElement CXmlResolver::findRCURchElement(int bcuid, int rcuid)
{
    if(bcuid >= BCUMAX)
    {
        QDomElement element;
        return element;
    }
    QDomElement rcu = findRCUElement(bcuid,rcuid);
    QDomNodeList list = rcu.childNodes();
    for(int i = 0; i<list.count(); i++)
    {
        QDomElement mem_write = list.at(i).toElement();
        if(mem_write.tagName() == XML_RCU_RCH_NAME)return mem_write;
    }
    //没找到 新建
    QDomElement element = m_document->createElement(XML_RCU_RCH_NAME);
    rcu.appendChild(element);
    return element;
}

/**
 * CXmlResolver::findRCUElement
 * \brief
 * \param   bcuid
 * \param   rcuid
 * \return  QDomElement
 * \author  zhangjun
 * \date    2016-10-12
 */
QDomElement CXmlResolver::findRCUElement(int bcuid, int rcuid)
{
    QDomElement bcu = findBCUElement(bcuid);
    if(bcuid >= BCUMAX || rcuid >= RCUMAX || bcu.isNull())
    {
        QDomElement element;
        return element;
    }
    QDomNodeList list = bcu.childNodes();
    for(int i = 0; i<list.count(); i++)
    {
        QDomElement rcu = list.at(i).toElement();
        if(rcu.tagName() == "RCU" && rcu.attribute("id").toInt() == rcuid)return rcu;
    }
    //没找到
    QDomElement element = m_document->createElement("RCU");
    element.setAttribute("id",rcuid);
    bcu.appendChild(element);
    return element;
}
/**
 * CXmlResolver::findBFUInput
 * \brief
 * \param   bcuid
 * \param   rcuid
 * \param   id
 * \param   type
 * \return  QDomElement
 * \author  zhangjun
 * \date    2016-10-12
 */
QDomElement CXmlResolver::findBFUInput(int bcuid, int rcuid, int id, Input_Type type)
{
    QDomElement bfu = findElement(bcuid,rcuid,ELEMENT_BFU,id);
    if(bcuid >= BCUMAX || rcuid >= RCUMAX || id >= BFUMAX || bfu.isNull())
    {
        QDomElement element;
        return element;
    }
    QString tagname = (type==INPUT_A)?"INPUTA":(type==INPUT_B)?"INPUTB":"INPUTT";

    QDomNodeList list = bfu.childNodes();
    for(int i = 0; i<list.count(); i++)
    {
        QDomElement ele = list.at(i).toElement();
        if(ele.tagName() == tagname)return ele;
    }
    QDomElement ele = m_document->createElement(tagname);
    bfu.appendChild(ele);
    return ele;
}
/**
 * XmlResolver::findCUSTOMPEInput
 * \brief
 * \param   bcuid
 * \param   rcuid
 * \param   id
 * \return  QDomElement
 * \author  xufei
 * \date    2021-2-22
 */
QDomElement CXmlResolver::findCUSTOMPEInput(int bcuid, int rcuid,int elementid, int inputid)
{
    QDomElement custompe = findElement(bcuid,rcuid,ELEMENT_CUSTOMPE,elementid);
    if(bcuid >= BCUMAX || rcuid >= RCUMAX || inputid >= BFUMAX || custompe.isNull())
    {
        QDomElement element;
        return element;
    }
    QDomNodeList list = custompe.childNodes();
    for(int i = 0; i<list.count(); i++)
    {
        QDomElement ele = list.at(i).toElement();
        if(ele.tagName() == "INPUT" && ele.attribute("id").toInt() == inputid)
            return ele;
    }
    QDomElement ele = m_document->createElement("INPUT");
    ele.setAttribute("id",inputid);
    custompe.appendChild(ele);
    return ele;
}
/**
 * CXmlResolver::findSBOXInput
 * \brief
 * \param   bcuid
 * \param   rcuid
 * \param   id
 * \return  QDomElement
 * \author  zhangjun
 * \date    2016-10-12
 */
QDomElement CXmlResolver::findSBOXInput(int bcuid, int rcuid, int id)
{
    QDomElement sbox = findElement(bcuid,rcuid,ELEMENT_SBOX,id);
    if(bcuid >= BCUMAX || rcuid >= RCUMAX || id >= SBOXMAX || sbox.isNull())
    {
        QDomElement element;
        return element;
    }
    QDomNodeList list = sbox.childNodes();
    for(int i = 0; i<list.count(); i++)
    {
        QDomElement ele = list.at(i).toElement();
        if(ele.tagName() == "INPUT")return ele;
    }
    QDomElement ele = m_document->createElement("INPUT");
    sbox.appendChild(ele);
    return ele;
}
/**
 * CXmlResolver::findBENESInput
 * \brief
 * \param   bcuid
 * \param   rcuid
 * \param   inputid
 * \return  QDomElement
 * \author  zhangjun
 * \date    2016-10-12
 */
QDomElement CXmlResolver::findBENESInput(int bcuid, int rcuid, int inputid)
{
    QDomElement benes = findElement(bcuid,rcuid,ELEMENT_BENES);
    if(bcuid >= BCUMAX || rcuid >= RCUMAX || inputid >= BENESINPUTNUM || benes.isNull())
    {
        QDomElement element;
        return element;
    }
    QDomNodeList list = benes.childNodes();
    for(int i = 0; i<list.count(); i++)
    {
        QDomElement ele = list.at(i).toElement();
        if(ele.tagName() == "INPUT" && ele.attribute("id").toInt() == inputid)return ele;
    }
    QDomElement ele = m_document->createElement("INPUT");
    ele.setAttribute("id",inputid);
    benes.appendChild(ele);
    return ele;
}

/**
 * CXmlResolver::findSBOXCnt
 * \brief
 * \param   bcuid
 * \param   rcuid
 * \param   sboxid
 * \param   cnt
 * \return  QDomElement
 * \author  zhangjun
 * \date    2016-10-12
 */
QDomElement CXmlResolver::findSBOXCnt(int bcuid, int rcuid, int sboxid, int cnt)
{
    QDomElement sbox = findElement(bcuid,rcuid,ELEMENT_SBOX,sboxid);
    if(bcuid >= BCUMAX || rcuid >= RCUMAX || sboxid >= SBOXMAX || sbox.isNull())
    {
        QDomElement element;
        return element;
    }
    QDomNodeList list = sbox.childNodes();
    for(int i = 0; i<list.count(); i++)
    {
        QDomElement ele = list.at(i).toElement();
        if(ele.tagName() == "CNT" && ele.attribute("id").toInt() == cnt)return ele;
    }
    QDomElement ele = m_document->createElement("CNT");
    ele.setAttribute("id",cnt);
    sbox.appendChild(ele);
    return ele;
}

/**
 * CXmlResolver::findElement
 * \brief
 * \param   bcuid
 * \param   rcuid
 * \param   element
 * \param   id
 * \return  QDomElement
 * \author  zhangjun
 * \date    2016-10-12
 */
QDomElement CXmlResolver::findElement(int bcuid, int rcuid, ElementType element, int id)
{
    QDomElement rcu = findRCUElement(bcuid,rcuid);
    int max = (element==ELEMENT_BFU || element == ELEMENT_CUSTOMPE)?BFUMAX:((element==ELEMENT_SBOX)?SBOXMAX:BENESMAX);
    if(bcuid >= BCUMAX || rcuid >= RCUMAX || id >= max || rcu.isNull())
    {
        QDomElement newElement;
        return newElement;
    }
    QDomNodeList list = rcu.childNodes();
    QString name = element==ELEMENT_BFU?"BFU":
                        element == ELEMENT_CUSTOMPE?"CBFU":
                            element==ELEMENT_SBOX?"SBOX":
                                 element==ELEMENT_BENES?"BENES":"";
    if(name.isEmpty())
    {
        QDomElement newElement;
        return newElement;
    }
    for(int i = 0; i<list.count(); i++)
    {
        QDomElement ele = list.at(i).toElement();
        if(ele.tagName() == name && ele.attribute("id").toInt() == id)return ele;
    }
    //
    QDomElement ret = m_document->createElement(name);
    ret.setAttribute("id",id);
    rcu.appendChild(ret);
    return ret;
}
bool CXmlResolver::isHadBcuInXml(int bcuid)
{
    if(bcuid >= BCUMAX)
    {
        return false;
    }
    QDomNodeList list = m_rca.childNodes();
    for(int i = 0; i<list.count(); i++)
    {
        QDomElement bcu = list.at(i).toElement();
        if(bcu.tagName() == "BCU" && bcu.attribute("id").toInt() == bcuid)
        {
            return true;
        }
    }
    return false;
}

bool CXmlResolver::isHadRcuInXml(int bcuid, int rcuid)
{
    if(isHadBcuInXml(bcuid) == false) return false;

    QDomElement bcu = findBCUElement(bcuid);
    if(bcuid >= BCUMAX || rcuid >= RCUMAX || bcu.isNull())
    {
        return false;
    }
    QDomNodeList list = bcu.childNodes();
    for(int i = 0; i<list.count(); i++)
    {
        QDomElement rcu = list.at(i).toElement();
        if(rcu.tagName() == "RCU" && rcu.attribute("id").toInt() == rcuid)
        {
            return true;
        }
    }
    return false;
}

bool CXmlResolver::isHadWriteMemInXml(int bcuid, int rcuid, int memputid)
{
    if(isHadRcuInXml(bcuid,rcuid) == false)  return false;
    QDomElement rcu = findRCUElement(bcuid,rcuid);
    if(bcuid >= BCU_MAX_COUNT || rcuid >= RCU_MAX_COUNT || rcu.isNull())
    {
        return false;
    }
    QDomNodeList list = rcu.childNodes();
    for(int i = 0; i<list.count(); i++)
    {
        QDomElement mem = list.at(i).toElement();
        if(mem.tagName() == XML_RCU_MEM_INPUT_NAME && mem.attribute("id").toInt() == memputid)
        {
            return true;
        }
    }

    return false;
}

bool CXmlResolver::isHadWriteRchInXml(int bcuid, int rcuid, int memputid)
{
    if(isHadRcuInXml(bcuid,rcuid) == false)  return false;
    QDomElement rcu = findRCUElement(bcuid,rcuid);
    if(bcuid >= BCU_MAX_COUNT || rcuid >= RCU_MAX_COUNT || rcu.isNull())
    {
        return false;
    }
    QDomNodeList list = rcu.childNodes();
    for(int i = 0; i<list.count(); i++)
    {
        QDomElement mem = list.at(i).toElement();
        if(mem.tagName() == XML_RCU_RCH_INPUT_NAME && mem.attribute("id").toInt() == memputid)
        {
            return true;
        }
    }

    return false;
}

bool CXmlResolver::isHadWriteLfsrInXml(int bcuid, int rcuid, int memputid)
{
    if(isHadRcuInXml(bcuid,rcuid) == false)  return false;
    QDomElement rcu = findRCUElement(bcuid,rcuid);
    if(bcuid >= BCU_MAX_COUNT || rcuid >= RCU_MAX_COUNT || rcu.isNull())
    {
        return false;
    }
    QDomNodeList list = rcu.childNodes();
    for(int i = 0; i<list.count(); i++)
    {
        QDomElement mem = list.at(i).toElement();
        if(mem.tagName() == XML_RCU_LFSR_INPUT_NAME && mem.attribute("id").toInt() == memputid)
        {
            return true;
        }
    }

    return false;
}

/**
 * CXmlResolver::findRCUMEMElement
 * \brief
 * \param   bcuid
 * \param   rcuid
 * \param   memputid
 * \return  QDomElement
 * \author  zhangjun
 * \date    2016-10-12
 */
QDomElement CXmlResolver::findRCUMEMElement(int bcuid, int rcuid, int memputid)
{
    QDomElement rcu = findRCUElement(bcuid,rcuid);
    if(bcuid >= BCUMAX || rcuid >= RCUMAX || rcu.isNull())
    {
        QDomElement element;
        return element;
    }
    QDomNodeList list = rcu.childNodes();
    for(int i = 0; i<list.count(); i++)
    {
        QDomElement mem = list.at(i).toElement();
        if(mem.tagName() == TAGRCUMEM && mem.attribute("id").toInt() == memputid)return mem;
    }
    //
    QDomElement element = m_document->createElement(TAGRCUMEM);
    element.setAttribute("id",memputid);
    rcu.appendChild(element);
    return element;

}

QDomElement CXmlResolver::findRCURCHElement(int bcuid, int rcuid, int id)
{
    QDomElement rcu = findRCUElement(bcuid,rcuid);
    if(bcuid >= BCUMAX || rcuid >= RCUMAX || rcu.isNull())
    {
        QDomElement element;
        return element;
    }
    QDomNodeList list = rcu.childNodes();
    for(int i = 0; i < list.count(); i++)
    {
        QDomElement ele = list.at(i).toElement();
        if(ele.tagName() == TAGRCURCH && ele.attribute("id").toInt() == id) return ele;
    }
    //
    QDomElement element = m_document->createElement(TAGRCURCH);
    element.setAttribute("id", id);
    rcu.appendChild(element);
    return element;
}

QDomElement CXmlResolver::findRCULFSRElement(int bcuid, int rcuid, int id)
{
    QDomElement rcu = findRCUElement(bcuid,rcuid);
    if(bcuid >= BCUMAX || rcuid >= RCUMAX || rcu.isNull())
    {
        QDomElement element;
        return element;
    }
    QDomNodeList list = rcu.childNodes();
    for(int i = 0; i < list.count(); i++)
    {
        QDomElement ele = list.at(i).toElement();
        if(ele.tagName() == TAGRCULFSR && ele.attribute("id").toInt() == id) return ele;
    }
    //
    QDomElement element = m_document->createElement(TAGRCULFSR);
    element.setAttribute("id", id);
    rcu.appendChild(element);
    return element;
}

QDomElement CXmlResolver::findRCUOUTElement(int bcuid, int rcuid, int outputid)
{
    QDomElement rcu = findRCUElement(bcuid,rcuid);
    if(bcuid >= BCUMAX || rcuid >= RCUMAX || rcu.isNull())
    {
        QDomElement element;
        return element;
    }
    QDomNodeList list = rcu.childNodes();
    for(int i = 0; i<list.count(); i++)
    {
        QDomElement out = list.at(i).toElement();
        if(out.tagName() == TAGOUTFIFO && out.attribute("id").toInt() == outputid)return out;
    }
    //
    QDomElement element = m_document->createElement(TAGOUTFIFO);
    element.setAttribute("id",outputid);
    rcu.appendChild(element);
    return element;
}
/**
 * CXmlResolver::findBCUOUTElement
 * \brief
 * \param   bcuid
 * \param   outputid
 * \return  QDomElement
 * \author  zhangjun
 * \date    2016-10-12
 */
//QDomElement CXmlResolver::findBCUOUTElement(int bcuid, int outputid)
//{
//    QDomElement bcu = findBCUElement(bcuid);
//    if(bcuid >= BCUMAX || bcu.isNull())
//    {
//        QDomElement element;
//        return element;
//    }
//    QDomNodeList list = bcu.childNodes();
//    for(int i = 0; i<list.count(); i++)
//    {
//        QDomElement out = list.at(i).toElement();
//        if(out.tagName() == TAGOUTFIFO && out.attribute("id").toInt() == outputid)return out;
//    }
//    //
//    QDomElement element = m_document->createElement(TAGOUTFIFO);
//    element.setAttribute("id",outputid);
//    bcu.appendChild(element);
//    return element;
//}

QDomElement CXmlResolver::findPortInfelctPos(QDomElement *domElement)
{
    QDomNodeList list = domElement->childNodes();
    for(int i = 0; i<list.count(); i++)
    {
        QDomElement out = list.at(i).toElement();
        if(out.tagName() == XML_PORT_INFECT_POS)return out;
    }
    //
    QDomElement element = m_document->createElement(XML_PORT_INFECT_POS);
    domElement->appendChild(element);
    return element;
}

QDomElement CXmlResolver::findPortPressedPos(QDomElement *domElement)
{
    QDomNodeList list = domElement->childNodes();
    for(int i = 0; i<list.count(); i++)
    {
        QDomElement out = list.at(i).toElement();
        if(out.tagName() == XML_PORT_PRESSED_POS)return out;
    }
    //
    QDomElement element = m_document->createElement(XML_PORT_PRESSED_POS);
    domElement->appendChild(element);
    return element;
}
/**
 * CXmlResolver::removeElement
 * \brief
 * \param   bcuid
 * \param   rcuid
 * \param   element
 * \param   id
 * \author  zhangjun
 * \date    2016-10-12
 */
void CXmlResolver::removeElement(int bcuid, int rcuid, ElementType element, int id)
{
    findRCUElement(bcuid,rcuid).removeChild(findElement(bcuid,rcuid,element,id));
}

void CXmlResolver::removeBcuElement(int bcuid)
{
    m_rca.removeChild(findBCUElement(bcuid));
}

void CXmlResolver::removeRcuElement(int bcuid, int rcuid)
{
    findBCUElement(bcuid).removeChild(findRCUElement(bcuid,rcuid));
}

void CXmlResolver::removeRcuMemElemtnt(int bcuid, int rcuid, int memputid)
{
    findRCUElement(bcuid,rcuid).removeChild(findRCUMEMElement(bcuid,rcuid,memputid));
}

void CXmlResolver::readXmlToPort(ElementPort* port, QDomElement* portXml)
{
    port->setInputType(InputPortType(portXml->attribute(XML_InPort_Type).toInt()));
    port->setInputIndex(portXml->attribute(XML_InPort_Index).toInt());
    //    port->middlePosY = portXml->attribute(XML_InPort_Middle_Y).toInt();

    QDomNodeList portPosNodeList = portXml->childNodes();
    for(int i = 0; i<portPosNodeList.count(); i++)
    {
        QDomElement portPos = portPosNodeList.at(i).toElement();
        if(portPos.tagName() == XML_PORT_INFECT_POS)
        {
            port->setInfletionPosByXml(portPos.attribute(XML_INFECT_POS));
        }
        else if(portPos.tagName() == XML_PORT_PRESSED_POS)
        {
            port->setPressedPosByXml(portPos.attribute(XML_PRESSED_POS));
        }
    }
}

bool CXmlResolver::initXmlToCode(CCode &code, const SProjectParameter *projectParameter)
{
    static const auto type2SIndex = [](InputPortType type, QPair<BasePe::SIndex, int> &sindexPair){
        auto sourcePeID = sindexPair.first.peIndex;
        auto sourcePePortID = sindexPair.second;
        switch (type) {
        case InputPort_NULL:
            sindexPair.first.bcuIndex = -1;
            sindexPair.first.rcuIndex = -1;
            sindexPair.first.peIndex = -1;
            sindexPair.second = -1;
            sourcePeID = -1;
            sourcePePortID = -1;
            break;
        case InputPort_LastBfuX:
            sindexPair.first.rcuIndex--;
        case InputPort_CurrentBfuX:
            sourcePeID = sourcePePortID;
            sourcePePortID = 0;
            break;
        case InputPort_LastBfuY:
            sindexPair.first.rcuIndex--; // goto next case
        case InputPort_CurrentBfuY:
            sourcePeID = sourcePePortID;
            sourcePePortID = 1;
            break;
        case InputPort_LastSbox:
            sindexPair.first.rcuIndex--; // goto next case
        case InputPort_CurrentSbox:
            sourcePeID = 8 + sourcePePortID/4;
            sourcePePortID = sourcePePortID%4;
            break;
        case InputPort_LastBenes:
            sindexPair.first.rcuIndex--; // goto next case
        case InputPort_CurrentBenes:
            sourcePeID = 12;
            break;
        case InputPort_Mem:
            sourcePeID = 13;
            break;
        case InputPort_Imd:
            sourcePeID = 14;
            break;
        case InputPort_Rch:
            sourcePeID = 15;
            break;
        case InputPort_InFifo:
            sourcePeID = 16;
            break;
        case InputPort_Lfsr:
            sourcePeID = 17;
            break;
        default:
            break;
        }
        sindexPair.first.peIndex = sourcePeID;
        sindexPair.second = sourcePePortID;
    };

    QDomNodeList bcuList = m_rca.childNodes();

    QL_DEBUG << bcuList.count();
    for(int bcuIndex = 0; bcuIndex<bcuList.count(); bcuIndex++)
    {
        QDomElement bcuXml = bcuList.at(bcuIndex).toElement();
        if((bcuXml.tagName() == mapModuleUnitName.value(Module_Bcu))
            && (bcuXml.attribute(XML_Attr_Id).toInt() < BCU_MAX_COUNT))
        {
            int bcuIndexInRca = bcuXml.attribute(XML_Attr_Id).toInt();
            if(code.findBcu(bcuIndexInRca) != nullptr)
                continue;

            CBcuElement *bcuElement = new CBcuElement;
            code.addBcuElement(bcuElement);
            bcuElement->setIndex(bcuIndexInRca, 0, 0);
            bcuElement->sort = bcuXml.attribute(XML_BCU_DATA_SORT).toInt();
            bcuElement->burst = bcuXml.attribute(XML_BCU_DATA_BURST).toInt();
            bcuElement->gap = bcuXml.attribute(XML_BCU_DATA_GAP).toInt();
            bcuElement->readImdMode128_32 = bcuXml.attribute(XML_BCU_RIMD_MODE128_32).toInt();
            bcuElement->memorySrcConfig = bcuXml.attribute(XML_BCU_MEM_SRC);
            if(!bcuElement->memorySrcConfig.startsWith(":/"))
                bcuElement->memorySrcConfig.insert(0, QString(":/"));
            bcuElement->inFifoSrcConfig = bcuXml.attribute(XML_BCU_INFIFO_SRC);
            if(!bcuElement->inFifoSrcConfig.startsWith(":/"))
                bcuElement->inFifoSrcConfig.insert(0, QString(":/"));
            bcuElement->imdSrcConfig = bcuXml.attribute(XML_BCU_IMD_SRC);
            if(!bcuElement->imdSrcConfig.startsWith(":/"))
                bcuElement->imdSrcConfig.insert(0, QString(":/"));

            QDomNodeList rcuListXml = bcuXml.childNodes();
            for(int rcuIndex = 0; rcuIndex < rcuListXml.count(); rcuIndex++)
            {
                QDomElement rcuXml = rcuListXml.at(rcuIndex).toElement();
                if((rcuXml.tagName() == mapModuleUnitName.value(Module_Rcu))
                        && (rcuXml.attribute(XML_Attr_Id).toInt() < RCU_MAX_COUNT))
                {
                    int rcuIndexInBcu = rcuXml.attribute(XML_Attr_Id).toInt();
                    if(bcuElement->findRcu(bcuIndexInRca, rcuIndexInBcu) != nullptr)
                        continue;

                    CRcuElement *rcuElement = new CRcuElement;
                    bcuElement->addRcuElement(rcuElement);
                    rcuElement->setIndex(bcuIndexInRca, rcuIndexInBcu, 0);

                    CLfsrElement *lfsrElement = new CLfsrElement;
                    lfsrElement->setIndex(bcuIndexInRca, rcuIndexInBcu, BFU_COUNT+SBOX_COUNT+BENES_COUNT+MEMORY_COUNT+SREG_COUNT+OUTFIFO_COUNT);
                    rcuElement->addLfsrElement(lfsrElement);
                    CMemoryElement *memoryElement = new CMemoryElement;
                    rcuElement->addMemoryElement(memoryElement);
                    memoryElement->setIndex(bcuIndexInRca, rcuIndexInBcu, BFU_COUNT+SBOX_COUNT+BENES_COUNT);
                    CSRegElement *sregElement = new CSRegElement;
                    sregElement->setIndex(bcuIndexInRca, rcuIndexInBcu, BFU_COUNT+SBOX_COUNT+BENES_COUNT+MEMORY_COUNT);
                    rcuElement->addSregElement(sregElement);
                    COutfifoElement *outfifoElement = new COutfifoElement;
                    rcuElement->addOutfifoElement(outfifoElement);
                    outfifoElement->setIndex(bcuIndexInRca, rcuIndexInBcu, BFU_COUNT+SBOX_COUNT+BENES_COUNT+MEMORY_COUNT+SREG_COUNT);

                    QDomNodeList rcuChildNodesXml = rcuXml.childNodes();
                    for(int rcuChildNodesIndex = 0; rcuChildNodesIndex < rcuChildNodesXml.count(); rcuChildNodesIndex++)
                    {
                        QDomElement rcuMemXml = rcuChildNodesXml.at(rcuChildNodesIndex).toElement();
                        if(rcuMemXml.tagName() == XML_RCU_WMEM_NAME)
                        {
                            memoryElement->writeMemOffset = rcuMemXml.attribute(XML_RCU_WMEM_OFFSET).toInt();
                            memoryElement->writeMemMask = rcuMemXml.attribute(XML_RCU_WMEM_MASK).toInt();
                            memoryElement->writeMemMode = rcuMemXml.attribute(XML_RCU_WMEM_MODE).toInt();
                            memoryElement->writeMemThreashold = rcuMemXml.attribute(XML_RCU_WMEM_THREASHOLD).toInt();
                            memoryElement->writeMemAddr = rcuMemXml.attribute(XML_RCU_WMEM_ADDR).toInt();
                        }
                        else if(rcuMemXml.tagName() == XML_RCU_RMEM_NAME)
                        {
                            memoryElement->readMemOffset = rcuMemXml.attribute(XML_RCU_RMEM_OFFSET).toInt();
                            memoryElement->readMemMode = rcuMemXml.attribute(XML_RCU_RMEM_MODE).toInt();
                            memoryElement->readMemMode128_32 = rcuMemXml.attribute(XML_RCU_RMEM_MODE128_32).toInt();
                            memoryElement->readMemThreashold = rcuMemXml.attribute(XML_RCU_RMEM_THREASHOLD).toInt();
                            memoryElement->readMemAddr1 = rcuMemXml.attribute(XML_RCU_RMEM_ADDR1).toInt();
                            memoryElement->readMemAddr2 = rcuMemXml.attribute(XML_RCU_RMEM_ADDR2).toInt();
                            memoryElement->readMemAddr3 = rcuMemXml.attribute(XML_RCU_RMEM_ADDR3).toInt();
                            memoryElement->readMemAddr4 = rcuMemXml.attribute(XML_RCU_RMEM_ADDR4).toInt();
                        }
                        else if(rcuMemXml.tagName() == XML_RCU_RCH_NAME)
                        {
                            sregElement->rchMode = rcuMemXml.attribute(XML_RCU_RCH_MODE).toInt();
//                            sregElement->writeRchAddr1 = rcuMemXml.attribute(XML_RCU_RCH_ADDR1).toInt();
//                            sregElement->writeRchAddr2 = rcuMemXml.attribute(XML_RCU_RCH_ADDR2).toInt();
//                            sregElement->writeRchAddr3 = rcuMemXml.attribute(XML_RCU_RCH_ADDR3).toInt();
//                            sregElement->writeRchAddr4 = rcuMemXml.attribute(XML_RCU_RCH_ADDR4).toInt();
                        }
                        else if(rcuMemXml.tagName() == XML_RCU_MEM_INPUT_NAME)
                        {
                            InputPortType type = InputPortType(rcuMemXml.attribute(XML_InPort_Type).toInt());
                            int sourcePePortID = rcuMemXml.attribute(XML_InPort_Index).toInt();
                            int sourcePeID = 0;
                            QPair<BasePe::SIndex, int> sindexPair = {{bcuIndexInRca, rcuIndexInBcu, sourcePeID}, sourcePePortID};
                            type2SIndex(type, sindexPair);

                            int inputPortID = rcuMemXml.attribute(XML_Attr_Id).toInt();
                            memoryElement->m_inputSource.replace(inputPortID, sindexPair);
                        }
                        else if(rcuMemXml.tagName() == XML_RCU_RCH_INPUT_NAME)
                        {
                            if(rcuMemXml.attribute(XML_Attr_Id).toInt() == 0)
                                sregElement->writeRchAddr1 = rcuMemXml.attribute(XML_InPort_Address).toInt();
                            else if(rcuMemXml.attribute(XML_Attr_Id).toInt() == 1)
                                sregElement->writeRchAddr2 = rcuMemXml.attribute(XML_InPort_Address).toInt();
                            else if(rcuMemXml.attribute(XML_Attr_Id).toInt() == 2)
                                sregElement->writeRchAddr3 = rcuMemXml.attribute(XML_InPort_Address).toInt();
                            else if(rcuMemXml.attribute(XML_Attr_Id).toInt() == 3)
                                sregElement->writeRchAddr4 = rcuMemXml.attribute(XML_InPort_Address).toInt();

                            InputPortType type = InputPortType(rcuMemXml.attribute(XML_InPort_Type).toInt());
                            int sourcePePortID = rcuMemXml.attribute(XML_InPort_Index).toInt();
                            int sourcePeID = 0;
                            QPair<BasePe::SIndex, int> sindexPair = {{bcuIndexInRca, rcuIndexInBcu, sourcePeID}, sourcePePortID};
                            type2SIndex(type, sindexPair);

                            int inputPortID = rcuMemXml.attribute(XML_Attr_Id).toInt();
                            sregElement->m_inputSource.replace(inputPortID, sindexPair);
                        }
                        else if(rcuMemXml.tagName() == XML_RCU_LFSR_INPUT_NAME)
                        {
                            InputPortType type = InputPortType(rcuMemXml.attribute(XML_InPort_Type).toInt());
                            int sourcePePortID = rcuMemXml.attribute(XML_InPort_Index).toInt();
                            int sourcePeID = 0;
                            QPair<BasePe::SIndex, int> sindexPair = {{bcuIndexInRca, rcuIndexInBcu, sourcePeID}, sourcePePortID};
                            type2SIndex(type, sindexPair);

                            int inputPortID = rcuMemXml.attribute(XML_Attr_Id).toInt();
                            lfsrElement->m_inputSource.replace(inputPortID, sindexPair);
                        }
                        else if(rcuMemXml.tagName() == XML_BCU_OUT_FIFO)
                        {
                            InputPortType type = InputPortType(rcuMemXml.attribute(XML_InPort_Type).toInt());
                            int sourcePePortID = rcuMemXml.attribute(XML_InPort_Index).toInt();
                            int sourcePeID = 0;
                            QPair<BasePe::SIndex, int> sindexPair = {{bcuIndexInRca, rcuIndexInBcu, sourcePeID}, sourcePePortID};
                            type2SIndex(type, sindexPair);

                            int inputPortID = rcuMemXml.attribute(XML_Attr_Id).toInt();
                            outfifoElement->m_inputSource.replace(inputPortID, sindexPair);
                        }
                        else if(rcuMemXml.tagName() == XML_RCU_LOOP)
                        {
                            rcuElement->setLoopStartEndFlag(rcuMemXml.attribute(XML_RCU_LOOP_START_END).toInt());
                            rcuElement->setLoopTimes(rcuMemXml.attribute(XML_RCU_LOOP_TIMES).toInt());
                            rcuElement->setLoopStartEndFlag2(rcuMemXml.attribute(XML_RCU_LOOP_START_END2).toInt());
                            rcuElement->setLoopTimes2(rcuMemXml.attribute(XML_RCU_LOOP_TIMES2).toInt());
                        }
                    }

                    QL_DEBUG << rcuXml.tagName() << rcuXml.attribute(XML_Attr_Id).toInt();
                    QDomNodeList arithUnitlistXml = rcuXml.childNodes();
                    for(int arithIndex = 0; arithIndex < arithUnitlistXml.count(); arithIndex++)
                    {
                        QDomElement arithUnitXml = arithUnitlistXml.at(arithIndex).toElement();
                        if(arithUnitXml.tagName() == mapArithUnitName.value(BFU))
                        {
                            if(arithUnitXml.attribute(XML_Attr_Id).toInt() < BFU_MAX_COUNT)
                            {
                                int indexInRcu = arithUnitXml.attribute(XML_Attr_Id).toInt();

                                if(rcuElement->findBfu({bcuIndexInRca,rcuIndexInBcu,indexInRcu}) != nullptr)
                                    continue;

                                CBfuElement* bfuElement = new CBfuElement;
                                rcuElement->addBfuElement(bfuElement);
                                bfuElement->setIndex({bcuIndexInRca,rcuIndexInBcu,indexInRcu});
                                bfuElement->funcIndex = CBfuElement::BfuFuncIndex(mapBfuFunc.key(arithUnitXml.attribute(XML_Bfu_Func), Func_Au_Index));
                                bfuElement->funcExp = arithUnitXml.attribute(XML_Bfu_Func_Exp);
                                bfuElement->funcAuModIndex = arithUnitXml.attribute(XML_Bfu_Au_Mod).toInt();
                                bfuElement->funcAuCarry = arithUnitXml.attribute(XML_Bfu_Au_Carry).toInt() == 1;
                                bfuElement->funcMuModIndex = arithUnitXml.attribute(XML_Bfu_Mu_Mod).toInt();
                                bfuElement->bypassIndex = arithUnitXml.attribute(XML_Bfu_Bypass).toInt();
                                bfuElement->bypassExp = arithUnitXml.attribute(XML_Bfu_Bypass_Exp);

                                QDomNodeList bfuChildListXml = arithUnitXml.childNodes();
                                for(int bfuChildIndex = 0; bfuChildIndex < bfuChildListXml.count(); bfuChildIndex++)
                                {
                                    QDomElement bfuChildXml = bfuChildListXml.at(bfuChildIndex).toElement();
                                    int inputPortID = 0;
                                    if(bfuChildXml.tagName() == XML_Bfu_InputA)
                                        inputPortID = 0;
                                    else if(bfuChildXml.tagName() == XML_Bfu_InputB)
                                        inputPortID = 1;
                                    else if(bfuChildXml.tagName() == XML_Bfu_InputT)
                                        inputPortID = 2;
                                    else
                                        continue;

                                    InputPortType type = InputPortType(bfuChildXml.attribute(XML_InPort_Type).toInt());
                                    int sourcePePortID = bfuChildXml.attribute(XML_InPort_Index).toInt();
                                    int sourcePeID = 0;
                                    QPair<BasePe::SIndex, int> sindexPair = {{bcuIndexInRca, rcuIndexInBcu, sourcePeID}, sourcePePortID};
                                    type2SIndex(type, sindexPair);

                                    bfuElement->m_inputSource.replace(inputPortID, sindexPair);
                                }
                            }
                        }
                        else if(arithUnitXml.tagName() == mapArithUnitName.value(SBOX))
                        {
                            if(arithUnitXml.attribute(XML_Attr_Id).toInt() < SBOX_MAX_COUNT)
                            {
                                int indexInRcu = arithUnitXml.attribute(XML_Attr_Id).toInt();
                                CBaseElement* baseElement = rcuElement->findSbox({bcuIndexInRca,rcuIndexInBcu,/*indexInRcu+*/BFU_COUNT});
                                CSboxElement* sboxElement = nullptr;
                                if(baseElement == nullptr)
                                {
                                    sboxElement = new CSboxElement;
                                    rcuElement->addSboxElement(sboxElement);
                                    sboxElement->setIndex({bcuIndexInRca,rcuIndexInBcu, BFU_COUNT/*+indexInRcu*/});
                                }
                                else
                                {
                                    sboxElement = dynamic_cast<CSboxElement*>(baseElement);
                                }
                                if(sboxElement == nullptr)
                                    continue;

                                sboxElement->srcConfig.replace(indexInRcu, projectParameter->resourceMap.value(arithUnitXml.attribute(XML_Sbox_Src)));
                                sboxElement->sboxMode.replace(indexInRcu, arithUnitXml.attribute(XML_Sbox_Mode).toInt());
                                sboxElement->sboxGroup.replace(indexInRcu, arithUnitXml.attribute(XML_Sbox_Group).toInt());
                                sboxElement->sboxByte.replace(indexInRcu, arithUnitXml.attribute(XML_Sbox_ByteSel).toInt());

                                QDomNodeList sboxChildListXml = arithUnitXml.childNodes();
                                for(int sboxChildIndex = 0; sboxChildIndex<sboxChildListXml.count(); sboxChildIndex++)
                                {
                                    QDomElement sboxChildXml = sboxChildListXml.at(sboxChildIndex).toElement();
                                    if(sboxChildXml.tagName() == XML_Sbox_Input)
                                    {
                                        InputPortType type = InputPortType(sboxChildXml.attribute(XML_InPort_Type).toInt());
                                        int sourcePePortID = sboxChildXml.attribute(XML_InPort_Index).toInt();
                                        int sourcePeID = 0;
                                        QPair<BasePe::SIndex, int> sindexPair = {{bcuIndexInRca, rcuIndexInBcu, sourcePeID}, sourcePePortID};
                                        type2SIndex(type, sindexPair);

                                        sboxElement->m_inputSource.replace(indexInRcu, sindexPair);
                                    }
                                }
                            }
                        }
                        else if(arithUnitXml.tagName() == mapArithUnitName.value(BENES))
                        {
                            if(arithUnitXml.attribute(XML_Attr_Id).toInt() < BENES_MAX_COUNT)
                            {
                                int indexInRcu = arithUnitXml.attribute(XML_Attr_Id).toInt();
                                if(rcuElement->findBenes({bcuIndexInRca,rcuIndexInBcu,BFU_COUNT+SBOX_COUNT+indexInRcu}) != nullptr)
                                    continue;

                                CBenesElement* benesElement = new CBenesElement;
                                rcuElement->addBenesElement(benesElement);
                                benesElement->setIndex({bcuIndexInRca,rcuIndexInBcu, BFU_COUNT+SBOX_COUNT+indexInRcu});
                                benesElement->srcConfig = projectParameter->resourceMap.value(arithUnitXml.attribute(XML_Benes_Src));

                                QDomNodeList benesChildListXml = arithUnitXml.childNodes();
                                for(int benesChildIndex=0; benesChildIndex<benesChildListXml.count(); benesChildIndex++)
                                {
                                    QDomElement benesChildXml = benesChildListXml.at(benesChildIndex).toElement();
                                    if(benesChildXml.tagName() == XML_Benes_Input)
                                    {
                                        InputPortType type = InputPortType(benesChildXml.attribute(XML_InPort_Type).toInt());
                                        int sourcePePortID = benesChildXml.attribute(XML_InPort_Index).toInt();
                                        int sourcePeID = 0;
                                        QPair<BasePe::SIndex, int> sindexPair = {{bcuIndexInRca, rcuIndexInBcu, sourcePeID}, sourcePePortID};
                                        type2SIndex(type, sindexPair);

                                        int inputPortID = benesChildXml.attribute(XML_Attr_Id).toInt();
                                        benesElement->m_inputSource.replace(inputPortID, sindexPair);

                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool CXmlResolver::initXmlToScene(RcaGraphScene &rcaScene)
{
    QDomNodeList bcuList = m_rca.childNodes();

    QL_DEBUG << bcuList.count();
    for(int bcuIndex = 0; bcuIndex<bcuList.count(); bcuIndex++)
    {
        QDomElement bcuXml = bcuList.at(bcuIndex).toElement();
        if((bcuXml.tagName() == mapModuleUnitName.value(Module_Bcu))
            && (bcuXml.attribute(XML_Attr_Id).toInt() < BCU_MAX_COUNT))
        {
            int bcuIndexInRca = bcuXml.attribute(XML_Attr_Id).toInt();

            QPointF posAtSecne = QPointF(bcuXml.attribute(XML_XAtParent).toInt(),
                                         bcuXml.attribute(XML_YAtParent).toInt());
            QRectF bcuRect = QRectF(bcuXml.attribute(XML_X).toInt(),
                                    bcuXml.attribute(XML_Y).toInt(),
                                    bcuXml.attribute(XML_Width).toInt(),
                                    bcuXml.attribute(XML_Height).toInt());

            if(rcaScene.findBcuFromScene(bcuIndexInRca) != nullptr) continue;

            ModuleBcu * moduleBcu = new ModuleBcu(bcuIndexInRca);
            moduleBcu->setRealBoundingRect(bcuRect);
            moduleBcu->srcDataFromBcu = ModuleBcuIndex(bcuXml.attribute(XML_BCU_DATA_SRC).toInt());
            moduleBcu->setSort(bcuXml.attribute(XML_BCU_DATA_SORT).toInt());
            moduleBcu->burst = bcuXml.attribute(XML_BCU_DATA_BURST).toInt();
            moduleBcu->gap = bcuXml.attribute(XML_BCU_DATA_GAP).toInt();
            moduleBcu->infifoSrc = bcuXml.attribute(XML_BCU_DATA_INFIFOSRC).toInt();
            moduleBcu->outfifoSrc = bcuXml.attribute(XML_BCU_DATA_OUTFIFOSRC).toInt();
            moduleBcu->lfsrGroup = bcuXml.attribute(XML_BCU_DATA_LFSRGROUP).toInt();
            moduleBcu->readImdMode128_32 = bcuXml.attribute(XML_BCU_RIMD_MODE128_32).toInt();
            moduleBcu->memorySrcConfig = bcuXml.attribute(XML_BCU_MEM_SRC);
            moduleBcu->inFifoSrcConfig = bcuXml.attribute(XML_BCU_INFIFO_SRC);
            moduleBcu->imdSrcConfig = bcuXml.attribute(XML_BCU_IMD_SRC);
            moduleBcu->writeDepth = bcuXml.attribute(XML_BCU_LFSR_DEPTH).toInt();
            moduleBcu->writeWidth = bcuXml.attribute(XML_BCU_LFSR_WIDTH).toInt();
            moduleBcu->addressMode = bcuXml.attribute(XML_BCU_LFSR_ADDRESSMODE);
            if(moduleBcu->addressMode.length() != 8)
                moduleBcu->addressMode = "00000000";

            if(false == rcaScene.addBcuFromXml(posAtSecne,moduleBcu))
            {
                delete moduleBcu;
            }
            else
            {
                QL_DEBUG << bcuXml.tagName() << bcuXml.attribute(XML_Attr_Id).toInt();
                QDomNodeList rcuListXml = bcuXml.childNodes();
                for(int rcuIndex = 0; rcuIndex<rcuListXml.count(); rcuIndex++)
                {
                    QDomElement rcuXml = rcuListXml.at(rcuIndex).toElement();
                    if((rcuXml.tagName() == mapModuleUnitName.value(Module_Rcu))
                        && (rcuXml.attribute(XML_Attr_Id).toInt() < RCU_MAX_COUNT))
                    {
                        int rcuIndexInBcu = rcuXml.attribute(XML_Attr_Id).toInt();
                        QPointF posAtBcu = QPointF(rcuXml.attribute(XML_XAtParent).toInt(),
                                                   rcuXml.attribute(XML_YAtParent).toInt());
                        QRectF rcuRcet = QRectF(rcuXml.attribute(XML_X).toInt(),
                                                rcuXml.attribute(XML_Y).toInt(),
                                                rcuXml.attribute(XML_Width).toInt(),
                                                rcuXml.attribute(XML_Height).toInt());

                        if(rcaScene.findRcuFromScene(bcuIndexInRca,rcuIndexInBcu) != nullptr) continue;

                        ModuleRcu* moduleRcu = new ModuleRcu(bcuIndexInRca,rcuIndexInBcu);
                        moduleRcu->setRealBoundingRect(rcuRcet);

                        //zhangjun 增加于20201210，适配CGRA RPU，fifo移至RCU
                        QDomNodeList rcuOutFifoListXml = rcuXml.childNodes();
                        for(int outFifoIndex = 0; outFifoIndex < rcuOutFifoListXml.count(); outFifoIndex++)
                        {
                            QDomElement rcuOutFifoXml = rcuOutFifoListXml.at(outFifoIndex).toElement();
                            if(rcuOutFifoXml.tagName() == XML_BCU_OUT_FIFO)
                            {
                                int outputId = rcuOutFifoXml.attribute(XML_Attr_Id).toInt();
                                switch(outputId)
                                {
                                case 0:
                                {
                                    //                                moduleBcu->outFifo0->setInputType(InputPortType(bcuOutFifoXml.attribute(XML_InPort_Type).toInt()));
                                    //                                moduleBcu->outFifo0->setInputIndex(bcuOutFifoXml.attribute(XML_InPort_Index).toInt());
                                    readXmlToPort(moduleRcu->outFifo0,&rcuOutFifoXml);
                                    break;
                                }
                                case 1:
                                {
                                    //                                moduleBcu->outFifo1->setInputType(InputPortType(bcuOutFifoXml.attribute(XML_InPort_Type).toInt()));
                                    //                                moduleBcu->outFifo1->setInputIndex(bcuOutFifoXml.attribute(XML_InPort_Index).toInt());
                                    readXmlToPort(moduleRcu->outFifo1,&rcuOutFifoXml);
                                    break;
                                }
                                case 2:
                                {
                                    //                                moduleBcu->outFifo2->setInputType(InputPortType(bcuOutFifoXml.attribute(XML_InPort_Type).toInt()));
                                    //                                moduleBcu->outFifo2->setInputIndex(bcuOutFifoXml.attribute(XML_InPort_Index).toInt());
                                    readXmlToPort(moduleRcu->outFifo2,&rcuOutFifoXml);
                                    break;
                                }
                                case 3:
                                {
                                    //                                moduleBcu->outFifo3->setInputType(InputPortType(bcuOutFifoXml.attribute(XML_InPort_Type).toInt()));
                                    //                                moduleBcu->outFifo3->setInputIndex(bcuOutFifoXml.attribute(XML_InPort_Index).toInt());
                                    readXmlToPort(moduleRcu->outFifo3,&rcuOutFifoXml);
                                    break;
                                }
                                default:break;
                                }
                            }
                        }
                        // zhangjun modified end

                        QDomNodeList rcuWRMemListXml = rcuXml.childNodes();
                        for(int rcuWRMemIndex = 0; rcuWRMemIndex<rcuWRMemListXml.count(); rcuWRMemIndex++)
                        {
                            QDomElement rcuMemXml = rcuWRMemListXml.at(rcuWRMemIndex).toElement();
                            if(rcuMemXml.tagName() == XML_RCU_LOOP)
                            {
                                moduleRcu->loopStartEndFlag = rcuMemXml.attribute(XML_RCU_LOOP_START_END).toInt();
                                moduleRcu->loopTimes = rcuMemXml.attribute(XML_RCU_LOOP_TIMES).toInt();
                                moduleRcu->loopStartEndFlag2 = rcuMemXml.attribute(XML_RCU_LOOP_START_END2).toInt();
                                moduleRcu->loopTimes2 = rcuMemXml.attribute(XML_RCU_LOOP_TIMES2).toInt();

                            }
                            else if(rcuMemXml.tagName() == XML_RCU_WMEM_NAME)
                            {
                                moduleRcu->writeMemOffset = rcuMemXml.attribute(XML_RCU_WMEM_OFFSET).toInt();
                                moduleRcu->writeMemMask = rcuMemXml.attribute(XML_RCU_WMEM_MASK).toInt();
                                moduleRcu->writeMemMode = rcuMemXml.attribute(XML_RCU_WMEM_MODE).toInt();
                                moduleRcu->writeMemThreashold = rcuMemXml.attribute(XML_RCU_WMEM_THREASHOLD).toInt();
                                moduleRcu->writeMemAddr = rcuMemXml.attribute(XML_RCU_WMEM_ADDR).toInt();
                            }
                            else if(rcuMemXml.tagName() == XML_RCU_RMEM_NAME)
                            {
                                moduleRcu->readMemOffset = rcuMemXml.attribute(XML_RCU_RMEM_OFFSET).toInt();

                                moduleRcu->readMemMode = rcuMemXml.attribute(XML_RCU_RMEM_MODE).toInt();
                                moduleRcu->readMemMode128_32 = rcuMemXml.attribute(XML_RCU_RMEM_MODE128_32).toInt();
                                moduleRcu->readMemThreashold = rcuMemXml.attribute(XML_RCU_RMEM_THREASHOLD).toInt();
                                moduleRcu->readMemAddr1 = rcuMemXml.attribute(XML_RCU_RMEM_ADDR1).toInt();
                                moduleRcu->readMemAddr2 = rcuMemXml.attribute(XML_RCU_RMEM_ADDR2).toInt();
                                moduleRcu->readMemAddr3 = rcuMemXml.attribute(XML_RCU_RMEM_ADDR3).toInt();
                                moduleRcu->readMemAddr4 = rcuMemXml.attribute(XML_RCU_RMEM_ADDR4).toInt();
                            }
                            else if(rcuMemXml.tagName() == XML_RCU_RCH_NAME)
                            {
                                moduleRcu->rchMode = rcuMemXml.attribute(XML_RCU_RCH_MODE).toInt();
                                moduleRcu->writeRchAddr1 = rcuMemXml.attribute(XML_RCU_RCH_ADDR1).toInt();
                                moduleRcu->writeRchAddr2 = rcuMemXml.attribute(XML_RCU_RCH_ADDR2).toInt();
                                moduleRcu->writeRchAddr3 = rcuMemXml.attribute(XML_RCU_RCH_ADDR3).toInt();
                                moduleRcu->writeRchAddr4 = rcuMemXml.attribute(XML_RCU_RCH_ADDR4).toInt();
                            }
                            else if(rcuMemXml.tagName() == XML_RCU_MEM_INPUT_NAME)
                            {

                            }
                        }

                        if(false == rcaScene.addRcuFromXml(posAtBcu,moduleRcu))
                        {
                            delete moduleRcu;
                        }
                        else
                        {
                            QL_DEBUG << rcuXml.tagName() << rcuXml.attribute(XML_Attr_Id).toInt();
                            QDomNodeList arithUnitlistXml = rcuXml.childNodes();
                            for(int arithIndex = 0; arithIndex < arithUnitlistXml.count(); arithIndex++)
                            {
                                QDomElement arithUnitXml = arithUnitlistXml.at(arithIndex).toElement();
                                if(arithUnitXml.tagName() == mapArithUnitName.value(BFU))
                                {
                                    if(arithUnitXml.attribute(XML_Attr_Id).toInt() < BFU_MAX_COUNT)
                                    {
                                        int indexInRcu = arithUnitXml.attribute(XML_Attr_Id).toInt();
                                        QPointF posAtRcu = QPointF(arithUnitXml.attribute(XML_XAtParent).toInt(),
                                                                   arithUnitXml.attribute(XML_YAtParent).toInt());

                                        if(rcaScene.findBfuFromScene(bcuIndexInRca,rcuIndexInBcu,indexInRcu) != nullptr) continue;

                                        ElementBfu* elementBfu = new ElementBfu(bcuIndexInRca,rcuIndexInBcu,indexInRcu);

                                        elementBfu->funcIndex = BfuFuncIndex(mapBfuFunc.key(arithUnitXml.attribute(XML_Bfu_Func),Func_Au_Index));
                                        elementBfu->funcExp = arithUnitXml.attribute(XML_Bfu_Func_Exp);
                                        elementBfu->funcAuModIndex = arithUnitXml.attribute(XML_Bfu_Au_Mod).toInt();
                                        elementBfu->funcAuCarry = arithUnitXml.attribute(XML_Bfu_Au_Carry).toInt() == 1;
                                        elementBfu->funcMuModIndex = arithUnitXml.attribute(XML_Bfu_Mu_Mod).toInt();
                                        elementBfu->bypassIndex = arithUnitXml.attribute(XML_Bfu_Bypass).toInt();
                                        elementBfu->bypassExp = arithUnitXml.attribute(XML_Bfu_Bypass_Exp);

                                        QDomNodeList bfuChildListXml = arithUnitXml.childNodes();
                                        for(int bfuChildIndex=0; bfuChildIndex < bfuChildListXml.count(); bfuChildIndex++)
                                        {
                                            QDomElement bfuChildXml = bfuChildListXml.at(bfuChildIndex).toElement();
                                            if(bfuChildXml.tagName() == XML_Bfu_InputA)
                                            {
                                                //                                        elementBfu->inPortA->setInputType(InputPortType(bfuChildXml.attribute(XML_InPort_Type).toInt()));
                                                //                                        elementBfu->inPortA->setInputIndex(bfuChildXml.attribute(XML_InPort_Index).toInt());
                                                readXmlToPort(elementBfu->inPortA,&bfuChildXml);
                                            }
                                            else if(bfuChildXml.tagName() == XML_Bfu_InputB)
                                            {
                                                //                                        elementBfu->inPortB->setInputType(InputPortType(bfuChildXml.attribute(XML_InPort_Type).toInt()));
                                                //                                        elementBfu->inPortB->setInputIndex(bfuChildXml.attribute(XML_InPort_Index).toInt());
                                                readXmlToPort(elementBfu->inPortB,&bfuChildXml);
                                            }
                                            else if(bfuChildXml.tagName() == XML_Bfu_InputT)
                                            {
                                                //                                        elementBfu->inPortT->setInputType(InputPortType(bfuChildXml.attribute(XML_InPort_Type).toInt()));
                                                //                                        elementBfu->inPortT->setInputIndex(bfuChildXml.attribute(XML_InPort_Index).toInt());
                                                readXmlToPort(elementBfu->inPortT,&bfuChildXml);
                                            }
                                        }


                                        if(false == rcaScene.addBfuFromXml(posAtRcu,elementBfu))
                                        {
                                            delete elementBfu;
                                        }


                                        QL_DEBUG << arithUnitXml.tagName() << arithUnitXml.attribute(XML_Attr_Id).toInt();
                                    }
                                }
                                else if(arithUnitXml.tagName() == mapArithUnitName.value(CUSTOMPE))
                                {
                                    if(arithUnitXml.attribute(XML_Attr_Id).toInt() < BFU_MAX_COUNT)
                                    {
                                        int indexInRcu = arithUnitXml.attribute(XML_Attr_Id).toInt();
                                        QPointF posAtRcu = QPointF(arithUnitXml.attribute(XML_XAtParent).toInt(),
                                                                   arithUnitXml.attribute(XML_YAtParent).toInt());

                                        if(rcaScene.findCustomPEFromScene(bcuIndexInRca,rcuIndexInBcu,indexInRcu) != nullptr) continue;

                                        ElementCustomPE* elementCustomPE = new ElementCustomPE(bcuIndexInRca,rcuIndexInBcu,indexInRcu);

                                        elementCustomPE->funcIndex = BfuFuncIndex(mapBfuFunc.key(arithUnitXml.attribute(XML_CustomPE_Func),Func_Au_Index));
                                        elementCustomPE->funcExp = arithUnitXml.attribute(XML_CustomPE_Func_Exp);
                                        elementCustomPE->funcAuModIndex = arithUnitXml.attribute(XML_CustomPE_Au_Mod).toInt();
//                                        elementBfu->funcAuCarry = arithUnitXml.attribute(XML_CustomPE_Au_Carry).toInt() == 1;
//                                        elementBfu->funcMuModIndex = arithUnitXml.attribute(XML_CustomPE_Mu_Mod).toInt();
                                        elementCustomPE->bypassIndex = arithUnitXml.attribute(XML_CustomPE_Bypass).toInt();
                                        elementCustomPE->bypassExp = arithUnitXml.attribute(XML_CustomPE_Bypass_Exp);

                                        int iInputPortNum = arithUnitXml.attribute(XML_CustomPE_InputNum).toInt();
                                        elementCustomPE->setInputNum(iInputPortNum);
                                        elementCustomPE->changeInputPort(iInputPortNum);
                                        int iOutputPortNum = arithUnitXml.attribute(XML_CustomPE_OutputNum).toInt();
                                        elementCustomPE->setOutputNum(iOutputPortNum);
                                        elementCustomPE->changeOutputPort(iOutputPortNum);

                                        QDomNodeList customPEChildListXml = arithUnitXml.childNodes();
                                        for(int customPEChildIndex=0; customPEChildIndex<customPEChildListXml.count(); customPEChildIndex++)
                                        {
                                            QDomElement customPEChildXml = customPEChildListXml.at(customPEChildIndex).toElement();
                                            if(customPEChildXml.tagName() == XML_CustomPE_Input)
                                            {
                                                int inputId = customPEChildXml.attribute(XML_Attr_Id).toInt();
                                                if(iInputPortNum < inputId + 1)
                                                    continue;
                                                switch(inputId)
                                                {
                                                case 0:
                                                {
                                                    readXmlToPort(elementCustomPE->m_inputPortVector.at(inputId),&customPEChildXml);
                                                    break;
                                                }
                                                case 1:
                                                {
                                                    readXmlToPort(elementCustomPE->m_inputPortVector.at(inputId),&customPEChildXml);
                                                    break;
                                                }
                                                case 2:
                                                {
                                                    readXmlToPort(elementCustomPE->m_inputPortVector.at(inputId),&customPEChildXml);
                                                    break;
                                                }
                                                case 3:
                                                {
                                                    readXmlToPort(elementCustomPE->m_inputPortVector.at(inputId),&customPEChildXml);
                                                    break;
                                                }
                                                default:break;
                                                }
                                            }
                                        }


                                        if(false == rcaScene.addCustomPEFromXml(posAtRcu,elementCustomPE))
                                        {
                                            delete elementCustomPE;
                                        }


                                        QL_DEBUG << arithUnitXml.tagName() << arithUnitXml.attribute(XML_Attr_Id).toInt();
                                    }
                                }
                                else if(arithUnitXml.tagName() == mapArithUnitName.value(SBOX))
                                {
                                    if(arithUnitXml.attribute(XML_Attr_Id).toInt() < SBOX_MAX_COUNT)
                                    {
                                        int indexInRcu = arithUnitXml.attribute(XML_Attr_Id).toInt();
                                        QPointF posAtRcu = QPointF(arithUnitXml.attribute(XML_XAtParent).toInt(),
                                                                   arithUnitXml.attribute(XML_YAtParent).toInt());

                                        if(rcaScene.findSboxFromScene(bcuIndexInRca,rcuIndexInBcu,indexInRcu) != nullptr) continue;

                                        ElementSbox* elementSbox = new ElementSbox(bcuIndexInRca,rcuIndexInBcu,indexInRcu);

                                        elementSbox->srcConfig = arithUnitXml.attribute(XML_Sbox_Src);
                                        elementSbox->sboxMode = arithUnitXml.attribute(XML_Sbox_Mode).toInt();
                                        elementSbox->sboxGroup = arithUnitXml.attribute(XML_Sbox_Group).toInt();
                                        elementSbox->sboxByteSel = arithUnitXml.attribute(XML_Sbox_ByteSel).toInt();
                                        elementSbox->sboxIncreaseMode = arithUnitXml.attribute(XML_Sbox_IncreaseMode).toInt();

                                        QDomNodeList sboxChildListXml = arithUnitXml.childNodes();
                                        for(int sboxChildIndex=0; sboxChildIndex<sboxChildListXml.count(); sboxChildIndex++)
                                        {
                                            QDomElement sboxChildXml = sboxChildListXml.at(sboxChildIndex).toElement();
                                            if(sboxChildXml.tagName() == XML_Sbox_Input)
                                            {
                                                //                                        elementSbox->inPort0->setInputType(InputPortType(sboxChildXml.attribute(XML_InPort_Type).toInt()));
                                                //                                        elementSbox->inPort0->setInputIndex(sboxChildXml.attribute(XML_InPort_Index).toInt());
                                                readXmlToPort(elementSbox->inPort0,&sboxChildXml);
                                            }
                                            else if(sboxChildXml.tagName() == XML_Sbox_Cnt)
                                            {
                                                int cntId = sboxChildXml.attribute(XML_Attr_Id).toInt();
                                                switch(cntId)
                                                {
                                                case 0: elementSbox->groupCnt0 = SboxGroupValue(sboxChildXml.attribute(XML_Sbox_Cnt_Value).toInt());break;
                                                case 1: elementSbox->groupCnt1 = SboxGroupValue(sboxChildXml.attribute(XML_Sbox_Cnt_Value).toInt());break;
                                                case 2: elementSbox->groupCnt2 = SboxGroupValue(sboxChildXml.attribute(XML_Sbox_Cnt_Value).toInt());break;
                                                case 3: elementSbox->groupCnt3 = SboxGroupValue(sboxChildXml.attribute(XML_Sbox_Cnt_Value).toInt());break;
                                                default:break;
                                                }
                                            }
                                        }

                                        if(false == rcaScene.addSboxFromXml(posAtRcu,elementSbox))
                                        {
                                            delete elementSbox;
                                        }

                                        QL_DEBUG << arithUnitXml.tagName() << arithUnitXml.attribute(XML_Attr_Id).toInt();
                                    }
                                }
                                else if(arithUnitXml.tagName() == mapArithUnitName.value(BENES))
                                {
                                    if(arithUnitXml.attribute(XML_Attr_Id).toInt() < BENES_MAX_COUNT)
                                    {
                                        int indexInRcu = arithUnitXml.attribute(XML_Attr_Id).toInt();
                                        QPointF posAtRcu = QPointF(arithUnitXml.attribute(XML_XAtParent).toInt(),
                                                                   arithUnitXml.attribute(XML_YAtParent).toInt());

                                        if(rcaScene.findBenesFromScene(bcuIndexInRca,rcuIndexInBcu,indexInRcu) != nullptr) continue;

                                        ElementBenes* elementBenes = new ElementBenes(bcuIndexInRca,rcuIndexInBcu,indexInRcu);

                                        elementBenes->srcConfig = arithUnitXml.attribute(XML_Benes_Src);

                                        QDomNodeList benesChildListXml = arithUnitXml.childNodes();
                                        for(int benesChildIndex=0; benesChildIndex<benesChildListXml.count(); benesChildIndex++)
                                        {
                                            QDomElement benesChildXml = benesChildListXml.at(benesChildIndex).toElement();
                                            if(benesChildXml.tagName() == XML_Benes_Input)
                                            {
                                                int inputId = benesChildXml.attribute(XML_Attr_Id).toInt();
                                                switch(inputId)
                                                {
                                                case 0:
                                                {
                                                    //                                                    elementBenes->inPort0->setInputType(InputPortType(benesChildXml.attribute(XML_InPort_Type).toInt()));
                                                    //                                                    elementBenes->inPort0->setInputIndex(benesChildXml.attribute(XML_InPort_Index).toInt());
                                                    readXmlToPort(elementBenes->inPort0,&benesChildXml);
                                                    break;
                                                }
                                                case 1:
                                                {
                                                    //                                                    elementBenes->inPort1->setInputType(InputPortType(benesChildXml.attribute(XML_InPort_Type).toInt()));
                                                    //                                                    elementBenes->inPort1->setInputIndex(benesChildXml.attribute(XML_InPort_Index).toInt());
                                                    readXmlToPort(elementBenes->inPort1,&benesChildXml);
                                                    break;
                                                }
                                                case 2:
                                                {
                                                    //                                                    elementBenes->inPort2->setInputType(InputPortType(benesChildXml.attribute(XML_InPort_Type).toInt()));
                                                    //                                                    elementBenes->inPort2->setInputIndex(benesChildXml.attribute(XML_InPort_Index).toInt());
                                                    readXmlToPort(elementBenes->inPort2,&benesChildXml);
                                                    break;
                                                }
                                                case 3:
                                                {
                                                    //                                                    elementBenes->inPort3->setInputType(InputPortType(benesChildXml.attribute(XML_InPort_Type).toInt()));
                                                    //                                                    elementBenes->inPort3->setInputIndex(benesChildXml.attribute(XML_InPort_Index).toInt());
                                                    readXmlToPort(elementBenes->inPort3,&benesChildXml);
                                                    break;
                                                }
                                                default:break;
                                                }
                                            }
                                        }

                                        if(false == rcaScene.addBenesFromXml(posAtRcu,elementBenes))
                                        {
                                            delete elementBenes;
                                        }
                                        QL_DEBUG << arithUnitXml.tagName() << arithUnitXml.attribute(XML_Attr_Id).toInt();
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    //add write mem to scene

    for(int bcuIndex = 0; bcuIndex < BCU_MAX_COUNT; bcuIndex++)
    {
        bool bExist = false;
        for(int rcuIndex = 0; rcuIndex < RCU_MAX_COUNT; rcuIndex++)
        {
            bExist = isHadRcuInXml(bcuIndex,rcuIndex);
            if(!bExist)
                continue;
            for(int writeRchIndex = 0; writeRchIndex < WRITE_RCH_MAX_COUNT; writeRchIndex++)
            {
                QL_DEBUG;
                if(isHadWriteRchInXml(bcuIndex,rcuIndex,writeRchIndex))
                {
                    QDomElement xmlWriteRch = findRCURCHElement(bcuIndex,rcuIndex,writeRchIndex);

                    if(xmlWriteRch.attribute(XML_InPort_Type).isEmpty()) continue;
                    //                    if(xmlWriteMem.attribute(XML_InPort_Index).isEmpty()) continue;

                    InputPortType inputType = InputPortType(xmlWriteRch.attribute(XML_InPort_Type).toInt());
                    int inputIndex = xmlWriteRch.attribute(XML_InPort_Index).toInt();
                    int writeRchAddress = xmlWriteRch.attribute(XML_InPort_Address).toInt();
                    QL_DEBUG << inputType << inputIndex;
                    switch(inputType)
                    {
                    case InputPort_CurrentBfuX:
                    {
                        QL_DEBUG;
                        ElementBfu* elementBfu = rcaScene.findBfuFromScene(bcuIndex,rcuIndex,inputIndex);
                        if(elementBfu)
                        {
                            //验证成功，添加writeMem。
                            QL_DEBUG;
                            elementBfu->outPortX->addWriteRchFromXml(inputType,inputIndex,writeRchIndex,writeRchAddress,elementBfu);
                        }
                        break;
                    }
                    case InputPort_CurrentBfuY:
                    {
                        ElementBfu* elementBfu = rcaScene.findBfuFromScene(bcuIndex,rcuIndex,inputIndex);
                        if(elementBfu)
                        {
                            //验证成功，添加writeMem。
                            elementBfu->outPortY->addWriteRchFromXml(inputType,inputIndex,writeRchIndex,writeRchAddress,elementBfu);
                        }
                        break;
                    }
                    case InputPort_CurrentCustomPE:
                    {
                        ElementCustomPE* elementCustomPE = rcaScene.findCustomPEFromScene(bcuIndex,rcuIndex,inputIndex/CUSTOMPE_OUTPUTCOUNT);
                        if(elementCustomPE)
                        {
                            //验证成功，添加writeMem。
                            switch(inputIndex%CUSTOMPE_OUTPUTCOUNT)
                            {
                            case 0:elementCustomPE->m_outputPortVector.at(0)->addWriteRchFromXml(inputType,inputIndex,writeRchIndex,writeRchAddress,elementCustomPE);break;
                            case 1:elementCustomPE->m_outputPortVector.at(1)->addWriteRchFromXml(inputType,inputIndex,writeRchIndex,writeRchAddress,elementCustomPE);break;
                            case 2:elementCustomPE->m_outputPortVector.at(2)->addWriteRchFromXml(inputType,inputIndex,writeRchIndex,writeRchAddress,elementCustomPE);break;
                            case 3:elementCustomPE->m_outputPortVector.at(3)->addWriteRchFromXml(inputType,inputIndex,writeRchIndex,writeRchAddress,elementCustomPE);break;
                            default:break;
                            }
                        }
                        break;
                    }
                    case InputPort_CurrentSbox:
                    {
                        ElementSbox* elementSbox = rcaScene.findSboxFromScene(bcuIndex,rcuIndex,inputIndex/SBOX_OUTPUTCOUNT);
                        if(elementSbox)
                        {
                            //验证成功，添加writeMem。
                            switch(inputIndex%SBOX_OUTPUTCOUNT)
                            {
                            case 0:elementSbox->outPort0->addWriteRchFromXml(inputType,inputIndex,writeRchIndex,writeRchAddress,elementSbox);break;
                            case 1:elementSbox->outPort1->addWriteRchFromXml(inputType,inputIndex,writeRchIndex,writeRchAddress,elementSbox);break;
                            case 2:elementSbox->outPort2->addWriteRchFromXml(inputType,inputIndex,writeRchIndex,writeRchAddress,elementSbox);break;
                            case 3:elementSbox->outPort3->addWriteRchFromXml(inputType,inputIndex,writeRchIndex,writeRchAddress,elementSbox);break;
                            default:break;
                            }
                        }
                        break;
                    }
                    case InputPort_CurrentBenes:
                    {
                        ElementBenes* elementBenes = rcaScene.findBenesFromScene(bcuIndex,rcuIndex,inputIndex/BENES_OUTPUTCOUNT);
                        if(elementBenes)
                        {
                            //验证成功，添加writeMem。
                            switch(inputIndex%BENES_OUTPUTCOUNT)
                            {
                            case 0:elementBenes->outPort0->addWriteRchFromXml(inputType,inputIndex,writeRchIndex,writeRchAddress,elementBenes);break;
                            case 1:elementBenes->outPort1->addWriteRchFromXml(inputType,inputIndex,writeRchIndex,writeRchAddress,elementBenes);break;
                            case 2:elementBenes->outPort2->addWriteRchFromXml(inputType,inputIndex,writeRchIndex,writeRchAddress,elementBenes);break;
                            case 3:elementBenes->outPort3->addWriteRchFromXml(inputType,inputIndex,writeRchIndex,writeRchAddress,elementBenes);break;
                            default:break;
                            }
                        }
                        break;
                    }
                    default: break;
                    }
                }
            }

            for(int writeLfsrIndex = 0; writeLfsrIndex < WRITE_LFSR_MAX_COUNT; writeLfsrIndex++)
            {
                QL_DEBUG;
                if(isHadWriteLfsrInXml(bcuIndex,rcuIndex,writeLfsrIndex))
                {
                    QDomElement xmlWriteLfsr = findRCULFSRElement(bcuIndex,rcuIndex,writeLfsrIndex);

                    if(xmlWriteLfsr.attribute(XML_InPort_Type).isEmpty()) continue;

                    InputPortType inputType = InputPortType(xmlWriteLfsr.attribute(XML_InPort_Type).toInt());
                    int inputIndex = xmlWriteLfsr.attribute(XML_InPort_Index).toInt();
                    QL_DEBUG << inputType << inputIndex;
                    switch(inputType)
                    {
                    case InputPort_CurrentBfuX:
                    {
                        QL_DEBUG;
                        ElementBfu* elementBfu = rcaScene.findBfuFromScene(bcuIndex,rcuIndex,inputIndex);
                        if(elementBfu)
                        {
                            //验证成功，添加writeMem。
                            QL_DEBUG;
                            elementBfu->outPortX->addWriteLfsrFromXml(inputType,inputIndex,writeLfsrIndex,elementBfu);
                        }
                        break;
                    }
                    case InputPort_CurrentBfuY:
                    {
                        ElementBfu* elementBfu = rcaScene.findBfuFromScene(bcuIndex,rcuIndex,inputIndex);
                        if(elementBfu)
                        {
                            //验证成功，添加writeMem。
                            elementBfu->outPortY->addWriteLfsrFromXml(inputType,inputIndex,writeLfsrIndex,elementBfu);
                        }
                        break;
                    }
                    case InputPort_CurrentCustomPE:
                    {
                        ElementCustomPE* elementCustomPE = rcaScene.findCustomPEFromScene(bcuIndex,rcuIndex,inputIndex/CUSTOMPE_OUTPUTCOUNT);
                        if(elementCustomPE)
                        {
                            //验证成功，添加writeMem。
                            switch(inputIndex%CUSTOMPE_OUTPUTCOUNT)
                            {
                            case 0:elementCustomPE->m_outputPortVector.at(0)->addWriteLfsrFromXml(inputType,inputIndex,writeLfsrIndex,elementCustomPE);break;
                            case 1:elementCustomPE->m_outputPortVector.at(1)->addWriteLfsrFromXml(inputType,inputIndex,writeLfsrIndex,elementCustomPE);break;
                            case 2:elementCustomPE->m_outputPortVector.at(2)->addWriteLfsrFromXml(inputType,inputIndex,writeLfsrIndex,elementCustomPE);break;
                            case 3:elementCustomPE->m_outputPortVector.at(3)->addWriteLfsrFromXml(inputType,inputIndex,writeLfsrIndex,elementCustomPE);break;
                            default:break;
                            }
                        }
                        break;
                    }
                    case InputPort_CurrentSbox:
                    {
                        ElementSbox* elementSbox = rcaScene.findSboxFromScene(bcuIndex,rcuIndex,inputIndex/SBOX_OUTPUTCOUNT);
                        if(elementSbox)
                        {
                            //验证成功，添加writeMem。
                            switch(inputIndex%SBOX_OUTPUTCOUNT)
                            {
                            case 0:elementSbox->outPort0->addWriteLfsrFromXml(inputType,inputIndex,writeLfsrIndex,elementSbox);break;
                            case 1:elementSbox->outPort1->addWriteLfsrFromXml(inputType,inputIndex,writeLfsrIndex,elementSbox);break;
                            case 2:elementSbox->outPort2->addWriteLfsrFromXml(inputType,inputIndex,writeLfsrIndex,elementSbox);break;
                            case 3:elementSbox->outPort3->addWriteLfsrFromXml(inputType,inputIndex,writeLfsrIndex,elementSbox);break;
                            default:break;
                            }
                        }
                        break;
                    }
                    case InputPort_CurrentBenes:
                    {
                        ElementBenes* elementBenes = rcaScene.findBenesFromScene(bcuIndex,rcuIndex,inputIndex/BENES_OUTPUTCOUNT);
                        if(elementBenes)
                        {
                            //验证成功，添加writeMem。
                            switch(inputIndex%BENES_OUTPUTCOUNT)
                            {
                            case 0:elementBenes->outPort0->addWriteLfsrFromXml(inputType,inputIndex,writeLfsrIndex,elementBenes);break;
                            case 1:elementBenes->outPort1->addWriteLfsrFromXml(inputType,inputIndex,writeLfsrIndex,elementBenes);break;
                            case 2:elementBenes->outPort2->addWriteLfsrFromXml(inputType,inputIndex,writeLfsrIndex,elementBenes);break;
                            case 3:elementBenes->outPort3->addWriteLfsrFromXml(inputType,inputIndex,writeLfsrIndex,elementBenes);break;
                            default:break;
                            }
                        }
                        break;
                    }
                    default: break;
                    }
                }
            }

            for(int writeMemIndex = 0; writeMemIndex < WRITE_MEM_MAX_COUNT; writeMemIndex++)
            {
                QL_DEBUG;
                if(isHadWriteMemInXml(bcuIndex,rcuIndex,writeMemIndex))
                {
                    QDomElement xmlWriteMem = findRCUMEMElement(bcuIndex,rcuIndex,writeMemIndex);

                    if(xmlWriteMem.attribute(XML_InPort_Type).isEmpty()) continue;
                    //                    if(xmlWriteMem.attribute(XML_InPort_Index).isEmpty()) continue;

                    InputPortType inputType = InputPortType(xmlWriteMem.attribute(XML_InPort_Type).toInt());
                    int inputIndex = xmlWriteMem.attribute(XML_InPort_Index).toInt();
                    QL_DEBUG << inputType << inputIndex;
                    switch(inputType)
                    {
                    case InputPort_CurrentBfuX:
                    {
                        QL_DEBUG;
                        ElementBfu* elementBfu = rcaScene.findBfuFromScene(bcuIndex,rcuIndex,inputIndex);
                        if(elementBfu)
                        {
                            //验证成功，添加writeMem。
                            QL_DEBUG;
                            elementBfu->outPortX->addWriteMemFromXml(inputType,inputIndex,writeMemIndex,elementBfu);
                        }
                        break;
                    }
                    case InputPort_CurrentBfuY:
                    {
                        ElementBfu* elementBfu = rcaScene.findBfuFromScene(bcuIndex,rcuIndex,inputIndex);
                        if(elementBfu)
                        {
                            //验证成功，添加writeMem。
                            elementBfu->outPortY->addWriteMemFromXml(inputType,inputIndex,writeMemIndex,elementBfu);
                        }
                        break;
                    }
                    case InputPort_CurrentSbox:
                    {
                        ElementSbox* elementSbox = rcaScene.findSboxFromScene(bcuIndex,rcuIndex,inputIndex/SBOX_OUTPUTCOUNT);
                        if(elementSbox)
                        {
                            //验证成功，添加writeMem。
                            switch(inputIndex%SBOX_OUTPUTCOUNT)
                            {
                            case 0:elementSbox->outPort0->addWriteMemFromXml(inputType,inputIndex,writeMemIndex,elementSbox);break;
                            case 1:elementSbox->outPort1->addWriteMemFromXml(inputType,inputIndex,writeMemIndex,elementSbox);break;
                            case 2:elementSbox->outPort2->addWriteMemFromXml(inputType,inputIndex,writeMemIndex,elementSbox);break;
                            case 3:elementSbox->outPort3->addWriteMemFromXml(inputType,inputIndex,writeMemIndex,elementSbox);break;
                            default:break;
                            }
                        }
                        break;
                    }
                    case InputPort_CurrentBenes:
                    {
                        ElementBenes* elementBenes = rcaScene.findBenesFromScene(bcuIndex,rcuIndex,inputIndex/BENES_OUTPUTCOUNT);
                        if(elementBenes)
                        {
                            //验证成功，添加writeMem。
                            switch(inputIndex%BENES_OUTPUTCOUNT)
                            {
                            case 0:elementBenes->outPort0->addWriteMemFromXml(inputType,inputIndex,writeMemIndex,elementBenes);break;
                            case 1:elementBenes->outPort1->addWriteMemFromXml(inputType,inputIndex,writeMemIndex,elementBenes);break;
                            case 2:elementBenes->outPort2->addWriteMemFromXml(inputType,inputIndex,writeMemIndex,elementBenes);break;
                            case 3:elementBenes->outPort3->addWriteMemFromXml(inputType,inputIndex,writeMemIndex,elementBenes);break;
                            default:break;
                            }
                        }
                        break;
                    }
                    default: break;
                    }
                }
            }
        }
    }
    return isAutoResetPos;
}

// parse log, zhangjun add at 20200708
bool CXmlResolver::initLogToScene(RcaGraphScene &rcaScene, const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    struct SElementIndex
    {
        ElementType type = ELEMENT_NULL;
        int bcu = 0;
        int rcu = 0;
        int pe = 0;

    };

    static auto getElementIndex = [](const QString& type, QPair<int, int> coordinate)->SElementIndex {
        Q_ASSERT(coordinate.first >= 0);
//        Q_ASSERT(coordinate.second >= 0 && coordinate.second < 13);

        SElementIndex index;
        index.bcu = 0;
        index.rcu = coordinate.first;
        index.pe = coordinate.second;

        if(type == "bfu") {
            index.type = ELEMENT_BFU;
        } else if(type == "sbox") {
            index.type = ELEMENT_SBOX;
        } else if(type == "benes") {
            index.type = ELEMENT_BENES;
        } else if(type == "load") {
            index.type = ELEMENT_INMEM;
        } else if(type == "store") {
            index.type = ELEMENT_OUTMEM;
        } else if(type == "input") {
            index.type = ELEMENT_INFIFO;
        } else if(type == "output") {
            index.type = ELEMENT_OUTFIFO;
        } else if(type == "imd") {
            index.type = ELEMENT_INIMD;
        } else if(type == "sregr") {
            index.type = ELEMENT_INRCH;
        } else if(type == "sregw") {
            index.type = ELEMENT_OUTRCH;
        }

        if(index.type == ELEMENT_BFU || index.type == ELEMENT_SBOX || index.type == ELEMENT_BENES)
        {
            if(coordinate.second == 12)
                index.pe = coordinate.second - 12;
            else if(coordinate.second > 7)
                index.pe = coordinate.second - 8;
            else
                index.pe = coordinate.second;
        }

//        if(coordinate.first < 10)
//            index.bcu = 0;
//        else if(coordinate.first < 19)
//            index.bcu = 1;
//        else if(coordinate.first < 28)
//            index.bcu = 2;
//        else
//            index.bcu = 3;

//        switch (coordinate.first) {
//        case 1:
//        case 10:
//        case 19:
//        case 28:
//            index.rcu = index.pe = coordinate.second % 4;
//            break;
//        case 2:
//        case 3:
//        case 11:
//        case 12:
//        case 20:
//        case 21:
//        case 29:
//        case 30:
//            index.rcu = 0;
//            if(coordinate.second == 12)
//                index.pe = coordinate.second - 12;
//            else if(coordinate.second > 7)
//                index.pe = coordinate.second - 8;
//            else
//                index.pe = coordinate.second;
//            break;
//        case 4:
//        case 5:
//        case 13:
//        case 14:
//        case 22:
//        case 23:
//        case 31:
//        case 32:
//            index.rcu = 1;
//            if(coordinate.second == 12)
//                index.pe = coordinate.second - 12;
//            else if(coordinate.second > 7)
//                index.pe = coordinate.second - 8;
//            else
//                index.pe = coordinate.second;
//            break;
//        case 6:
//        case 7:
//        case 15:
//        case 16:
//        case 24:
//        case 25:
//        case 33:
//        case 34:
//            index.rcu = 2;
//            if(coordinate.second == 12)
//                index.pe = coordinate.second - 12;
//            else if(coordinate.second > 7)
//                index.pe = coordinate.second - 8;
//            else
//                index.pe = coordinate.second;
//            break;
//        case 8:
//        case 9:
//        case 17:
//        case 18:
//        case 26:
//        case 27:
//        case 35:
//        case 36:
//            index.rcu = 3;
//            if(coordinate.second == 12)
//                index.pe = coordinate.second - 12;
//            else if(coordinate.second > 7)
//                index.pe = coordinate.second - 8;
//            else
//                index.pe = coordinate.second;
//            break;
//        default:
//            break;
//        }

        return index;
    };

    auto stringListToMap = [](QStringList list, const QString &symbol)->QMap<QString, QString> {
        QMap<QString, QString> keyValueMap;
        foreach (const QString str, list) {
            QStringList strList = str.split(symbol);
            if(strList.count() != 2)
                continue;
            keyValueMap.insert(QString(strList.at(0)).trimmed().remove("\""), QString(strList.at(1)).trimmed().remove("\""));
        }
        return keyValueMap;
    };

    //文件全局参数
    QTextStream in(&file);
    QString line;
//    line = in.readLine();
//    if(!in.atEnd() && !line.startsWith("Loop="))
//        return false;

//    if(!in.atEnd()){
//        line = in.readLine();
//        int loopnum = line.right(line.length() - line.indexOf("=")).toInt();
//        Q_UNUSED(loopnum)
//        // FIXME LOOPNUM set loop num
//    }

    QMap<QString, SElementIndex> elementMap;
    bool isConnect = false;
    while(!in.atEnd())
    {
        line = in.readLine().trimmed();
        if(line.isEmpty())
            continue;

        int loop = 0, loopStart = 0, loopEnd = 0;
        if(line.startsWith("Loop="))
        {
            QStringList cellList = line.split(";");
            if(cellList.count() != 3)
                continue;

            QStringList cellList2 = QString(cellList.at(0)).trimmed().remove(";").split("=");
            if(cellList2.count() != 2)
                continue;
            if(cellList2.at(0) == "Loop")
                loop = cellList2.at(1).toInt();

            cellList2 = QString(cellList.at(1)).trimmed().remove(";").split("=");
            if(cellList2.count() != 2)
                continue;
            if(cellList2.at(0) == "loopStart")
                loopStart = cellList2.at(1).toInt();

            cellList2 = QString(cellList.at(2)).trimmed().remove(";").split("=");
            if(cellList2.count() != 2)
                continue;
            if(cellList2.at(0) == "loopEnd")
                loopEnd = cellList2.at(1).toInt();

            for(int i = loopStart; i <= loopEnd; ++i)
            {
                rcaScene.addNewRcuCfg(i);
            }
            auto* rcuStart = rcaScene.findRcuFromScene(0, loopStart);
            rcuStart->loopTimes = loop;
            if(loopStart == loopEnd) {
                rcuStart->loopStartEndFlag = 3;
            } else {
                rcuStart->loopStartEndFlag = 1;
                auto* rcuEnd = rcaScene.findRcuFromScene(0, loopEnd);
                rcuEnd->loopStartEndFlag = 2;
            }
            continue;
        }
        else if(line == "Operation Mapping Result:")
        {
            isConnect = false;
            continue;
        }
        else if(!isConnect && line == "Connection Mapping Result:")
        {
            isConnect = true;
//            QMap<QString, SElementIndex>::iterator i;
//            for (i = elementMap.begin(); i != elementMap.end(); ++i)
//                qDebug() << i.key() << "," << i.value().bcu << "," << i.value().rcu << ","
//                          << i.value().pe;
            continue;
        }

        if(!isConnect){  // 处理 "Operation Mapping Result:"
            QStringList cellList = line.split(":");
            if(cellList.count() < 3)
                continue;
            QString cell0 = cellList.at(0);
            QString cell2 = cellList.at(2);
            QString type = cell0.mid(cell0.indexOf("(") + 1, cell0.indexOf(")") - cell0.indexOf("(") - 1);
            QString key = cell0.left(cell0.indexOf("("));
            QStringList cell2List = cell2.left(cell2.indexOf(".")).remove("block_").split("_");
            if(cell2List.count() != 2)
                continue;

            QPair<int, int> coordinate = {QString(cell2List.at(0)).toInt(), QString(cell2List.at(1)).toInt()};
            SElementIndex elementIndex = getElementIndex(type, coordinate);
            elementMap.insert(key, elementIndex);

            if(cellList.count() == 4) {
                //FIXME 需要设置memory、imd、infifo的资源源文件
                QString cell3 = cellList.at(3);
                cell3 = cell3.trimmed();
                cell3 = cell3.right(cell3.length() - 1);
                cell3 = cell3.left(cell3.length() - 1);
                QStringList cell3List = cell3.trimmed().split(",");
                QMap<QString, QString> attributeMap = stringListToMap(cell3List, "=");
                switch (elementIndex.type) {
                case ELEMENT_BFU:
                {
                    ElementBfu *bfu = rcaScene.findBfuFromScene(elementIndex.bcu, elementIndex.rcu, elementIndex.pe);
                    bfu->funcIndex = BfuFuncIndex(mapBfuFunc.key(attributeMap.value(XML_Bfu_Func, "au")));
                    bfu->funcExp = attributeMap.value(XML_Bfu_Func_Exp);
                    bfu->funcAuModIndex = attributeMap.value(XML_Bfu_Au_Mod + "e", "0").toInt(); // zhangjun modified at 20201022
                    bfu->bypassIndex = attributeMap.value(XML_Bfu_Bypass.toLower(), "0").toInt();
                    bfu->bypassExp = attributeMap.value(XML_Bfu_Bypass_Exp.toLower());
                    break;
                }
                case ELEMENT_SBOX:
                {
                    ElementSbox *sbox = rcaScene.findSboxFromScene(elementIndex.bcu, elementIndex.rcu, elementIndex.pe);
                    sbox->srcConfig = attributeMap.value(XML_Sbox_Src);
                    if(!sbox->srcConfig.startsWith(":/"))
                        sbox->srcConfig.insert(0, QString(":/"));
                    sbox->sboxMode = attributeMap.value(XML_Sbox_Mode, "0").toInt();
                    sbox->sboxIncreaseMode = attributeMap.value(XML_Sbox_IncreaseMode, "0").toInt();
                    sbox->groupCnt0 = SboxGroupValue(attributeMap.value("id0", "0").toInt());
                    sbox->groupCnt1 = SboxGroupValue(attributeMap.value("id1", "0").toInt());
                    sbox->groupCnt2 = SboxGroupValue(attributeMap.value("id2", "0").toInt());
                    sbox->groupCnt3 = SboxGroupValue(attributeMap.value("id3", "0").toInt());
                    break;
                }
                case ELEMENT_BENES:
                {
                    ElementBenes *bense = rcaScene.findBenesFromScene(elementIndex.bcu, elementIndex.rcu, elementIndex.pe);
                    bense->srcConfig = attributeMap.value(XML_Benes_Src);
                    if(!bense->srcConfig.startsWith(":/"))
                        bense->srcConfig.insert(0, QString(":/"));
                    break;
                }
                case ELEMENT_INMEM:
                {
                    ModuleRcu *rcu = rcaScene.findRcuFromScene(elementIndex.bcu, elementIndex.rcu);
                    rcu->readMemOffset = attributeMap.value(XML_RCU_RMEM_OFFSET.toLower()).toInt();
                    rcu->readMemMode = attributeMap.value(XML_RCU_RMEM_MODE.toLower()).toInt();

                    // FIXME 暂时按照cicada的来解析，所以需要一下转换
                    if(rcu->readMemMode != 1)
                        rcu->readMemMode = 0;

                    rcu->readMemMode128_32 = attributeMap.value(XML_RCU_RMEM_MODE128_32.toLower()).toInt();
                    rcu->readMemThreashold = attributeMap.value(XML_RCU_RMEM_THREASHOLD.toLower()).toInt();
                    switch (elementIndex.pe) {
                    case 0:
                        rcu->readMemAddr1 = attributeMap.value("addr").toInt();
                        break;
                    case 1:
                        rcu->readMemAddr2 = attributeMap.value("addr").toInt();
                        break;
                    case 2:
                        rcu->readMemAddr3 = attributeMap.value("addr").toInt();
                        break;
                    case 3:
                        rcu->readMemAddr4 = attributeMap.value("addr").toInt();
                        break;
                    default:
                        break;
                    }
                    break;
                }
                case ELEMENT_OUTMEM:
                {
                    ModuleRcu *rcu = rcaScene.findRcuFromScene(elementIndex.bcu, elementIndex.rcu);
                    rcu->writeMemOffset = attributeMap.value(XML_RCU_WMEM_OFFSET.toLower(), "0").toInt();
                    rcu->writeMemMask = attributeMap.value(XML_RCU_WMEM_MASK.toLower(), "0").toInt();
                    rcu->writeMemMode = attributeMap.value(XML_RCU_WMEM_MODE.toLower(), "0").toInt();
                    rcu->writeMemThreashold = attributeMap.value(XML_RCU_WMEM_THREASHOLD.toLower(), "0").toInt();
                    rcu->writeMemAddr = attributeMap.value(XML_RCU_WMEM_ADDR.toLower(), "0").toInt();

                    // FIXME 暂时按照cicada的来解析，所以需要一下转换
                    if(rcu->writeMemMode != 1)
                        rcu->writeMemMode = 0;
                    break;
                }
                case ELEMENT_INIMD: // IMD 128/32模式设置
                {
                    ModuleBcu *bcu = rcaScene.findBcuFromScene(elementIndex.bcu);
                    bcu->readImdMode128_32 = attributeMap.value(XML_BCU_RIMD_MODE128_32.toLower()).toInt();
                    break;
                }
                case ELEMENT_INRCH:
                {
                    ModuleRcu *rcu = rcaScene.findRcuFromScene(elementIndex.bcu, elementIndex.rcu);
                    rcu->rchMode = attributeMap.value(XML_RCU_RMEM_MODE.toLower()).toInt();
                    break;
                }
                case ELEMENT_OUTRCH:
                {
                    ModuleRcu *rcu = rcaScene.findRcuFromScene(elementIndex.bcu, elementIndex.rcu);
                    rcu->rchMode = attributeMap.value(XML_RCU_RMEM_MODE.toLower()).toInt();
                    switch (elementIndex.pe) {
                    case 0:
                        rcu->writeRchAddr1 = attributeMap.value("addr").toInt();
                        break;
                    case 1:
                        rcu->writeRchAddr2 = attributeMap.value("addr").toInt();
                        break;
                    case 2:
                        rcu->writeRchAddr3 = attributeMap.value("addr").toInt();
                        break;
                    case 3:
                        rcu->writeRchAddr4 = attributeMap.value("addr").toInt();
                        break;
                    default:
                        break;
                    }
                    break;
                }
                default:
                    break;
                }

            }
        } else { //处理连线 "Connection Mapping Result:"
            QStringList cellList = line.split("->");
            if(cellList.count() != 2)
                continue;

            QString cell0 = cellList.at(0);
            QStringList cell0List = cell0.split("_");
            if(cell0List.count() != 4) {
                continue;
            }
            QString srcKey = cell0List.at(0);
            if(!elementMap.contains(srcKey))
            {
                continue;
            }
            SElementIndex srcElementIndex = elementMap.value(srcKey);
            int srcOutputNum = QString(cell0List.at(3)).toInt();

            QString cell1 = cellList.at(1);
            QString dstKey = cell1.left(cell1.indexOf("("));
            if(!elementMap.contains(dstKey))
            {
                continue;
            }
            SElementIndex dstElementIndex = elementMap.value(dstKey);
            const QStringList &dstInputStrCellList = cell1.mid(cell1.indexOf("(") + 1, cell1.indexOf(")") - cell1.indexOf("(") - 1).remove(")").split(".");
            if(dstInputStrCellList.count() < 3){
                continue;
            }
            QString dstInputStr = dstInputStrCellList.last();
            ElementPort* port = nullptr;
            switch (dstElementIndex.type) {
            case ELEMENT_BFU:
            {
                int dstInputNum = *(dstInputStr.right(1).toLocal8Bit().data()) - 97;
                ElementBfu *bfu = rcaScene.findBfuFromScene(dstElementIndex.bcu, dstElementIndex.rcu, dstElementIndex.pe);
                switch (dstInputNum) {
                case 0:
                    port = bfu->inPortA;
                    break;
                case 1:
                    port = bfu->inPortB;
                    break;
                default:
                    port = bfu->inPortT;
                    break;
                }
                break;
            }
            case ELEMENT_SBOX:
            {
                ElementSbox *sbox = rcaScene.findSboxFromScene(dstElementIndex.bcu, dstElementIndex.rcu, dstElementIndex.pe);
                port = sbox->inPort0;

                break;
            }
            case ELEMENT_BENES:
            {
                int dstInputNum = *(dstInputStr.right(1).toLocal8Bit().data()) - 97;
                ElementBenes *bense = rcaScene.findBenesFromScene(dstElementIndex.bcu, dstElementIndex.rcu, dstElementIndex.pe);
                switch (dstInputNum) {
                case 0:
                    port = bense->inPort0;
                    break;
                case 1:
                    port = bense->inPort1;
                    break;
                case 2:
                    port = bense->inPort2;
                    break;
                case 3:
                    port = bense->inPort3;
                    break;
                default:
                    break;
                }
                break;
            }
            case ELEMENT_OUTRCH: //输入为rch时，记录在输出端子的port上，和输入为memory情况相同
            {
//                int dstInputNum = dstElementIndex.pe;
                ModuleRcu *rcu = rcaScene.findRcuFromScene(srcElementIndex.bcu, srcElementIndex.rcu);
                int rchAddr = 0;
                switch (dstElementIndex.pe) {
                case 0:
                    rchAddr = rcu->writeRchAddr1;
                    break;
                case 1:
                    rchAddr = rcu->writeRchAddr2;
                    break;
                case 2:
                    rchAddr = rcu->writeRchAddr3;
                    break;
                case 3:
                    rchAddr = rcu->writeRchAddr4;
                    break;
                }

                switch (srcElementIndex.type) {
                case ELEMENT_BFU:
                {

                    ElementBfu *bfu = rcaScene.findBfuFromScene(srcElementIndex.bcu, srcElementIndex.rcu, srcElementIndex.pe);
                    switch (srcOutputNum) {
                    case 0:
                        port = bfu->outPortX;
                        port->addWriteRchFromXml(InputPort_CurrentBfuX, srcElementIndex.pe, dstElementIndex.pe, rchAddr, bfu);
                        break;
                    case 1:
                        port = bfu->outPortY;
                        port->addWriteRchFromXml(InputPort_CurrentBfuY, srcElementIndex.pe, dstElementIndex.pe, rchAddr, bfu);
                        break;
                    default:
                        break;
                    }
                    break;
                }
                case ELEMENT_SBOX:
                {
                    ElementSbox *sbox = rcaScene.findSboxFromScene(srcElementIndex.bcu, srcElementIndex.rcu, srcElementIndex.pe);
                    switch (srcOutputNum) {
                    case 0:
                        port = sbox->outPort0;
                        break;
                    case 1:
                        port = sbox->outPort1;
                        break;
                    case 2:
                        port = sbox->outPort2;
                        break;
                    case 3:
                        port = sbox->outPort3;
                        break;
                    default:
                        break;
                    }
                    port->addWriteRchFromXml(InputPort_CurrentSbox, srcElementIndex.pe * 4 + srcOutputNum, dstElementIndex.pe, rchAddr, sbox);
                    break;
                }
                case ELEMENT_BENES:
                {
                    ElementBenes *bense = rcaScene.findBenesFromScene(srcElementIndex.bcu, srcElementIndex.rcu, srcElementIndex.pe);
                    switch (srcOutputNum) {
                    case 0:
                        port = bense->outPort0;
                        break;
                    case 1:
                        port = bense->outPort1;
                        break;
                    case 2:
                        port = bense->outPort2;
                        break;
                    case 3:
                        port = bense->outPort3;
                        break;
                    default:
                        break;
                    }
                    port->addWriteRchFromXml(InputPort_CurrentBenes, srcElementIndex.pe * 4 + srcOutputNum, dstElementIndex.pe, rchAddr, bense);
                    break;
                }
                default:
                    break;
                }

                continue;
//                break;
            }
            case ELEMENT_OUTMEM: //输入为memory时，记录在输出端子的port上
            {
//                int dstInputNum = dstElementIndex.pe;
                switch (srcElementIndex.type) {
                case ELEMENT_BFU:
                {
                    ElementBfu *bfu = rcaScene.findBfuFromScene(srcElementIndex.bcu, srcElementIndex.rcu, srcElementIndex.pe);
                    switch (srcOutputNum) {
                    case 0:
                        port = bfu->outPortX;
                        port->addWriteMemFromXml(InputPort_CurrentBfuX, srcElementIndex.pe, dstElementIndex.pe, bfu);
                        break;
                    case 1:
                        port = bfu->outPortY;
                        port->addWriteMemFromXml(InputPort_CurrentBfuY, srcElementIndex.pe, dstElementIndex.pe, bfu);
                        break;
                    default:
                        break;
                    }
                    break;
                }
                case ELEMENT_SBOX:
                {
                    ElementSbox *sbox = rcaScene.findSboxFromScene(srcElementIndex.bcu, srcElementIndex.rcu, srcElementIndex.pe);
                    switch (srcOutputNum) {
                    case 0:
                        port = sbox->outPort0;
                        break;
                    case 1:
                        port = sbox->outPort1;
                        break;
                    case 2:
                        port = sbox->outPort2;
                        break;
                    case 3:
                        port = sbox->outPort3;
                        break;
                    default:
                        break;
                    }
                    port->addWriteMemFromXml(InputPort_CurrentSbox, srcElementIndex.pe * 4 + srcOutputNum, dstElementIndex.pe, sbox);
                    break;
                }
                case ELEMENT_BENES:
                {
                    ElementBenes *bense = rcaScene.findBenesFromScene(srcElementIndex.bcu, srcElementIndex.rcu, srcElementIndex.pe);
                    switch (srcOutputNum) {
                    case 0:
                        port = bense->outPort0;
                        break;
                    case 1:
                        port = bense->outPort1;
                        break;
                    case 2:
                        port = bense->outPort2;
                        break;
                    case 3:
                        port = bense->outPort3;
                        break;
                    default:
                        break;
                    }
                    port->addWriteMemFromXml(InputPort_CurrentBenes, srcElementIndex.pe * 4 + srcOutputNum, dstElementIndex.pe, bense);
                    break;
                }
                default:
                    break;
                }

                continue;
//                break;
            }
            case ELEMENT_OUTFIFO:
            {
                ModuleRcu *rcu = rcaScene.findRcuFromScene(dstElementIndex.bcu, dstElementIndex.rcu);
                switch (dstElementIndex.pe) {
                case 0:
                    port = rcu->outFifo0;
                    break;
                case 1:
                    port = rcu->outFifo1;
                    break;
                case 2:
                    port = rcu->outFifo2;
                    break;
                case 3:
                    port = rcu->outFifo3;
                    break;
                default:
                    break;
                }
                break;
            }
            default:
                break;
            }

            int inputIndex = 0;
            InputPortType inputType = InputPort_NULL;
            switch (srcElementIndex.type) {
            case ELEMENT_BFU:
            {
                inputIndex = srcElementIndex.pe;
                if(srcElementIndex.bcu == dstElementIndex.bcu){
                    if(dstElementIndex.rcu - 1 == srcElementIndex.rcu) {
                        inputType = srcOutputNum == 0 ? InputPort_LastBfuX : InputPort_LastBfuY;
                    } else if(dstElementIndex.rcu == srcElementIndex.rcu ||
                               dstElementIndex.type == ELEMENT_OUTFIFO) { // for outfifo
                        inputType = srcOutputNum == 0 ? InputPort_CurrentBfuX : InputPort_CurrentBfuY;
                    }
                } else if((dstElementIndex.bcu - 1 == srcElementIndex.bcu ||
                            (dstElementIndex.bcu == 0 && srcElementIndex.bcu == 3)) &&
                            dstElementIndex.rcu == 0 && srcElementIndex.rcu == 3) {
                        inputType = srcOutputNum == 0 ? InputPort_LastBfuX : InputPort_LastBfuY;
                }
                break;
            }
            case ELEMENT_SBOX:
            {
                inputIndex = srcElementIndex.pe * 4 + srcOutputNum;
                if(srcElementIndex.bcu == dstElementIndex.bcu){
                    if(dstElementIndex.rcu - 1 == srcElementIndex.rcu) {
                        inputType = InputPort_LastSbox;
                    } else if(dstElementIndex.rcu == srcElementIndex.rcu ||
                               dstElementIndex.type == ELEMENT_OUTFIFO) { // for outfifo
                        inputType = InputPort_CurrentSbox;
                    }
                } else if((dstElementIndex.bcu - 1 == srcElementIndex.bcu ||
                            (dstElementIndex.bcu == 0 && srcElementIndex.bcu == 3)) &&
                                                     dstElementIndex.rcu == 0 && srcElementIndex.rcu == 3) {
                    inputType = InputPort_LastSbox;
                }
                break;
            }
            case ELEMENT_BENES:
            {
                inputIndex = srcOutputNum;
                if(srcElementIndex.bcu == dstElementIndex.bcu){
                    if(dstElementIndex.rcu - 1 == srcElementIndex.rcu) {
                        inputType = InputPort_LastBenes;
                    } else if(dstElementIndex.rcu == srcElementIndex.rcu ||
                               dstElementIndex.type == ELEMENT_OUTFIFO) { // for outfifo
                        inputType = InputPort_CurrentBenes;
                    }
                } else if((dstElementIndex.bcu - 1 == srcElementIndex.bcu ||
                            (dstElementIndex.bcu == 0 && srcElementIndex.bcu == 3)) &&
                                                     dstElementIndex.rcu == 0 && srcElementIndex.rcu == 3) {
                    inputType = InputPort_LastBenes;
                }
                break;
            }
            case ELEMENT_INMEM:
            {
                Q_ASSERT(srcElementIndex.bcu == dstElementIndex.bcu &&
                         srcElementIndex.rcu == dstElementIndex.rcu);
                inputIndex = srcElementIndex.pe;
                inputType = InputPort_Mem;
                break;
            }
            case ELEMENT_INFIFO:
            {
                Q_ASSERT(srcElementIndex.bcu == dstElementIndex.bcu);
                inputIndex = srcElementIndex.pe;
                inputType = InputPort_InFifo;
                break;
            }
            case ELEMENT_INIMD:
            {
                Q_ASSERT(srcElementIndex.bcu == dstElementIndex.bcu);
                inputIndex = srcElementIndex.pe;
                inputType = InputPort_Imd;
                break;
            }
            case ELEMENT_INRCH:
            {
                Q_ASSERT(srcElementIndex.bcu == dstElementIndex.bcu &&
                         srcElementIndex.rcu == dstElementIndex.rcu);
                inputIndex = srcElementIndex.pe;
                inputType = InputPort_Rch;
                break;
            }
            default:
                break;
            }
            port->setInputIndex(inputIndex);
            port->setInputType(inputType);
        }
    }
    return true;
}

void CXmlResolver::cleanRcaDomElement()
{
    m_document->clear();
    m_root.clear();
    m_rca.clear();

    if(m_root.isNull())
    {
        QDomElement element = m_document->createElement(TAGALGRITHM);
        element.setAttribute("name","");
        element.setAttribute("md5", "");
        m_root = element;
    }

    if(m_rca.isNull())
    {
        QDomElement element = m_document->createElement("RCA");
        element.setAttribute("id",0);
        m_rca = element;
    }


    m_root.appendChild(m_rca);
    m_root.appendChild(m_rsm);
    m_root.appendChild(m_uiVer);

    m_document->appendChild(m_root);

    QDomProcessingInstruction instruction;
    instruction = m_document->createProcessingInstruction("xml", "version=\'1.0\' encoding=\'UTF-8\'");
    m_document->insertBefore(instruction, m_root);
}
/**
 * CXmlResolver::isElementExist
 * \brief
 * \param   bcuid
 * \param   rcuid
 * \param   element
 * \param   id
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool CXmlResolver::isElementExist(int bcuid, int rcuid, ElementType element, int id)
{
    QDomElement rcu = findRCUElement(bcuid,rcuid);
    int max = (element==ELEMENT_BFU)?BFUMAX:((element==ELEMENT_SBOX)?SBOXMAX:BENESMAX);
    if(bcuid >= BCUMAX || rcuid >= RCUMAX || id >= max || rcu.isNull())
    {
        return false;
    }
    QDomNodeList list = rcu.childNodes();
    QString name = element==ELEMENT_BFU?"BFU":
                                          element==ELEMENT_SBOX?"SBOX":
                                                                  element==ELEMENT_BENES?"BENES":"";
    if(name.isEmpty())return false;
    for(int i = 0; i<list.count(); i++)
    {
        QDomElement ele = list.at(i).toElement();
        if(ele.tagName() == name && ele.attribute("id").toInt() == id)return true;
    }
    return false;
}

/**
 * CXmlResolver::initNeededMap
 * \brief
 * \author  zhangjun
 * \date    2016-10-12
 */
void CXmlResolver::initNeededMap()
{
    //inputtype
    m_inputTypeMap.insert(0, QString(EMPTYCHOICE));
    m_inputTypeMap.insert(1,CURBFUX);
    m_inputTypeMap.insert(2,CURBFUY);
    m_inputTypeMap.insert(3,CURSBOX);
    m_inputTypeMap.insert(4,CURBENES);
    m_inputTypeMap.insert(5,LASTBFUX);
    m_inputTypeMap.insert(6,LASTBFUY);
    m_inputTypeMap.insert(7,LASTSBOX);
    m_inputTypeMap.insert(8,LASTBENES);
    m_inputTypeMap.insert(9,INFIFO);
    m_inputTypeMap.insert(10,MEMOUT);
    //bypass
    m_bypassMap.insert(0,BYPASS_A);
    m_bypassMap.insert(1,BYPASS_B);
    m_bypassMap.insert(2,BYPASS_T);
    m_bypassMap.insert(3,BYPASS_NB);
    //func
    m_funcMap.insert(0,FUNC_AU);
    m_funcMap.insert(1,FUNC_LU);
    m_funcMap.insert(2,FUNC_SU);
    //sbox
    m_sboxModeMap.insert(0,trUtf8("4bit-4bit拼接"));
    m_sboxModeMap.insert(1,trUtf8("6bit-4bit拼接"));
    m_sboxModeMap.insert(2,trUtf8("8bit-8bit拼接"));
    m_sboxModeMap.insert(3,trUtf8("8bit-8bit异或"));
    m_sboxModeMap.insert(4,trUtf8("8bit-32bit"));
    m_sboxModeMap.insert(5,trUtf8("9bit-9bit一路"));
    //bfu_au
    m_auModMap.insert(0,trUtf8("2^32"));
    m_auModMap.insert(1,trUtf8("2^16"));
    m_auModMap.insert(2,trUtf8("2^8"));
    m_auModMap.insert(3,trUtf8("2^31-1"));
    //mem
    m_memModeMap.insert(0,trUtf8("固定模式"));
    m_memModeMap.insert(1,trUtf8("累加模式"));
    m_memModeMap.insert(2,trUtf8("阈值累加"));
    m_memModeMap.insert(3,trUtf8("累加&阈值清零"));
}

/**
 * CXmlResolver::save
 * \brief
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool CXmlResolver::save()
{
    qInfo() << tr("保存XML文件：%0").arg(m_curFile);
    m_uiVer.setAttribute("ver",CURRENT_UI_VER);

    QByteArray tempBty;
    QTextStream tempOut(&tempBty);
    m_root.setAttribute("MD5", "");
    m_root.setAttribute("name", "");
    m_root.setAttribute("value", m_curFile);
    m_document->save(tempOut, 4);
    QByteArray md5Code = QCryptographicHash::hash(tempBty, QCryptographicHash::Md5);
    QString md5 = md5Code.toHex();
    m_root.setAttribute("MD5", md5);
    m_root.setAttribute("name", "");
    m_root.setAttribute("value", m_curFile);
    QByteArray bty;
    QTextStream out(&bty);
    m_document->save(out, 4);

    if(m_file->isOpen())
        m_file->close();
    if(!m_file->open(QIODevice::WriteOnly | QIODevice::Text)){
        return false;
    }
    QTextStream outToFile(m_file);
    outToFile << bty;
    m_file->close();

    if(!m_file->open(QFile::ReadOnly | QFile::Text))
        return false;
    return true;
}

/**
 * CXmlResolver::saveAs
 * \brief
 * \param   fileName
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool CXmlResolver::saveAs(const QString &fileName)
{
    qInfo() << tr("另存为XML文件到：%0").arg(fileName);
    m_uiVer.setAttribute("ver", CURRENT_UI_VER);

    QByteArray tempBty;
    QTextStream tempOut(&tempBty);
    m_root.setAttribute("MD5", "");
    m_root.setAttribute("name", "");
    m_root.setAttribute("value", m_curFile);
    m_document->save(tempOut, 4);
    QByteArray md5Code = QCryptographicHash::hash(tempBty, QCryptographicHash::Md5);
    QString md5 = md5Code.toHex();
    m_root.setAttribute("MD5", md5);
    m_root.setAttribute("name", "");
    m_root.setAttribute("value", m_curFile);
    QByteArray bty;
    QTextStream out(&bty);
    m_document->save(out, 4);

    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream outToFile(&file);
    outToFile << bty;
    file.close();

    m_file->close();
    m_file->setFileName(fileName);
    m_curFile = fileName;
    //    m_paraInfo->xmlFNP = m_curFile;
    //    m_cmdWordInfo->xmlFile = m_curFile;
    if(!m_file->open(QFile::ReadOnly | QFile::Text))
        return false;
    m_document = new QDomDocument();
    m_document->setContent(m_file);
    m_root = m_document->documentElement();

    if(m_root.isNull())
    {
        QDomElement element = m_document->createElement(TAGALGRITHM);
        element.setAttribute("name","");
        m_document->appendChild(element);
        m_root = element;
    }
    else if(m_root.tagName() != TAGALGRITHM)
    {
    }
    m_rca = m_root.firstChildElement("RCA");
    if(m_rca.isNull())
    {
        QDomElement element = m_document->createElement("RCA");
        element.setAttribute("id",0);
        m_root.appendChild(element);
        m_rca = element;
    }
    m_rsm = m_rca.nextSiblingElement("RSM");
    if(m_rsm.isNull())
    {
        QDomElement element = m_document->createElement("RSM");
        element.setAttribute("id",0);
        m_root.appendChild(element);
        m_rsm = element;
    }
    return true;
}

/**
 * CXmlResolver::getRsm
 * \brief
 * \param   attr
 * \param   defaultValue
 * \return  int
 * \author  zhangjun
 * \date    2016-10-12
 */
int CXmlResolver::getRsm(AttributeID attr, int defaultValue)
{
    switch(attr)
    {
#define GETATTR(attr,str) \
    case attr:\
    {\
    return m_rsm.attribute(str).toInt();\
    }
        GETATTR(ONLYRSM,"OnlyRsm")
        GETATTR(RMODE,"rmode")
        GETATTR(GAP,"gap")
        GETATTR(SOURCE,"source")
        GETATTR(DEST,"dest")
        GETATTR(BURST,"burst")
        GETATTR(LOOP,"loop")
        GETATTR(REPEAT,"repeat")
        GETATTR(BCUSEL,"bcusel")
    default:return defaultValue;
    }
}

/**
 * CXmlResolver::setRsm
 * \brief
 * \param   attr
 * \param   value
 * \author  zhangjun
 * \date    2016-10-12
 */
void CXmlResolver::setRsm(AttributeID attr, int value)
{
    switch(attr)
    {
#define SETATTR(attr,str) \
    case attr:\
    {\
    m_rsm.setAttribute(str,value);\
    break;\
    }
        SETATTR(ONLYRSM,"OnlyRsm")
        SETATTR(RMODE,"rmode")
        SETATTR(GAP,"gap")
        SETATTR(SOURCE,"source")
        SETATTR(DEST,"dest")
        SETATTR(BURST,"burst")
        SETATTR(LOOP,"loop")
        SETATTR(REPEAT,"repeat")
        SETATTR(BCUSEL,"bcusel")
    default:break;
    }
}

void CXmlResolver::setBcuElementAttribute(ModuleBcu* bcuElement)
{
    if(bcuElement == nullptr) return;

    QDomElement element = findBCUElement(bcuElement->getIndexBcu());
    QRect bcuRcet=bcuElement->getRealBoundingRect().toRect();

    //    QPointF posAtParent = bcuElement->mapToParent(bcuElement->getRealBoundingRect().topLeft());
    QPointF posAtParent = bcuElement->mapToParent(QPointF(0,0));
    element.setAttribute(XML_XAtParent,posAtParent.toPoint().x());
    element.setAttribute(XML_YAtParent,posAtParent.toPoint().y());

    element.setAttribute(XML_X,bcuRcet.left());
    element.setAttribute(XML_Y,bcuRcet.top());
    element.setAttribute(XML_Width,bcuRcet.width());
    element.setAttribute(XML_Height,bcuRcet.height());

    element.setAttribute(XML_BCU_DATA_SRC, bcuElement->srcDataFromBcu);
    element.setAttribute(XML_BCU_DATA_SORT, bcuElement->sort);
    element.setAttribute(XML_BCU_DATA_BURST, bcuElement->burst);
    element.setAttribute(XML_BCU_DATA_GAP, bcuElement->gap);
    element.setAttribute(XML_BCU_DATA_INFIFOSRC, bcuElement->infifoSrc);
    element.setAttribute(XML_BCU_DATA_OUTFIFOSRC, bcuElement->outfifoSrc);
    element.setAttribute(XML_BCU_DATA_LFSRGROUP, bcuElement->lfsrGroup);
    element.setAttribute(XML_BCU_RIMD_MODE128_32, bcuElement->readImdMode128_32);
    element.setAttribute(XML_BCU_MEM_SRC, bcuElement->memorySrcConfig);
    element.setAttribute(XML_BCU_IMD_SRC, bcuElement->imdSrcConfig);
    element.setAttribute(XML_BCU_INFIFO_SRC, bcuElement->inFifoSrcConfig);
    element.setAttribute(XML_BCU_LFSR_DEPTH, bcuElement->writeDepth);
    element.setAttribute(XML_BCU_LFSR_WIDTH, bcuElement->writeWidth);
    element.setAttribute(XML_BCU_LFSR_ADDRESSMODE, bcuElement->addressMode);


    //zhangjun 于20201210注释，适配CGRA RPU，outfifo移至RCU
//    if(bcuElement->outFifo0 != nullptr)
//    {

//        QDomElement eleInPort = findBCUOUTElement(bcuElement->getIndexBcu(),0);
//        setInputPortToDom(&eleInPort,bcuElement->outFifo0);
//        //        eleInPort.setAttribute(XML_InPort_Type,bcuElement->outFifo0->getInputType());
//        //        eleInPort.setAttribute(XML_InPort_Index,bcuElement->outFifo0->getInputIndex());
//        //        eleInPort.setAttribute(XML_InPort_Middle_Y,bcuElement->outFifo0->middlePosY);
//    }
//    if(bcuElement->outFifo1 != nullptr)
//    {

//        QDomElement eleInPort = findBCUOUTElement(bcuElement->getIndexBcu(),1);
//        setInputPortToDom(&eleInPort,bcuElement->outFifo1);
//        //        eleInPort.setAttribute(XML_InPort_Type,bcuElement->outFifo1->getInputType());
//        //        eleInPort.setAttribute(XML_InPort_Index,bcuElement->outFifo1->getInputIndex());
//        //        eleInPort.setAttribute(XML_InPort_Middle_Y,bcuElement->outFifo1->middlePosY);
//    }
//    if(bcuElement->outFifo2 != nullptr)
//    {

//        QDomElement eleInPort = findBCUOUTElement(bcuElement->getIndexBcu(),2);
//        setInputPortToDom(&eleInPort,bcuElement->outFifo2);
//        //        eleInPort.setAttribute(XML_InPort_Type,bcuElement->outFifo2->getInputType());
//        //        eleInPort.setAttribute(XML_InPort_Index,bcuElement->outFifo2->getInputIndex());
//        //        eleInPort.setAttribute(XML_InPort_Middle_Y,bcuElement->outFifo2->middlePosY);
//    }
//    if(bcuElement->outFifo3 != nullptr)
//    {

//        QDomElement eleInPort = findBCUOUTElement(bcuElement->getIndexBcu(),3);
//        setInputPortToDom(&eleInPort,bcuElement->outFifo3);
//        //        eleInPort.setAttribute(XML_InPort_Type,bcuElement->outFifo3->getInputType());
//        //        eleInPort.setAttribute(XML_InPort_Index,bcuElement->outFifo3->getInputIndex());
//        //        eleInPort.setAttribute(XML_InPort_Middle_Y,bcuElement->outFifo3->middlePosY);
//    }
}

void CXmlResolver::setInputPortToDom(QDomElement* eleInPort,ElementPort* port)
{
    eleInPort->setAttribute(XML_InPort_Type,port->getInputType());
    eleInPort->setAttribute(XML_InPort_Index,port->getInputIndex());
    //    eleInPort->setAttribute(XML_InPort_Middle_Y,port->middlePosY);

    QDomElement infelctPosDom = findPortInfelctPos(eleInPort);
    infelctPosDom.setAttribute(XML_INFECT_POS,port->getPortInflectPos());
    QDomElement pressedPosDom = findPortPressedPos(eleInPort);
    pressedPosDom.setAttribute(XML_PRESSED_POS,port->getPortPressedPos());
}

void CXmlResolver::setRcuElementAttribute(ModuleRcu* rcuElement)
{
    if(rcuElement == nullptr) return;

    QDomElement element = findRCUElement(rcuElement->getIndexBcu(),rcuElement->getIndexRcu());
    QRect rcuRcet=rcuElement->getRealBoundingRect().toRect();

    //    QPointF posAtParent = rcuElement->mapToParent(rcuElement->getRealBoundingRect().topLeft());
    QPointF posAtParent = rcuElement->mapToParent(QPointF(0,0));

    element.setAttribute(XML_XAtParent,posAtParent.toPoint().x());
    element.setAttribute(XML_YAtParent,posAtParent.toPoint().y());
    element.setAttribute(XML_X,rcuRcet.left());
    element.setAttribute(XML_Y,rcuRcet.top());
    element.setAttribute(XML_Width,rcuRcet.width());
    element.setAttribute(XML_Height,rcuRcet.height());

    if(rcuElement->outFifo0 != nullptr)
    {
        QDomElement eleInPort = findRCUOUTElement(rcuElement->getIndexBcu(), rcuElement->getIndexRcu(), 0);
        setInputPortToDom(&eleInPort,rcuElement->outFifo0);
        //        eleInPort.setAttribute(XML_InPort_Type,rcuElement->outFifo0->getInputType());
        //        eleInPort.setAttribute(XML_InPort_Index,rcuElement->outFifo0->getInputIndex());
        //        eleInPort.setAttribute(XML_InPort_Middle_Y,rcuElement->outFifo0->middlePosY);
    }
    if(rcuElement->outFifo1 != nullptr)
    {
        QDomElement eleInPort = findRCUOUTElement(rcuElement->getIndexBcu(), rcuElement->getIndexRcu(), 1);
        setInputPortToDom(&eleInPort,rcuElement->outFifo1);
        //        eleInPort.setAttribute(XML_InPort_Type,rcuElement->outFifo1->getInputType());
        //        eleInPort.setAttribute(XML_InPort_Index,rcuElement->outFifo1->getInputIndex());
        //        eleInPort.setAttribute(XML_InPort_Middle_Y,rcuElement->outFifo1->middlePosY);
    }
    if(rcuElement->outFifo2 != nullptr)
    {

        QDomElement eleInPort = findRCUOUTElement(rcuElement->getIndexBcu(), rcuElement->getIndexRcu(), 2);
        setInputPortToDom(&eleInPort,rcuElement->outFifo2);
        //        eleInPort.setAttribute(XML_InPort_Type,rcuElement->outFifo2->getInputType());
        //        eleInPort.setAttribute(XML_InPort_Index,rcuElement->outFifo2->getInputIndex());
        //        eleInPort.setAttribute(XML_InPort_Middle_Y,rcuElement->outFifo2->middlePosY);
    }
    if(rcuElement->outFifo3 != nullptr)
    {

        QDomElement eleInPort = findRCUOUTElement(rcuElement->getIndexBcu(), rcuElement->getIndexRcu(), 3);
        setInputPortToDom(&eleInPort,rcuElement->outFifo3);
        //        eleInPort.setAttribute(XML_InPort_Type,rcuElement->outFifo3->getInputType());
        //        eleInPort.setAttribute(XML_InPort_Index,rcuElement->outFifo3->getInputIndex());
        //        eleInPort.setAttribute(XML_InPort_Middle_Y,rcuElement->outFifo3->middlePosY);
    }

    QDomElement loopDom = findRCULoopElement(rcuElement->getIndexBcu(),rcuElement->getIndexRcu());
    loopDom.setAttribute(XML_RCU_LOOP_START_END, rcuElement->loopStartEndFlag);
    loopDom.setAttribute(XML_RCU_LOOP_TIMES, rcuElement->loopTimes);
    loopDom.setAttribute(XML_RCU_LOOP_START_END2, rcuElement->loopStartEndFlag2);
    loopDom.setAttribute(XML_RCU_LOOP_TIMES2, rcuElement->loopTimes2);

    QDomElement writeMemDom = findRCUMemWTElement(rcuElement->getIndexBcu(),rcuElement->getIndexRcu());
    writeMemDom.setAttribute(XML_RCU_WMEM_OFFSET,rcuElement->writeMemOffset);
    writeMemDom.setAttribute(XML_RCU_WMEM_MASK,rcuElement->writeMemMask);
    writeMemDom.setAttribute(XML_RCU_WMEM_MODE,rcuElement->writeMemMode);
    writeMemDom.setAttribute(XML_RCU_WMEM_THREASHOLD,rcuElement->writeMemThreashold);
    writeMemDom.setAttribute(XML_RCU_WMEM_ADDR,rcuElement->writeMemAddr);

    QDomElement readMemDom = findRCUMemRDElement(rcuElement->getIndexBcu(),rcuElement->getIndexRcu());
    readMemDom.setAttribute(XML_RCU_RMEM_OFFSET,rcuElement->readMemOffset);
    readMemDom.setAttribute(XML_RCU_RMEM_MODE,rcuElement->readMemMode);
    readMemDom.setAttribute(XML_RCU_RMEM_MODE128_32,rcuElement->readMemMode128_32);
    readMemDom.setAttribute(XML_RCU_RMEM_THREASHOLD,rcuElement->readMemThreashold);
    readMemDom.setAttribute(XML_RCU_RMEM_ADDR1,rcuElement->readMemAddr1);
    readMemDom.setAttribute(XML_RCU_RMEM_ADDR2,rcuElement->readMemAddr2);
    readMemDom.setAttribute(XML_RCU_RMEM_ADDR3,rcuElement->readMemAddr3);
    readMemDom.setAttribute(XML_RCU_RMEM_ADDR4,rcuElement->readMemAddr4);

    QDomElement readRchDom = findRCURchElement(rcuElement->getIndexBcu(),rcuElement->getIndexRcu());
    readRchDom.setAttribute(XML_RCU_RCH_MODE,rcuElement->rchMode);
//    readRchDom.setAttribute(XML_RCU_RCH_ADDR1,rcuElement->writeRchAddr1);
//    readRchDom.setAttribute(XML_RCU_RCH_ADDR2,rcuElement->writeRchAddr2);
//    readRchDom.setAttribute(XML_RCU_RCH_ADDR3,rcuElement->writeRchAddr3);
//    readRchDom.setAttribute(XML_RCU_RCH_ADDR4,rcuElement->writeRchAddr4);
}

void CXmlResolver::setArithUnitElementAttribute(BaseArithUnit* arithUnit)
{
    if(arithUnit == nullptr) return;
    ElementBfu* elementBfu = dynamic_cast<ElementBfu*>(arithUnit);
    ElementCustomPE* elementCustomPE = dynamic_cast<ElementCustomPE*>(arithUnit);
    ElementSbox* elementSbox = dynamic_cast<ElementSbox*>(arithUnit);
    ElementBenes* elementBenes = dynamic_cast<ElementBenes*>(arithUnit);

    if(elementBfu)
    {
        setBfuElementAttribute(elementBfu);
    }
    else if(elementCustomPE)
    {
        setCustomPEElementAttribute(elementCustomPE);
    }
    else if(elementSbox)
    {
        setSboxElementAttribute(elementSbox);
    }
    else if(elementBenes)
    {
        setBenesElementAttribute(elementBenes);
    }
}

void CXmlResolver::setBfuElementAttribute(ElementBfu* bfuElement)
{
    if(bfuElement == nullptr) return;

    int bcuid = bfuElement->getIndexBcu();
    int rcuid = bfuElement->getIndexRcu();
    int elementid = bfuElement->getIndexInRcu();

    QDomElement element = findElement(bcuid,rcuid,ELEMENT_BFU,elementid);
    QRect bfuRcet=bfuElement->getRealBoundingRect().toRect();

    //    QPointF posAtParent = bfuElement->mapToParent(bfuElement->getRealBoundingRect().topLeft());
    QPointF posAtParent = bfuElement->mapToParent(QPointF(0,0));

    element.setAttribute(XML_XAtParent,posAtParent.toPoint().x());
    element.setAttribute(XML_YAtParent,posAtParent.toPoint().y());
    element.setAttribute(XML_X,bfuRcet.left());
    element.setAttribute(XML_Y,bfuRcet.top());
    element.setAttribute(XML_Width,bfuRcet.width());
    element.setAttribute(XML_Height,bfuRcet.height());

    element.setAttribute(XML_Bfu_Func,mapBfuFunc.value(bfuElement->funcIndex));
    element.setAttribute(XML_Bfu_Func_Exp,bfuElement->funcExp);
    element.setAttribute(XML_Bfu_Au_Mod,bfuElement->funcAuModIndex);
    element.setAttribute(XML_Bfu_Au_Carry,bfuElement->funcAuCarry);
    element.setAttribute(XML_Bfu_Mu_Mod,bfuElement->funcMuModIndex);
    element.setAttribute(XML_Bfu_Bypass,bfuElement->bypassIndex);
    element.setAttribute(XML_Bfu_Bypass_Exp,bfuElement->bypassExp);

    if(bfuElement->inPortA != nullptr)
    {
        QDomElement eleInPort = findBFUInput(bcuid,rcuid,elementid,INPUT_A);
        setInputPortToDom(&eleInPort,bfuElement->inPortA);
    }

    if(bfuElement->inPortB != nullptr)
    {
        QDomElement eleInPort = findBFUInput(bcuid,rcuid,elementid,INPUT_B);
        setInputPortToDom(&eleInPort,bfuElement->inPortB);
    }

    if(bfuElement->inPortT != nullptr)
    {
        QDomElement eleInPort = findBFUInput(bcuid,rcuid,elementid,INPUT_T);
        setInputPortToDom(&eleInPort,bfuElement->inPortT);
    }

    if(bfuElement->outPortX != nullptr)
    {
        QList<QGraphicsItem*> childItemList = bfuElement->outPortX->childItems();
        QList<QGraphicsItem*>::iterator iterChild;
        for(iterChild = childItemList.begin();iterChild!=childItemList.end();iterChild++)
        {
            if((*iterChild))
            {
                ModuleWriteMem* writeMem = dynamic_cast<ModuleWriteMem*>(*iterChild);
                ModuleWriteRch* writeRch = dynamic_cast<ModuleWriteRch*>(*iterChild);
                ModuleWriteLfsr* writeLfsr = dynamic_cast<ModuleWriteLfsr*>(*iterChild);
                if(writeMem)
                {
                    setWriteMemAttribute(bcuid,rcuid,writeMem);
                }
                else if(writeRch)
                {
                    setWriteRchAttribute(bcuid,rcuid,writeRch);
                }
                else if(writeLfsr)
                {
                    setWriteLfsrAttribute(bcuid,rcuid,writeLfsr);
                }
            }
        }
    }

    if(bfuElement->outPortY != nullptr)
    {
        QList<QGraphicsItem*> childItemList = bfuElement->outPortY->childItems();
        QList<QGraphicsItem*>::iterator iterChild;
        for(iterChild=childItemList.begin();iterChild!=childItemList.end();iterChild++)
        {
            if((*iterChild))
            {
                ModuleWriteMem* writeMem = dynamic_cast<ModuleWriteMem*>(*iterChild);
                ModuleWriteRch* writeRch = dynamic_cast<ModuleWriteRch*>(*iterChild);
                ModuleWriteLfsr* writeLfsr = dynamic_cast<ModuleWriteLfsr*>(*iterChild);
                if(writeMem)
                {
                    setWriteMemAttribute(bcuid,rcuid,writeMem);
                }
                else if(writeRch)
                {
                    setWriteRchAttribute(bcuid,rcuid,writeRch);
                }
                else if(writeLfsr)
                {
                    setWriteLfsrAttribute(bcuid,rcuid,writeLfsr);
                }
            }
        }
    }

}

void CXmlResolver::setCustomPEElementAttribute(ElementCustomPE* customPEElement)
{
    if(customPEElement == nullptr) return;

    int bcuid = customPEElement->getIndexBcu();
    int rcuid = customPEElement->getIndexRcu();
    int elementid = customPEElement->getIndexInRcu();

    QDomElement element = findElement(bcuid,rcuid,ELEMENT_CUSTOMPE,elementid);
    QRect customPERcet=customPEElement->getRealBoundingRect().toRect();

    QPointF posAtParent = customPEElement->mapToParent(QPointF(0,0));

    element.setAttribute(XML_XAtParent,posAtParent.toPoint().x());
    element.setAttribute(XML_YAtParent,posAtParent.toPoint().y());
    element.setAttribute(XML_X,customPERcet.left());
    element.setAttribute(XML_Y,customPERcet.top());
    element.setAttribute(XML_Width,customPERcet.width());
    element.setAttribute(XML_Height,customPERcet.height());

    element.setAttribute(XML_CustomPE_Func,mapBfuFunc.value(customPEElement->funcIndex));
    element.setAttribute(XML_CustomPE_Func_Exp,customPEElement->funcExp);
    element.setAttribute(XML_CustomPE_Au_Mod,customPEElement->funcAuModIndex);
//    element.setAttribute(XML_CustomPE_Au_Carry,customPEElement->funcAuCarry);
    element.setAttribute(XML_CustomPE_Mu_Mod,customPEElement->funcMuModIndex);
    element.setAttribute(XML_CustomPE_Bypass,customPEElement->bypassIndex);
    element.setAttribute(XML_CustomPE_Bypass_Exp,customPEElement->bypassExp);
    element.setAttribute(XML_CustomPE_InputNum,customPEElement->inputNum());
    element.setAttribute(XML_CustomPE_OutputNum,customPEElement->outputNum());

    for (int i = 0; i < customPEElement->m_inputPortVector.size(); ++i) {
        if(customPEElement->m_inputPortVector.at(i) != nullptr)
        {
            QDomElement eleInPort = findCUSTOMPEInput(bcuid,rcuid,elementid,i);
            setInputPortToDom(&eleInPort,customPEElement->m_inputPortVector.at(i));
        }
    }

    for (int i = 0; i < customPEElement->m_outputPortVector.size(); ++i) {
        if(customPEElement->m_outputPortVector.at(i) != nullptr)
        {
            QList<QGraphicsItem*> childItemList = customPEElement->m_outputPortVector.at(i)->childItems();
            QList<QGraphicsItem*>::iterator iterChild;
            for(iterChild = childItemList.begin();iterChild!=childItemList.end();iterChild++)
            {
                if((*iterChild))
                {
                    ModuleWriteMem* writeMem = dynamic_cast<ModuleWriteMem*>(*iterChild);
                    ModuleWriteRch* writeRch = dynamic_cast<ModuleWriteRch*>(*iterChild);
                    if(writeMem)
                    {
                        setWriteMemAttribute(bcuid,rcuid,writeMem);
                    }
                    else if(writeRch)
                    {
                        setWriteRchAttribute(bcuid,rcuid,writeRch);
                    }
                }
            }
        }
    }
}

void CXmlResolver::setSboxElementAttribute(ElementSbox* sboxElement)
{
    if(sboxElement == nullptr) return;

    int bcuid = sboxElement->getIndexBcu();
    int rcuid = sboxElement->getIndexRcu();
    int elementid = sboxElement->getIndexInRcu();

    QDomElement element = findElement(bcuid,rcuid,ELEMENT_SBOX,elementid);
    QRect sboxRcet=sboxElement->getRealBoundingRect().toRect();

    //    QPointF posAtParent = sboxElement->mapToParent(sboxElement->getRealBoundingRect().topLeft());
    QPointF posAtParent = sboxElement->mapToParent(QPointF(0,0));

    element.setAttribute(XML_XAtParent,posAtParent.toPoint().x());
    element.setAttribute(XML_YAtParent,posAtParent.toPoint().y());
    element.setAttribute(XML_X,sboxRcet.left());
    element.setAttribute(XML_Y,sboxRcet.top());
    element.setAttribute(XML_Width,sboxRcet.width());
    element.setAttribute(XML_Height,sboxRcet.height());
    element.setAttribute(XML_Sbox_Src,sboxElement->srcConfig);
    element.setAttribute(XML_Sbox_Mode,sboxElement->sboxMode);
    element.setAttribute(XML_Sbox_Group,sboxElement->sboxGroup);
    element.setAttribute(XML_Sbox_ByteSel,sboxElement->sboxByteSel);
    element.setAttribute(XML_Sbox_IncreaseMode,sboxElement->sboxIncreaseMode);

    if(sboxElement->inPort0 != nullptr)
    {
        QDomElement eleInPort = findSBOXInput(bcuid,rcuid,elementid);
        setInputPortToDom(&eleInPort,sboxElement->inPort0);
    }

    findSBOXCnt(bcuid,rcuid,elementid,0).setAttribute(XML_Sbox_Cnt_Value,sboxElement->groupCnt0);
    findSBOXCnt(bcuid,rcuid,elementid,1).setAttribute(XML_Sbox_Cnt_Value,sboxElement->groupCnt1);
    findSBOXCnt(bcuid,rcuid,elementid,2).setAttribute(XML_Sbox_Cnt_Value,sboxElement->groupCnt2);
    findSBOXCnt(bcuid,rcuid,elementid,3).setAttribute(XML_Sbox_Cnt_Value,sboxElement->groupCnt3);

    if(sboxElement->outPort0 != nullptr)
    {
        QList<QGraphicsItem*> childItemList = sboxElement->outPort0->childItems();
        QList<QGraphicsItem*>::iterator iterChild;
        for(iterChild=childItemList.begin();iterChild!=childItemList.end();iterChild++)
        {
            if((*iterChild))
            {
                ModuleWriteMem* writeMem = dynamic_cast<ModuleWriteMem*>(*iterChild);
                ModuleWriteRch* writeRch = dynamic_cast<ModuleWriteRch*>(*iterChild);
                ModuleWriteLfsr* writeLfsr = dynamic_cast<ModuleWriteLfsr*>(*iterChild);
                if(writeMem)
                {
                    setWriteMemAttribute(bcuid,rcuid,writeMem);
                }
                else if(writeRch)
                {
                    setWriteRchAttribute(bcuid,rcuid,writeRch);
                }
                else if(writeLfsr)
                {
                    setWriteLfsrAttribute(bcuid,rcuid,writeLfsr);
                }
            }
        }
    }
    if(sboxElement->outPort1 != nullptr)
    {
        QList<QGraphicsItem*> childItemList = sboxElement->outPort1->childItems();
        QList<QGraphicsItem*>::iterator iterChild;
        for(iterChild=childItemList.begin();iterChild!=childItemList.end();iterChild++)
        {
            if((*iterChild))
            {
                ModuleWriteMem* writeMem = dynamic_cast<ModuleWriteMem*>(*iterChild);
                ModuleWriteRch* writeRch = dynamic_cast<ModuleWriteRch*>(*iterChild);
                ModuleWriteLfsr* writeLfsr = dynamic_cast<ModuleWriteLfsr*>(*iterChild);
                if(writeMem)
                {
                    setWriteMemAttribute(bcuid,rcuid,writeMem);
                }
                else if(writeRch)
                {
                    setWriteRchAttribute(bcuid,rcuid,writeRch);
                }
                else if(writeLfsr)
                {
                    setWriteLfsrAttribute(bcuid,rcuid,writeLfsr);
                }
            }
        }
    }
    if(sboxElement->outPort2 != nullptr)
    {
        QList<QGraphicsItem*> childItemList = sboxElement->outPort2->childItems();
        QList<QGraphicsItem*>::iterator iterChild;
        for(iterChild=childItemList.begin();iterChild!=childItemList.end();iterChild++)
        {
            if((*iterChild))
            {
                ModuleWriteMem* writeMem = dynamic_cast<ModuleWriteMem*>(*iterChild);
                ModuleWriteRch* writeRch = dynamic_cast<ModuleWriteRch*>(*iterChild);
                ModuleWriteLfsr* writeLfsr = dynamic_cast<ModuleWriteLfsr*>(*iterChild);
                if(writeMem)
                {
                    setWriteMemAttribute(bcuid,rcuid,writeMem);
                }
                else if(writeRch)
                {
                    setWriteRchAttribute(bcuid,rcuid,writeRch);
                }
                else if(writeLfsr)
                {
                    setWriteLfsrAttribute(bcuid,rcuid,writeLfsr);
                }
            }
        }
    }
    if(sboxElement->outPort3 != nullptr)
    {
        QList<QGraphicsItem*> childItemList = sboxElement->outPort3->childItems();
        QList<QGraphicsItem*>::iterator iterChild;
        for(iterChild=childItemList.begin();iterChild!=childItemList.end();iterChild++)
        {
            if((*iterChild))
            {
                ModuleWriteMem* writeMem = dynamic_cast<ModuleWriteMem*>(*iterChild);
                ModuleWriteRch* writeRch = dynamic_cast<ModuleWriteRch*>(*iterChild);
                ModuleWriteLfsr* writeLfsr = dynamic_cast<ModuleWriteLfsr*>(*iterChild);
                if(writeMem)
                {
                    setWriteMemAttribute(bcuid,rcuid,writeMem);
                }
                else if(writeRch)
                {
                    setWriteRchAttribute(bcuid,rcuid,writeRch);
                }
                else if(writeLfsr)
                {
                    setWriteLfsrAttribute(bcuid,rcuid,writeLfsr);
                }
            }
        }
    }

}

void CXmlResolver::setBenesElementAttribute(ElementBenes* benesElement)
{
    if(benesElement == nullptr) return;

    int bcuid = benesElement->getIndexBcu();
    int rcuid = benesElement->getIndexRcu();
    int elementid = benesElement->getIndexInRcu();

    QDomElement element = findElement(bcuid,rcuid,ELEMENT_BENES,elementid);
    QRect benesRcet=benesElement->getRealBoundingRect().toRect();

    //    QPointF posAtParent = benesElement->mapToParent(benesElement->getRealBoundingRect().topLeft());
    QPointF posAtParent = benesElement->mapToParent(QPointF(0,0));

    element.setAttribute(XML_XAtParent,posAtParent.toPoint().x());
    element.setAttribute(XML_YAtParent,posAtParent.toPoint().y());
    element.setAttribute(XML_X,benesRcet.left());
    element.setAttribute(XML_Y,benesRcet.top());
    element.setAttribute(XML_Width,benesRcet.width());
    element.setAttribute(XML_Height,benesRcet.height());
    element.setAttribute(XML_Benes_Src,benesElement->srcConfig);

    if(benesElement->inPort0 != nullptr)
    {
        QDomElement eleInPort = findBENESInput(bcuid,rcuid,0);
        setInputPortToDom(&eleInPort,benesElement->inPort0);
    }
    if(benesElement->inPort1 != nullptr)
    {
        QDomElement eleInPort = findBENESInput(bcuid,rcuid,1);
        setInputPortToDom(&eleInPort,benesElement->inPort1);
    }
    if(benesElement->inPort2 != nullptr)
    {
        QDomElement eleInPort = findBENESInput(bcuid,rcuid,2);
        setInputPortToDom(&eleInPort,benesElement->inPort2);
    }
    if(benesElement->inPort3 != nullptr)
    {
        QDomElement eleInPort = findBENESInput(bcuid,rcuid,3);
        setInputPortToDom(&eleInPort,benesElement->inPort3);
    }

    if(benesElement->outPort0 != nullptr)
    {
        QList<QGraphicsItem*> childItemList = benesElement->outPort0->childItems();
        QList<QGraphicsItem*>::iterator iterChild;
        for(iterChild=childItemList.begin();iterChild!=childItemList.end();iterChild++)
        {
            if((*iterChild))
            {
                ModuleWriteMem* writeMem = dynamic_cast<ModuleWriteMem*>(*iterChild);
                ModuleWriteRch* writeRch = dynamic_cast<ModuleWriteRch*>(*iterChild);
                ModuleWriteLfsr* writeLfsr = dynamic_cast<ModuleWriteLfsr*>(*iterChild);
                if(writeMem)
                {
                    setWriteMemAttribute(bcuid,rcuid,writeMem);
                }
                else if(writeRch)
                {
                    setWriteRchAttribute(bcuid,rcuid,writeRch);
                }
                else if(writeLfsr)
                {
                    setWriteLfsrAttribute(bcuid,rcuid,writeLfsr);
                }
            }
        }
    }
    if(benesElement->outPort1 != nullptr)
    {
        QList<QGraphicsItem*> childItemList = benesElement->outPort1->childItems();
        QList<QGraphicsItem*>::iterator iterChild;
        for(iterChild=childItemList.begin();iterChild!=childItemList.end();iterChild++)
        {
            if((*iterChild))
            {
                ModuleWriteMem* writeMem = dynamic_cast<ModuleWriteMem*>(*iterChild);
                ModuleWriteRch* writeRch = dynamic_cast<ModuleWriteRch*>(*iterChild);
                ModuleWriteLfsr* writeLfsr = dynamic_cast<ModuleWriteLfsr*>(*iterChild);
                if(writeMem)
                {
                    setWriteMemAttribute(bcuid,rcuid,writeMem);
                }
                else if(writeRch)
                {
                    setWriteRchAttribute(bcuid,rcuid,writeRch);
                }
                else if(writeLfsr)
                {
                    setWriteLfsrAttribute(bcuid,rcuid,writeLfsr);
                }
            }
        }
    }
    if(benesElement->outPort2 != nullptr)
    {
        QList<QGraphicsItem*> childItemList = benesElement->outPort2->childItems();
        QList<QGraphicsItem*>::iterator iterChild;
        for(iterChild=childItemList.begin();iterChild!=childItemList.end();iterChild++)
        {
            if((*iterChild))
            {
                ModuleWriteMem* writeMem = dynamic_cast<ModuleWriteMem*>(*iterChild);
                ModuleWriteRch* writeRch = dynamic_cast<ModuleWriteRch*>(*iterChild);
                ModuleWriteLfsr* writeLfsr = dynamic_cast<ModuleWriteLfsr*>(*iterChild);
                if(writeMem)
                {
                    setWriteMemAttribute(bcuid,rcuid,writeMem);
                }
                else if(writeRch)
                {
                    setWriteRchAttribute(bcuid,rcuid,writeRch);
                }
                else if(writeLfsr)
                {
                    setWriteLfsrAttribute(bcuid,rcuid,writeLfsr);
                }
            }
        }
    }
    if(benesElement->outPort3 != nullptr)
    {
        QList<QGraphicsItem*> childItemList = benesElement->outPort3->childItems();
        QList<QGraphicsItem*>::iterator iterChild;
        for(iterChild=childItemList.begin();iterChild!=childItemList.end();iterChild++)
        {
            if((*iterChild))
            {
                ModuleWriteMem* writeMem = dynamic_cast<ModuleWriteMem*>(*iterChild);
                ModuleWriteRch* writeRch = dynamic_cast<ModuleWriteRch*>(*iterChild);
                ModuleWriteLfsr* writeLfsr = dynamic_cast<ModuleWriteLfsr*>(*iterChild);
                if(writeMem)
                {
                    setWriteMemAttribute(bcuid,rcuid,writeMem);
                }
                else if(writeRch)
                {
                    setWriteRchAttribute(bcuid,rcuid,writeRch);
                }
                else if(writeLfsr)
                {
                    setWriteLfsrAttribute(bcuid,rcuid,writeLfsr);
                }
            }
        }
    }

}

void CXmlResolver::setWriteMemAttribute(int bcuIndex,int rcuIndex,ModuleWriteMem* writeMem)
{
    QL_DEBUG;
    if(writeMem == nullptr) return;

    QDomElement element = findRCUMEMElement(bcuIndex,rcuIndex,writeMem->getWriteMemIndex());
    element.setAttribute(XML_InPort_Type,writeMem->getInputType());
    element.setAttribute(XML_InPort_Index,writeMem->getInputIndex());
}

void CXmlResolver::setWriteRchAttribute(int bcuIndex, int rcuIndex, ModuleWriteRch *writeRch)
{
    Q_UNUSED(rcuIndex);
    QL_DEBUG;
    if(writeRch == NULL) return;

    QDomElement element = findRCURCHElement(bcuIndex,rcuIndex,writeRch->getWriteRchIndex());
    element.setAttribute(XML_InPort_Type,writeRch->getInputType());
    element.setAttribute(XML_InPort_Index,writeRch->getInputIndex());
    element.setAttribute(XML_InPort_Address,writeRch->getWriteAddress());
//    element.setAttribute(XML_InPort_RcuIndex,writeRch->getIndexRcu());
}

void CXmlResolver::setWriteLfsrAttribute(int bcuIndex, int rcuIndex, ModuleWriteLfsr *writeLfsr)
{
    Q_UNUSED(rcuIndex);
    QL_DEBUG;
    if(writeLfsr == NULL) return;

    QDomElement element = findRCULFSRElement(bcuIndex,rcuIndex,writeLfsr->getWriteLfsrIndex());
    element.setAttribute(XML_InPort_Type,writeLfsr->getInputType());
    element.setAttribute(XML_InPort_Index,writeLfsr->getInputIndex());
}

void CXmlResolver::setRchAttribute(int bcuIndex, int rcuIndex, ModuleWriteRch *writeRch)
{
    if(writeRch == nullptr) return;

    QDomElement element = findRCURCHElement(bcuIndex,rcuIndex,writeRch->getWriteRchIndex());
    element.setAttribute(XML_InPort_Type,writeRch->getInputType());
    element.setAttribute(XML_InPort_Index,writeRch->getInputIndex());
}
/**
 * CXmlResolver::setElementAttributeDouble
 * \brief
 * \param   bcuid
 * \param   rcuid
 * \param   element
 * \param   elementid
 * \param   attrID
 * \param   value
 * \author  zhangjun
 * \date    2016-10-12
 */
void CXmlResolver::setElementAttributeDouble(int bcuid, int rcuid, ElementType element, int elementid, AttributeID attrID, double value)
{
    switch(attrID)
    {
    case PosX:
    {
        findElement(bcuid,rcuid,element,elementid).setAttribute("X",value);
        break;
    }
    case PosY:
    {
        findElement(bcuid,rcuid,element,elementid).setAttribute("Y",value);
        break;
    }
    default:break;
    }
}
/**
 * CXmlResolver::setElementAttributeStr
 * \brief
 * \param   bcuid
 * \param   rcuid
 * \param   element
 * \param   elementid
 * \param   attrID
 * \param   value
 * \author  zhangjun
 * \date    2016-10-12
 */
void CXmlResolver::setElementAttributeStr(int bcuid, int rcuid, ElementType element, int elementid, AttributeID attrID, QString value)
{
    Q_UNUSED(element)
    switch(attrID)
    {
    case BfuFuncExp:
    {
        findElement(bcuid,rcuid,ELEMENT_BFU,elementid).setAttribute("exp",value);
        break;
    }
    case BfuBypassExp:
    {
        findElement(bcuid,rcuid,ELEMENT_BFU,elementid).setAttribute("BypassExp",value);
        break;
    }
    case SboxSrc:
    {
        findElement(bcuid,rcuid,ELEMENT_SBOX,elementid).setAttribute("src",value);
        break;
    }
    case BenesSrc:
    {
        findElement(bcuid,rcuid,ELEMENT_BENES,elementid).setAttribute("src",value);
        break;
    }
    default:break;
    }
}

/**
 * CXmlResolver::setElementAttribute
 * \brief
 * \param   bcuid
 * \param   rcuid
 * \param   element
 * \param   elementid
 * \param   attrID
 * \param   value
 * \author  zhangjun
 * \date    2016-10-12
 */
void CXmlResolver::setElementAttribute(int bcuid, int rcuid, ElementType element, int elementid, AttributeID attrID, int value)
{
    switch(attrID)
    {
    case PosX:
    {
        findElement(bcuid,rcuid,element,elementid).setAttribute("X",value);
        break;
    }
    case PosY:
    {
        findElement(bcuid,rcuid,element,elementid).setAttribute("Y",value);
        break;
    }
    case BfuFunc:
    {
        findElement(bcuid,rcuid,ELEMENT_BFU,elementid).setAttribute("func",m_funcMap.value(value));
        break;
    }
    case BfuAUMod:
    {
        findElement(bcuid,rcuid,ELEMENT_BFU,elementid).setAttribute("mod",value);
        break;
    }
    case BfuBypass:
    {
        findElement(bcuid,rcuid,ELEMENT_BFU,elementid).setAttribute("Bypass",value);
        break;
    }
    case BfuInputA_Type:
    {
        findBFUInput(bcuid,rcuid,elementid,INPUT_A).setAttribute("Type",value);
        break;
    }
    case BfuInputA_Index:
    {
        findBFUInput(bcuid,rcuid,elementid,INPUT_A).setAttribute("Index",value);
        break;
    }
    case BfuInputA_Height:
    {
        findBFUInput(bcuid,rcuid,elementid,INPUT_A).setAttribute("Height",value);
        break;
    }
    case BfuInputB_Type:
    {
        findBFUInput(bcuid,rcuid,elementid,INPUT_B).setAttribute("Type",value);
        break;
    }
    case BfuInputB_Index:
    {
        findBFUInput(bcuid,rcuid,elementid,INPUT_B).setAttribute("Index",value);
        break;
    }
    case BfuInputB_Height:
    {
        findBFUInput(bcuid,rcuid,elementid,INPUT_B).setAttribute("Height",value);
        break;
    }
    case BfuInputT_Type:
    {
        findBFUInput(bcuid,rcuid,elementid,INPUT_T).setAttribute("Type",value);
        break;
    }
    case BfuInputT_Index:
    {
        findBFUInput(bcuid,rcuid,elementid,INPUT_T).setAttribute("Index",value);
        break;
    }
    case BfuInputT_Height:
    {
        findBFUInput(bcuid,rcuid,elementid,INPUT_T).setAttribute("Height",value);
        break;
    }
    case SboxInput_Type:
    {
        findSBOXInput(bcuid,rcuid,elementid).setAttribute("Type",value);
        break;
    }
    case SboxInput_Index:
    {
        findSBOXInput(bcuid,rcuid,elementid).setAttribute("Index",value);
        break;
    }
    case SboxInput_Height:
    {
        findSBOXInput(bcuid,rcuid,elementid).setAttribute("Height",value);
        break;
    }
    case SboxCNT0:
    {
        findSBOXCnt(bcuid,rcuid,elementid,0).setAttribute("value",value);
        break;
    }
    case SboxCNT1:
    {
        findSBOXCnt(bcuid,rcuid,elementid,1).setAttribute("value",value);
        break;
    }
    case SboxCNT2:
    {
        findSBOXCnt(bcuid,rcuid,elementid,2).setAttribute("value",value);
        break;
    }
    case SboxCNT3:
    {
        findSBOXCnt(bcuid,rcuid,elementid,3).setAttribute("value",value);
        break;
    }
    case SboxMode:
    {
        findElement(bcuid,rcuid,element,elementid).setAttribute("mode",value);
        break;
    }
    case BenesInput0_Type:
    {
        findBENESInput(bcuid,rcuid,0).setAttribute("Type",value);
        break;
    }
    case BenesInput0_Index:
    {
        findBENESInput(bcuid,rcuid,0).setAttribute("Index",value);
        break;
    }
    case BenesInput0_Height:
    {
        findBENESInput(bcuid,rcuid,0).setAttribute("Height",value);
        break;
    }
    case BenesInput1_Type:
    {
        findBENESInput(bcuid,rcuid,1).setAttribute("Type",value);
        break;
    }
    case BenesInput1_Index:
    {
        findBENESInput(bcuid,rcuid,1).setAttribute("Index",value);
        break;
    }
    case BenesInput1_Height:
    {
        findBENESInput(bcuid,rcuid,1).setAttribute("Height",value);
        break;
    }
    case BenesInput2_Type:
    {
        findBENESInput(bcuid,rcuid,2).setAttribute("Type",value);
        break;
    }
    case BenesInput2_Index:
    {
        findBENESInput(bcuid,rcuid,2).setAttribute("Index",value);
        break;
    }
    case BenesInput2_Height:
    {
        findBENESInput(bcuid,rcuid,2).setAttribute("Height",value);
        break;
    }
    case BenesInput3_Type:
    {
        findBENESInput(bcuid,rcuid,3).setAttribute("Type",value);
        break;
    }
    case BenesInput3_Index:
    {
        findBENESInput(bcuid,rcuid,3).setAttribute("Index",value);
        break;
    }
    case BenesInput3_Height:
    {
        findBENESInput(bcuid,rcuid,3).setAttribute("Height",value);
        break;
    }
    case RCUMEM0_Type:
    {
        findRCUMEMElement(bcuid,rcuid,0).setAttribute("Type",value);
        break;
    }
    case RCUMEM0_Index:
    {
        findRCUMEMElement(bcuid,rcuid,0).setAttribute("Index",value);
        break;
    }
    case RCUMEM0_Height:
    {
        findRCUMEMElement(bcuid,rcuid,0).setAttribute("Height",value);
        break;
    }
    case RCUMEM1_Type:
    {
        findRCUMEMElement(bcuid,rcuid,1).setAttribute("Type",value);
        break;
    }
    case RCUMEM1_Index:
    {
        findRCUMEMElement(bcuid,rcuid,1).setAttribute("Index",value);
        break;
    }
    case RCUMEM1_Height:
    {
        findRCUMEMElement(bcuid,rcuid,1).setAttribute("Height",value);
        break;
    }
    case RCUMEM2_Type:
    {
        findRCUMEMElement(bcuid,rcuid,2).setAttribute("Type",value);
        break;
    }
    case RCUMEM2_Index:
    {
        findRCUMEMElement(bcuid,rcuid,2).setAttribute("Index",value);
        break;
    }
    case RCUMEM2_Height:
    {
        findRCUMEMElement(bcuid,rcuid,2).setAttribute("Height",value);
        break;
    }
    case RCUMEM3_Type:
    {
        findRCUMEMElement(bcuid,rcuid,3).setAttribute("Type",value);
        break;
    }
    case RCUMEM3_Index:
    {
        findRCUMEMElement(bcuid,rcuid,3).setAttribute("Index",value);
        break;
    }
    case RCUMEM3_Height:
    {
        findRCUMEMElement(bcuid,rcuid,3).setAttribute("Height",value);
        break;
    }
    case RCUOUTFIFO0_Type:
    {
        findRCUOUTElement(bcuid,rcuid,0).setAttribute("Type",value);
        break;
    }
    case RCUOUTFIFO0_Index:
    {
        findRCUOUTElement(bcuid,rcuid,0).setAttribute("Index",value);
        break;
    }
    case RCUOUTFIFO0_Height:
    {
        findRCUOUTElement(bcuid,rcuid,0).setAttribute("Height",value);
        break;
    }
    case RCUOUTFIFO1_Type:
    {
        findRCUOUTElement(bcuid,rcuid,0).setAttribute("Type",value);
        break;
    }
    case RCUOUTFIFO1_Index:
    {
        findRCUOUTElement(bcuid,rcuid,0).setAttribute("Index",value);
        break;
    }
    case RCUOUTFIFO1_Height:
    {
        findRCUOUTElement(bcuid,rcuid,0).setAttribute("Height",value);
        break;
    }
    case RCUOUTFIFO2_Type:
    {
        findRCUOUTElement(bcuid,rcuid,0).setAttribute("Type",value);
        break;
    }
    case RCUOUTFIFO2_Index:
    {
        findRCUOUTElement(bcuid,rcuid,0).setAttribute("Index",value);
        break;
    }
    case RCUOUTFIFO2_Height:
    {
        findRCUOUTElement(bcuid,rcuid,0).setAttribute("Height",value);
        break;
    }
    case RCUOUTFIFO3_Type:
    {
        findRCUOUTElement(bcuid,rcuid,0).setAttribute("Type",value);
        break;
    }
    case RCUOUTFIFO3_Index:
    {
        findRCUOUTElement(bcuid,rcuid,0).setAttribute("Index",value);
        break;
    }
    case RCUOUTFIFO3_Height:
    {
        findRCUOUTElement(bcuid,rcuid,0).setAttribute("Height",value);
        break;
    }
    case BCUROUTER:
    {
        findBCUElement(bcuid).setAttribute("router",value);
        break;
    }
    case RCUMEMWTMODE:
    {
        findRCUMemWTElement(bcuid,rcuid).setAttribute("mode",value);
        break;
    }
    case RCUMEMWTADDR:
    {
        findRCUMemWTElement(bcuid,rcuid).setAttribute("addr",value);
        break;
    }
    case RCUMEMWTMASK:
    {
        findRCUMemWTElement(bcuid,rcuid).setAttribute("mask",value);
        break;
    }
    case RCUMEMWTOFFSET:
    {
        findRCUMemWTElement(bcuid,rcuid).setAttribute("offset",value);
        break;
    }
    case RCUMEMWTTHREASHOLD:
    {
        findRCUMemWTElement(bcuid,rcuid).setAttribute("threashold",value);
        break;
    }
    case RCUMEMRDMODE128_32:
    {
        findRCUMemRDElement(bcuid,rcuid).setAttribute("mode128_32",value);
        break;
    }
    case RCUMEMRDMODE:
    {
        findRCUMemRDElement(bcuid,rcuid).setAttribute("mode",value);
        break;
    }
    case RCUMEMRDADDR1:
    {
        findRCUMemRDElement(bcuid,rcuid).setAttribute("addr1",value);
        break;
    }
    case RCUMEMRDADDR2:
    {
        findRCUMemRDElement(bcuid,rcuid).setAttribute("addr2",value);
        break;
    }
    case RCUMEMRDADDR3:
    {
        findRCUMemRDElement(bcuid,rcuid).setAttribute("addr3",value);
        break;
    }
    case RCUMEMRDADDR4:
    {
        findRCUMemRDElement(bcuid,rcuid).setAttribute("addr4",value);
        break;
    }
    case RCUMEMRDOFFSET:
    {
        findRCUMemRDElement(bcuid,rcuid).setAttribute("offset",value);
        break;
    }
    case RCUMEMRDTHREASHOLD:
    {
        findRCUMemRDElement(bcuid,rcuid).setAttribute("threashold",value);
        break;
    }
    default:break;
    }
}
/**
 * CXmlResolver::getElementAttributeStr
 * \brief
 * \param   bcuid
 * \param   rcuid
 * \param   element
 * \param   elementid
 * \param   attrID
 * \param   defaultValue
 * \return  QString
 * \author  zhangjun
 * \date    2016-10-12
 */
QString CXmlResolver::getElementAttributeStr(int bcuid, int rcuid, ElementType element, int elementid, AttributeID attrID, QString defaultValue)
{
    Q_UNUSED(element)
    Q_UNUSED(defaultValue)
    switch(attrID)
    {
    case BfuFunc:
    {
        return findElement(bcuid,rcuid,ELEMENT_BFU,elementid).attribute("func");
    }
    case BfuFuncExp:
    {
        return findElement(bcuid,rcuid,ELEMENT_BFU,elementid).attribute("exp");
    }
    case BfuAUMod:
    {
        return m_auModMap.value(findElement(bcuid,rcuid,ELEMENT_BFU,elementid).attribute("mod").toInt());
    }
    case BfuBypassExp:
    {
        return findElement(bcuid,rcuid,ELEMENT_BFU,elementid).attribute("BypassExp");
    }
    case BfuBypass:
    {
        return m_bypassMap.value(findElement(bcuid,rcuid,ELEMENT_BFU,elementid).attribute("Bypass").toInt());
    }
    case BfuInputA_Type:
    {
        return m_inputTypeMap.value(findBFUInput(bcuid,rcuid,elementid,INPUT_A).attribute("Type").toInt());
    }
    case BfuInputB_Type:
    {
        return m_inputTypeMap.value(findBFUInput(bcuid,rcuid,elementid,INPUT_B).attribute("Type").toInt());
    }
    case BfuInputT_Type:
    {
        return m_inputTypeMap.value(findBFUInput(bcuid,rcuid,elementid,INPUT_T).attribute("Type").toInt());
    }
    case SboxInput_Type:
    {
        return m_inputTypeMap.value(findSBOXInput(bcuid,rcuid,elementid).attribute("Type").toInt());
    }
    case BenesInput0_Type:
    {
        return m_inputTypeMap.value(findBENESInput(bcuid,rcuid,0).attribute("Type").toInt());
    }
    case BenesInput1_Type:
    {
        return m_inputTypeMap.value(findBENESInput(bcuid,rcuid,1).attribute("Type").toInt());
    }
    case BenesInput2_Type:
    {
        return m_inputTypeMap.value(findBENESInput(bcuid,rcuid,2).attribute("Type").toInt());
    }
    case BenesInput3_Type:
    {
        return m_inputTypeMap.value(findBENESInput(bcuid,rcuid,3).attribute("Type").toInt());
    }
    case BenesSrc:
    {
        return findElement(bcuid,rcuid,ELEMENT_BENES,elementid).attribute("src");
    }
    case SboxSrc:
    {
        return findElement(bcuid,rcuid,ELEMENT_SBOX,elementid).attribute("src");
    }
    case RCUMEM0_Type:
    {
        return m_inputTypeMap.value(findRCUMEMElement(bcuid,rcuid,0).attribute("Type").toInt());
    }
    case RCUMEM1_Type:
    {
        return m_inputTypeMap.value(findRCUMEMElement(bcuid,rcuid,1).attribute("Type").toInt());
    }
    case RCUMEM2_Type:
    {
        return m_inputTypeMap.value(findRCUMEMElement(bcuid,rcuid,2).attribute("Type").toInt());
    }
    case RCUMEM3_Type:
    {
        return m_inputTypeMap.value(findRCUMEMElement(bcuid,rcuid,3).attribute("Type").toInt());
    }
    case RCUOUTFIFO0_Type:
    {
        return m_inputTypeMap.value(findRCUOUTElement(bcuid,rcuid,0).attribute("Type").toInt());
    }
    case RCUOUTFIFO1_Type:
    {
        return m_inputTypeMap.value(findRCUOUTElement(bcuid,rcuid,1).attribute("Type").toInt());
    }
    case RCUOUTFIFO2_Type:
    {
        return m_inputTypeMap.value(findRCUOUTElement(bcuid,rcuid,2).attribute("Type").toInt());
    }
    case RCUOUTFIFO3_Type:
    {
        return m_inputTypeMap.value(findRCUOUTElement(bcuid,rcuid,3).attribute("Type").toInt());
    }
    case RCUMEMWTMASK:
    {
        int i = findRCUMemWTElement(bcuid,rcuid).attribute("mask").toInt();
        return "'"+QString("%0").arg(i,4,2,QLatin1Char('0'));
    }
    case RCUMEMWTADDR:
    {
        return "0x"+QString::number(findRCUMemWTElement(bcuid,rcuid).attribute("addr").toInt(),16);
    }
    case RCUMEMRDADDR1:
    {
        return "0x"+QString::number(findRCUMemRDElement(bcuid,rcuid).attribute("addr1").toInt(),16);
    }
    case RCUMEMRDADDR2:
    {
        return "0x"+QString::number(findRCUMemRDElement(bcuid,rcuid).attribute("addr2").toInt(),16);
    }
    case RCUMEMRDADDR3:
    {
        return "0x"+QString::number(findRCUMemRDElement(bcuid,rcuid).attribute("addr3").toInt(),16);
    }
    case RCUMEMRDADDR4:
    {
        return "0x"+QString::number(findRCUMemRDElement(bcuid,rcuid).attribute("addr4").toInt(),16);
    }
    case RCUMEMWTMODE:
    {
        return m_memModeMap.value(findRCUMemWTElement(bcuid,rcuid).attribute("mode").toInt());
    }
    case RCUMEMRDMODE:
    {
        return m_memModeMap.value(findRCUMemRDElement(bcuid,rcuid).attribute("mode").toInt());
    }
    case RCUMEMWTOFFSET:
    {
        return "0x"+QString::number(findRCUMemWTElement(bcuid,rcuid).attribute("offset").toInt(),16);
    }
    case RCUMEMWTTHREASHOLD:
    {
        if(findRCUMemWTElement(bcuid,rcuid).attribute("mode").toInt() == 3)
            return "0x"+QString::number(findRCUMemWTElement(bcuid,rcuid).attribute("threashold").toInt(),16);
        else
            return QString::number(findRCUMemWTElement(bcuid,rcuid).attribute("threashold").toInt(),10);
    }
    case RCUMEMRDOFFSET:
    {
        return "0x"+QString::number(findRCUMemRDElement(bcuid,rcuid).attribute("offset").toInt(),16);
    }
    case RCUMEMRDTHREASHOLD:
    {
        if(findRCUMemRDElement(bcuid,rcuid).attribute("mode").toInt() == 3)
            return "0x"+QString::number(findRCUMemRDElement(bcuid,rcuid).attribute("threashold").toInt(),16);
        else
            return QString::number(findRCUMemRDElement(bcuid,rcuid).attribute("threashold").toInt(),10);

    }
    case RCUMEMRDMODE128_32:
    {
        return findRCUMemRDElement(bcuid,rcuid).attribute("mode128_32").toInt()==0?trUtf8("128位"):trUtf8("32位");
    }
    case SboxMode:
    {
        return m_sboxModeMap.value(findElement(bcuid,rcuid,ELEMENT_SBOX,elementid).attribute("mode").toInt());
    }
    default:return "";
    }
}
/**
 * CXmlResolver::getElementAttributeValue
 * \brief
 * \param   bcuid
 * \param   rcuid
 * \param   element
 * \param   elementid
 * \param   attrID
 * \param   defaultValue
 * \return  int
 * \author  zhangjun
 * \date    2016-10-12
 */
int CXmlResolver::getElementAttributeValue(int bcuid, int rcuid, ElementType element, int elementid, AttributeID attrID, int defaultValue)
{
    switch(attrID)
    {
    case PosX:
    {
        return findElement(bcuid,rcuid,element,elementid).attribute("X").toInt();
    }
    case PosY:
    {
        return findElement(bcuid,rcuid,element,elementid).attribute("Y").toInt();
    }
    case BfuFunc:
    {
        QString func = findElement(bcuid,rcuid,ELEMENT_BFU,elementid).attribute("func");
        return func=="au"?0:func=="lu"?1:func=="su"?2:0;
    }
    case BfuBypass:
    {
        return findElement(bcuid,rcuid,element,elementid).attribute("Bypass").toInt();
    }
    case BfuInputA_Type:
    {
        return findBFUInput(bcuid,rcuid,elementid,INPUT_A).attribute("Type").toInt();
    }
    case BfuInputA_Height:
    {
        return findBFUInput(bcuid,rcuid,elementid,INPUT_A).attribute("Height").toInt();
    }
    case BfuInputB_Type:
    {
        return findBFUInput(bcuid,rcuid,elementid,INPUT_B).attribute("Type").toInt();
    }
    case BfuInputB_Height:
    {
        return findBFUInput(bcuid,rcuid,elementid,INPUT_B).attribute("Height").toInt();
    }
    case BfuInputT_Type:
    {
        return findBFUInput(bcuid,rcuid,elementid,INPUT_T).attribute("Type").toInt();
    }
    case BfuInputT_Height:
    {
        return findBFUInput(bcuid,rcuid,elementid,INPUT_T).attribute("Height").toInt();
    }
    case SboxInput_Type:
    {
        return findSBOXInput(bcuid,rcuid,elementid).attribute("Type").toInt();
    }
    case SboxInput_Height:
    {
        return findSBOXInput(bcuid,rcuid,elementid).attribute("Height").toInt();
    }
    case BenesInput0_Type:
    {
        return findBENESInput(bcuid,rcuid,0).attribute("Type").toInt();
    }
    case BenesInput0_Height:
    {
        return findBENESInput(bcuid,rcuid,0).attribute("Height").toInt();
    }
    case BenesInput1_Height:
    {
        return findBENESInput(bcuid,rcuid,1).attribute("Height").toInt();
    }
    case BenesInput2_Height:
    {
        return findBENESInput(bcuid,rcuid,2).attribute("Height").toInt();
    }
    case BenesInput3_Height:
    {
        return findBENESInput(bcuid,rcuid,3).attribute("Height").toInt();
    }
    case BenesInput1_Type:
    {
        return findBENESInput(bcuid,rcuid,1).attribute("Type").toInt();
    }
    case BenesInput2_Type:
    {
        return findBENESInput(bcuid,rcuid,2).attribute("Type").toInt();
    }
    case BenesInput3_Type:
    {
        return findBENESInput(bcuid,rcuid,3).attribute("Type").toInt();
    }
    case RCUMEM0_Type:
    {
        return findRCUMEMElement(bcuid,rcuid,0).attribute("Type").toInt();
    }
    case RCUMEM0_Height:
    {
        return findRCUMEMElement(bcuid,rcuid,0).attribute("Height").toInt();
    }
    case RCUMEM1_Height:
    {
        return findRCUMEMElement(bcuid,rcuid,1).attribute("Height").toInt();
    }
    case RCUMEM2_Height:
    {
        return findRCUMEMElement(bcuid,rcuid,2).attribute("Height").toInt();
    }
    case RCUMEM3_Height:
    {
        return findRCUMEMElement(bcuid,rcuid,3).attribute("Height").toInt();
    }
    case RCUMEM1_Type:
    {
        return findRCUMEMElement(bcuid,rcuid,1).attribute("Type").toInt();
    }
    case RCUMEM2_Type:
    {
        return findRCUMEMElement(bcuid,rcuid,2).attribute("Type").toInt();
    }
    case RCUMEM3_Type:
    {
        return findRCUMEMElement(bcuid,rcuid,3).attribute("Type").toInt();
    }
    case RCUOUTFIFO0_Height:
    {
        return findRCUOUTElement(bcuid,rcuid,0).attribute("Height").toInt();
    }
    case RCUOUTFIFO1_Height:
    {
        return findRCUOUTElement(bcuid,rcuid,1).attribute("Height").toInt();
    }
    case RCUOUTFIFO2_Height:
    {
        return findRCUOUTElement(bcuid,rcuid,2).attribute("Height").toInt();
    }
    case RCUOUTFIFO3_Height:
    {
        return findRCUOUTElement(bcuid,rcuid,3).attribute("Height").toInt();
    }
    case RCUOUTFIFO0_Type:
    {
        return findRCUOUTElement(bcuid,rcuid,0).attribute("Type").toInt();
    }
    case RCUOUTFIFO1_Type:
    {
        return findRCUOUTElement(bcuid,rcuid,1).attribute("Type").toInt();
    }
    case RCUOUTFIFO2_Type:
    {
        return findRCUOUTElement(bcuid,rcuid,2).attribute("Type").toInt();
    }
    case RCUOUTFIFO3_Type:
    {
        return findRCUOUTElement(bcuid,rcuid,3).attribute("Type").toInt();
    }
    case BfuInputA_Index:
    {
        return findBFUInput(bcuid,rcuid,elementid,INPUT_A).attribute("Index").toInt();
    }
    case BfuInputB_Index:
    {
        return findBFUInput(bcuid,rcuid,elementid,INPUT_B).attribute("Index").toInt();
    }
    case BfuInputT_Index:
    {
        return findBFUInput(bcuid,rcuid,elementid,INPUT_T).attribute("Index").toInt();
    }
    case SboxInput_Index:
    {
        return findSBOXInput(bcuid,rcuid,elementid).attribute("Index").toInt();
    }
    case SboxCNT0:
    {
        return findSBOXCnt(bcuid,rcuid,elementid,0).attribute("value").toInt();
    }
    case SboxCNT1:
    {
        return findSBOXCnt(bcuid,rcuid,elementid,1).attribute("value").toInt();
    }
    case SboxCNT2:
    {
        return findSBOXCnt(bcuid,rcuid,elementid,2).attribute("value").toInt();
    }
    case SboxCNT3:
    {
        return findSBOXCnt(bcuid,rcuid,elementid,3).attribute("value").toInt();
    }
    case SboxMode:
    {
        return findElement(bcuid,rcuid,element,elementid).attribute("mode").toInt();
    }
    case BenesInput0_Index:
    {
        return findBENESInput(bcuid,rcuid,0).attribute("Index").toInt();
    }
    case BenesInput1_Index:
    {
        return findBENESInput(bcuid,rcuid,1).attribute("Index").toInt();
    }
    case BenesInput2_Index:
    {
        return findBENESInput(bcuid,rcuid,2).attribute("Index").toInt();
    }
    case BenesInput3_Index:
    {
        return findBENESInput(bcuid,rcuid,3).attribute("Index").toInt();
    }
    case RCUMEM0_Index:
    {
        return findRCUMEMElement(bcuid,rcuid,0).attribute("Index").toInt();
    }
    case RCUMEM1_Index:
    {
        return findRCUMEMElement(bcuid,rcuid,1).attribute("Index").toInt();
    }
    case RCUMEM2_Index:
    {
        return findRCUMEMElement(bcuid,rcuid,2).attribute("Index").toInt();
    }
    case RCUMEM3_Index:
    {
        return findRCUMEMElement(bcuid,rcuid,3).attribute("Index").toInt();
    }
    case RCUOUTFIFO0_Index:
    {
        return findRCUOUTElement(bcuid,rcuid,0).attribute("Index").toInt();
    }
    case RCUOUTFIFO1_Index:
    {
        return findRCUOUTElement(bcuid,rcuid,1).attribute("Index").toInt();
    }
    case RCUOUTFIFO2_Index:
    {
        return findRCUOUTElement(bcuid,rcuid,2).attribute("Index").toInt();
    }
    case RCUOUTFIFO3_Index:
    {
        return findRCUOUTElement(bcuid,rcuid,3).attribute("Index").toInt();
    }
    case BCUROUTER:
    {
        return findBCUElement(bcuid).attribute("router").toInt();
    }
    case RCUMEMWTMODE:
    {
        return findRCUMemWTElement(bcuid,rcuid).attribute("mode").toInt();
    }
    case RCUMEMWTADDR:
    {
        return findRCUMemWTElement(bcuid,rcuid).attribute("addr").toInt();
    }
    case RCUMEMWTMASK:
    {
        return findRCUMemWTElement(bcuid,rcuid).attribute("mask").toInt();
    }
    case RCUMEMWTOFFSET:
    {
        return findRCUMemWTElement(bcuid,rcuid).attribute("offset").toInt();
    }
    case RCUMEMWTTHREASHOLD:
    {
        return findRCUMemWTElement(bcuid,rcuid).attribute("threashold").toInt();
    }
    case RCUMEMRDMODE128_32:
    {
        return findRCUMemRDElement(bcuid,rcuid).attribute("mode128_32").toInt();
    }
    case RCUMEMRDMODE:
    {
        return findRCUMemRDElement(bcuid,rcuid).attribute("mode").toInt();
    }
    case RCUMEMRDADDR1:
    {
        return findRCUMemRDElement(bcuid,rcuid).attribute("addr1").toInt();
    }
    case RCUMEMRDADDR2:
    {
        return findRCUMemRDElement(bcuid,rcuid).attribute("addr2").toInt();
    }
    case RCUMEMRDADDR3:
    {
        return findRCUMemRDElement(bcuid,rcuid).attribute("addr3").toInt();
    }
    case RCUMEMRDADDR4:
    {
        return findRCUMemRDElement(bcuid,rcuid).attribute("addr4").toInt();
    }
    case RCUMEMRDOFFSET:
    {
        return findRCUMemRDElement(bcuid,rcuid).attribute("offset").toInt();
    }
    case RCUMEMRDTHREASHOLD:
    {
        return findRCUMemRDElement(bcuid,rcuid).attribute("threashold").toInt();
    }
    default:return defaultValue;
    }
}

/**
 * CXmlResolver::getElementAttributeValueDouble
 * \brief
 * \param   bcuid
 * \param   rcuid
 * \param   element
 * \param   elementid
 * \param   attrID
 * \param   defaultValue
 * \return  double
 * \author  zhangjun
 * \date    2016-10-12
 */
double CXmlResolver::getElementAttributeValueDouble(int bcuid, int rcuid, ElementType element, int elementid, AttributeID attrID, int defaultValue)
{
    switch(attrID)
    {
    case PosX:
    {
        return findElement(bcuid,rcuid,element,elementid).attribute("X").toDouble();
    }
    case PosY:
    {
        return findElement(bcuid,rcuid,element,elementid).attribute("Y").toDouble();
    }
    default:return defaultValue;
    }
}
