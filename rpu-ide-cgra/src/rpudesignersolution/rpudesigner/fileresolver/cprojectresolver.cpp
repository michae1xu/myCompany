/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    cprojectresolver.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QTextStream>
#include <QMessageBox>
#include <QCryptographicHash>
#include <memory>
#include "common/cappenv.hpp"
#include "common/debug.h"
#include "cprojectresolver.h"
//#include "fileresolver/xmlresolver.h"

/**
 * CProjectResolver::CProjectResolver
 * \brief   constructor of CProjectResolver
 * \param   fileName
 * \author  zhangjun
 * \date    2016-10-12
 */
CProjectResolver::CProjectResolver(const QString &fileName, const QString &solutionName, ECipherType cipherType) :
    m_fileName(fileName)
{
    m_projectParameter = new SProjectParameter;
    m_projectParameter->solutionName = solutionName;
    m_projectParameter->projectName = fileName;
    m_projectParameter->cipherType = cipherType;
    m_document = new QDomDocument();

    m_file = new QFile(fileName);
    if(!m_file->open(QFile::ReadOnly | QFile::Text))
    {
        m_isStart = false;
    }
    m_isStart = initProInfo();
}

/**
 * CProjectResolver::~CProjectResolver
 * \brief   destructor of CProjectResolver
 * \author  zhangjun
 * \date    2016-10-12
 */
CProjectResolver::~CProjectResolver()
{
    if(m_file && m_file->isOpen())
    {
        m_file->close();
        m_file->deleteLater();
    }

    delete m_document;
    delete m_projectParameter;
}

/*!
 * CProjectResolver::md5Check
 * \brief   md5校验
 * \return
 * \author  zhangjun
 * \date    2016-10-20
 */
//bool CProjectResolver::md5Check()
//{
//    qInfo() << tr(u8"项目解析器：开始对项目文件“%0”进行MD5校验……").arg(m_projectParameter->projectName);
//    QFile file(m_projectParameter->projectName);
//    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
//    {
//        return false;
//    }
//    QTextStream in(&file);
//    QString fileContent;
//    QString md5InFile;
//    while(!in.atEnd())
//    {
//        QString line = in.readLine().trimmed();
//        if(line.startsWith("<MD5"))
//        {
//            md5InFile = line.mid(line.indexOf("\"") + 1);
//            md5InFile = md5InFile.left(md5InFile.indexOf("\""));
//            continue;
//        }
//        if(line.isEmpty() || line.startsWith("<?xml"))
//        {
//            continue;
//        }
//        fileContent.append(line);
//    }
//    file.close();
//    QByteArray bty;
//    bty.append(fileContent + "zhangjun");
//    QByteArray md5Code = QCryptographicHash::hash(bty, QCryptographicHash::Md5);
//    QString md5 = md5Code.toHex();
//    DBG << "MD5" << md5;
//    if(md5 != md5InFile)
//    {
//        qInfo() << tr(u8"项目解析器：md5校验：在试图打开项目时遇到错误，项目文件已被锁坏");
//        QMessageBox::warning(0, qApp->applicationName(), tr(u8"在试图打开项目时遇到错误，项目文件“%0”已被锁坏！").arg(m_projectParameter->projectName));
//        return false;
//    }
//    return true;
//}

/**
 * CProjectResolver::save
 * \brief   保存当前文件
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool CProjectResolver::save()
{
    qInfo() << tr(u8"项目解析器：保存文件“%0”").arg(m_file->fileName());
    if(m_file->isOpen())
    {
        m_file->close();
    }
    if(!m_file->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }

    QTextStream txt(m_file);
    m_document->save(txt, 4);
    m_file->close();
    if(!m_file->open(QFile::ReadOnly | QFile::Text))
    {
        return false;
    }
    m_isModified = false;
    emit projectSaved(m_fileName);
    //    m_md5Code = makeMd5();
    return true;
}

bool CProjectResolver::renameProject(const QString &oldName, const QString &newName)
{
    if(m_file->isOpen())
    {
        m_file->close();
    }
    m_isStart = false;

    if(!QFile::rename(oldName, newName))
    {
        if(m_file->open(QFile::ReadOnly | QFile::Text))
        {
            m_isStart = true;
        }
        return false;
    }

    m_projectParameter->projectName = newName;
    foreach (SXmlParameter xmlParameter, *m_projectParameter->xmlParameterList)
    {
        xmlParameter.projectName = newName;
    }

    m_file->setFileName(newName);
    if(!m_file->open(QFile::ReadOnly | QFile::Text))
    {
        return false;
    }
    m_isStart = true;
    return true;
}


//bool CProjectResolver::getIsModified()
//{
//    return !(m_md5Code == makeMd5());
//}

//QByteArray CProjectResolver::makeMd5()
//{
//    QString content;
//    foreach (SXmlParameter xmlParameter, *m_projectParameter->xmlParameterList)
//    {
//        content.append(xmlParameter.xmlPath);
//        content.append(QString::number(xmlParameter.sort));
//        content.append(xmlParameter.inFifoPath0);
//        content.append(xmlParameter.inFifoPath1);
//        content.append(xmlParameter.inFifoPath2);
//        content.append(xmlParameter.inFifoPath3);
//        content.append(xmlParameter.inMemPath0);
//        content.append(xmlParameter.inMemPath1);
//        content.append(xmlParameter.inMemPath2);
//        content.append(xmlParameter.inMemPath3);
//        content.append(xmlParameter.inMemPath4);
//        content.append(QString::number(xmlParameter.onlyRsm));
//        content.append(QString::number(xmlParameter.rMode0));
//        content.append(QString::number(xmlParameter.rMode1));
//        content.append(QString::number(xmlParameter.gap));
//        content.append(xmlParameter.source);
//        content.append(xmlParameter.dest);
//        content.append(QString::number(xmlParameter.burstNum));
//        content.append(QString::number(xmlParameter.loopNum));
//        content.append(QString::number(xmlParameter.repeatNum));
//    }
//    QByteArray bty;
//    bty.append(content);
//    return QCryptographicHash::hash(bty, QCryptographicHash::Md5);
//}



/**
 * GuiViewer::okToContinue
 * \brief
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool CProjectResolver::okToContinue()
{
    //    m_isModified = !(m_md5Code == makeMd5());
#ifdef GUI
    if (m_isModified) {
        int &&ret = QMessageBox::warning(CAppEnv::m_mainWgt.data(), qApp->applicationName(),
                                       tr(u8"是否保存对项目 “%0” 的修改？")
                                       .arg(CAppEnv::stpToNm(m_file->fileName())),
                                       QMessageBox::Save | QMessageBox::Discard |
                                       QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
        {
            paraChanged();
            return save();
        }
        else if (ret == QMessageBox::Cancel)
        {
            return false;
        }
        else if (ret == QMessageBox::Discard)
        {
            m_projectParameter->xmlParameterList->clear();
            m_projectParameter->resourceMap.clear();
            refreshProjectParameter();
            emit refreshed();
        }
    }
#endif
    return true;
}

/*!
 * sortASC
 * \brief   顺序排序
 * \param   paraInfo1
 * \param   paraInfo2
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool sortBySetting(SXmlParameter xmlParameter1, SXmlParameter xmlParameter2)
{
    if(xmlParameter1.sort == -1)
        return false;
    if(xmlParameter2.sort == -1)
        return true;
    if(xmlParameter1.sort < xmlParameter2.sort)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool sortByName(SXmlParameter xmlParameter1, SXmlParameter xmlParameter2)
{
    if(QFileInfo(xmlParameter1.xmlPath).fileName().toLower() < QFileInfo(xmlParameter2.xmlPath).fileName().toLower())
        return true;
    else
        return false;
}

//初始化pro信息
/**
 * CProjectResolver::initProInfo
 * \brief   初始化项目解析器
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool CProjectResolver::initProInfo()
{
    m_document->setContent(m_file);
    //    QDomNode document->documentElement();
    m_root = m_document->documentElement();

    if(m_root.isNull())
    {
        QDomElement element = m_document->createElement("ALG");
        element.setAttribute("value", m_projectParameter->projectName);
        element.setAttribute("name", QFileInfo(m_projectParameter->projectName).baseName());
        element.setAttribute("cipherType", m_projectParameter->cipherType);
        element.setAttribute("slnname", m_projectParameter->solutionName);
        element.setAttribute("lfsrMode", m_projectParameter->lfsrMode);
        element.setAttribute("lfsrWidthDeep", m_projectParameter->lfsrWidthDeep);
        m_document->appendChild(element);
        m_root = element;
    }
    else if(m_root.tagName() != "ALG")
    {
        return false;
    }
    else
    {
        m_root.setAttribute("value", m_projectParameter->projectName);
        m_root.setAttribute("name", QFileInfo(m_projectParameter->projectName).baseName());
    }

    QDomNode node = m_document->childNodes().at(0);
    if(node.toProcessingInstruction().isNull())
    {
        QDomProcessingInstruction instruction;
        instruction = m_document->createProcessingInstruction("xml", "version=\'1.0\' encoding=\'UTF-8\'");
        m_document->insertBefore(instruction, m_root);
    }

    refreshProjectParameter();
    save();
    return true;
}

void CProjectResolver::refreshProjectParameter()
{
    m_projectParameter->cipherType = (ECipherType)m_root.attribute("cipherType", QString::number(m_projectParameter->cipherType)).toInt();
    m_projectParameter->solutionName = m_root.attribute("slnname");
    m_projectParameter->lfsrMode = m_root.attribute("lfsrMode", "0").toInt();
    m_projectParameter->lfsrWidthDeep = m_root.attribute("lfsrWidthDeep", "0").toInt();

    QDomNodeList list = m_root.childNodes();
    QStringList xmlNames, srcNames;
    QList<QDomElement> domList;
    for(int i = 0; i < list.count(); ++i)
    {
        QDomElement ele = list.at(i).toElement();
        if(ele.tagName() == "XML")
        {
            QString xmlPath = ele.attribute("value");

            //xml文件是否存在
            if(!QFile(CAppEnv::relToAbs(m_projectParameter->projectName, xmlPath)).exists())
            {
//                m_root.removeChild(ele);
                domList.append(ele);
                continue;
            }
            if(xmlNames.contains(QFileInfo(xmlPath).fileName().toLower()))
            {
//                m_root.removeChild(ele);
                domList.append(ele);
                continue;
            }
            xmlNames.append(QFileInfo(xmlPath).fileName().toLower());

            SXmlParameter /***/xmlParameter/* = new SXmlParameter*/;
            if(ele.attribute("select", 0).toInt())
            {
                xmlParameter.selected = Qt::Checked;
            }
            else
            {
                xmlParameter.selected = Qt::Unchecked;
            }
            xmlParameter.projectName = m_projectParameter->projectName;
            xmlParameter.progress = 0;
            xmlParameter.status = tr(u8"准备就绪");
            xmlParameter.xmlPath = CAppEnv::relToAbs(m_projectParameter->projectName, xmlPath);
            xmlParameter.sort = ele.attribute("sort").toInt();
            xmlParameter.inFifoPath0 = CAppEnv::relToAbs(m_projectParameter->projectName, getFifoInputElement(ele, 0).attribute("value"));
            xmlParameter.inFifoPath1 = CAppEnv::relToAbs(m_projectParameter->projectName, getFifoInputElement(ele, 1).attribute("value"));
            xmlParameter.inFifoPath2 = CAppEnv::relToAbs(m_projectParameter->projectName, getFifoInputElement(ele, 2).attribute("value"));
            xmlParameter.inFifoPath3 = CAppEnv::relToAbs(m_projectParameter->projectName, getFifoInputElement(ele, 3).attribute("value"));
            xmlParameter.inMemPath0 = CAppEnv::relToAbs(m_projectParameter->projectName, getMemInputElement(ele, 0).attribute("value"));
            xmlParameter.inMemPath1 = CAppEnv::relToAbs(m_projectParameter->projectName, getMemInputElement(ele, 1).attribute("value"));
            xmlParameter.inMemPath2 = CAppEnv::relToAbs(m_projectParameter->projectName, getMemInputElement(ele, 2).attribute("value"));
            xmlParameter.inMemPath3 = CAppEnv::relToAbs(m_projectParameter->projectName, getMemInputElement(ele, 3).attribute("value"));
            xmlParameter.inMemPath4 = CAppEnv::relToAbs(m_projectParameter->projectName, getMemInputElement(ele, 4).attribute("value"));
//            QString outFifoPath = CAppEnv::getFifoOutDirPath() + "/" + QFileInfo(paraInfo->xmlFNP).baseName();
//            xmlParameter.outFifoPath0 = outFifoPath + "0.log"; //不用使用者指定
//            xmlParameter.outFifoPath1 = outFifoPath + "1.log"; //不用使用者指定
//            xmlParameter.outFifoPath2 = outFifoPath + "2.log"; //不用使用者指定
//            xmlParameter.outFifoPath3 = outFifoPath + "3.log"; //不用使用者指定
//            QString outMemPath = CAppEnv::getMergeOutDirPath() + "/" + QFileInfo(paraInfo->xmlFNP).baseName();
//            xmlParameter.outMemPath0 = outMemPath + "0.txt"; //不用使用者指定
//            xmlParameter.outMemPath1 = outMemPath + "1.txt"; //不用使用者指定
//            xmlParameter.outMemPath2 = outMemPath + "2.txt"; //不用使用者指定
//            xmlParameter.outMemPath3 = outMemPath + "3.txt"; //不用使用者指定
//            xmlParameter.outMemPath4 = outMemPath + "4.txt"; //不用使用者指定
            xmlParameter.cycleLogPath = CAppEnv::getCycleOutDirPath() + "/" + QFileInfo(xmlPath).baseName() + ".log"; //不用使用者指定
//            xmlParameter.binPath = CAppEnv::getBinOutDirPath() + "/["
//                    + QFileInfo(m_projectParameter->projectName).completeBaseName() + "]"
//                    + QFileInfo(xmlPath).completeBaseName() + ".bin.txt"; //运行时设置
//            xmlParameter.indexFile = CAppEnv::getMergeOutDirPath() + "/" + QFileInfo(xmlPath).baseName() + ".merge.txt"; //不用使用者指定

            xmlParameter.onlyRsm = getCodeChildElement(ele, "ONLYSIM").attribute("value").toInt();
            xmlParameter.rMode0 = getCodeChildElement(ele, "RMODE0").attribute("value").toInt();
            xmlParameter.rMode1 = getCodeChildElement(ele, "RMODE1").attribute("value").toInt();
            xmlParameter.gap = getCodeChildElement(ele, "GAP").attribute("value").toInt();
            xmlParameter.source = getCodeSourceElement(ele).attribute("value");
            xmlParameter.dest = getCodeDestElement(ele).attribute("value");
            xmlParameter.burstNum = getCodeChildElement(ele, "BURSTNUM").attribute("value").toInt();
            xmlParameter.loopNum = getCodeChildElement(ele, "LOOPNUM").attribute("value").toInt();
            xmlParameter.repeatNum = getCodeChildElement(ele, "REPEATNUM").attribute("value").toInt();

            m_projectParameter->xmlParameterList->append(xmlParameter);
        }
        else if(ele.tagName() == "SRC")
        {
            QString srcPath = ele.attribute("value");

            //src文件是否存在
            if(!QFile(CAppEnv::relToAbs(m_projectParameter->projectName, srcPath)).exists())
            {
                //                m_root.removeChild(ele);
                domList.append(ele);
                continue;
            }
            if(srcNames.contains(QFileInfo(srcPath).fileName().toLower()))
            {
                //                m_root.removeChild(ele);
                domList.append(ele);
                continue;
            }
            srcNames.append(QFileInfo(srcPath).fileName().toLower());
            m_projectParameter->resourceMap.insert(":/" + CAppEnv::stpToNm(srcPath), CAppEnv::relToAbs(m_projectParameter->projectName, srcPath));
        }

    }

    for(auto i = 0; i < domList.count(); i++)
    {
        m_root.removeChild(domList.at(i));
    }

    sortXml();
//    m_md5Code = makeMd5();
    m_isModified = false;
}

void CProjectResolver::sortXml(int type)
{
    if(type)
        qStableSort(m_projectParameter->xmlParameterList->begin(), m_projectParameter->xmlParameterList->end(), sortBySetting);
    else
        qStableSort(m_projectParameter->xmlParameterList->begin(), m_projectParameter->xmlParameterList->end(), sortByName);
}

/**
 * CProjectResolver::appendXmlElement
 * \brief   项目内增加XML文件
 * \param   fileName
 * \param   isExists
 * \author  zhangjun
 * \date    2016-10-12
 */
void CProjectResolver::appendXmlElement(const QString &fileName/*, bool isExists*/)
{
    QDomNodeList list = m_root.childNodes();
    for(int i = 0; i < list.count(); ++i)
    {
        QDomElement xml = list.at(i).toElement();
        if(xml.tagName() == "XML" && CAppEnv::relToAbs(m_projectParameter->projectName, xml.attribute("value")) == fileName)
        {
            m_root.removeChild(xml);
        }
    }

    QDomElement xml = m_document->createElement("XML");
    xml.setAttribute("name", "");
    xml.setAttribute("value", CAppEnv::absToRel(m_projectParameter->projectName, fileName));
    xml.setAttribute("sort", "-1");
    m_root.appendChild(xml);

    SXmlParameter /***/xmlParameter/* = new SXmlParameter*/;
    xmlParameter.xmlPath = fileName;
    xmlParameter.sort = -1;
    xmlParameter.cycleLogPath = CAppEnv::getCycleOutDirPath() + "/" + QFileInfo(fileName).baseName() + ".log"; //不用使用者指定
//    xmlParameter.binPath = CAppEnv::getBinOutDirPath() + "/["
//            + QFileInfo(m_projectParameter->projectName).completeBaseName() + "]"
//            + QFileInfo(fileName).completeBaseName() + ".bin.txt"; //运行时设置
//    xmlParameter.indexFile = CAppEnv::getMergeOutDirPath() + "/" + QFileInfo(fileName).baseName() + ".merge.txt"; //不用使用者指定
    xmlParameter.selected = Qt::Checked;
    xml.setAttribute("select", QString::number(1));
    xmlParameter.projectName = m_projectParameter->projectName;
    xml.setAttribute("project", xmlParameter.projectName);
    xmlParameter.progress = 0;
    xmlParameter.status = tr(u8"准备就绪");


//    if(isExists)
//    {
//        XmlResolver xmlResolver(fileName);

//        QString value;
//        QDomElement element;/* = getCycleElement(xml);
//        element.setAttribute("value", xmlResolver.findCycleElement().attribute("value"));

//        element = getIntervalElement(xml);
//        element.setAttribute("value", xmlResolver.findIntervalElement().attribute("value"));*/

//        element = getFifoInputElement(xml, 0);
//        value = xmlResolver.findFifoInput(0).attribute("value");
//        xmlParameter.inFifoPath0 = CAppEnv::relToAbs(m_projectParameter->projectName, value);
//        element.setAttribute("value",  CAppEnv::absToRel(m_projectParameter->projectName, value));

//        element = getFifoInputElement(xml, 1);
//        value = xmlResolver.findFifoInput(1).attribute("value");
//        xmlParameter.inFifoPath1 = CAppEnv::relToAbs(m_projectParameter->projectName, value);
//        element.setAttribute("value", CAppEnv::absToRel(m_projectParameter->projectName, value));

//        element = getFifoInputElement(xml, 2);
//        value = xmlResolver.findFifoInput(2).attribute("value");
//        xmlParameter.inFifoPath2 = CAppEnv::relToAbs(m_projectParameter->projectName, value);
//        element.setAttribute("value", CAppEnv::absToRel(m_projectParameter->projectName, value));

//        element = getFifoInputElement(xml, 3);
//        value = xmlResolver.findFifoInput(3).attribute("value");
//        xmlParameter.inFifoPath3 = CAppEnv::relToAbs(m_projectParameter->projectName, value);
//        element.setAttribute("value", CAppEnv::absToRel(m_projectParameter->projectName, value));

//        element = getMemInputElement(xml, 0);
//        value = xmlResolver.findMemInput(0).attribute("value");
//        xmlParameter.inMemPath0 = CAppEnv::relToAbs(m_projectParameter->projectName, value);
//        element.setAttribute("value", CAppEnv::absToRel(m_projectParameter->projectName, value));

//        element = getMemInputElement(xml, 1);
//        value = xmlResolver.findMemInput(1).attribute("value");
//        xmlParameter.inMemPath0 = CAppEnv::relToAbs(m_projectParameter->projectName, value);
//        element.setAttribute("value", CAppEnv::absToRel(m_projectParameter->projectName, value));

//        element = getMemInputElement(xml, 2);
//        value = xmlResolver.findMemInput(2).attribute("value");
//        xmlParameter.inMemPath0 = CAppEnv::relToAbs(m_projectParameter->projectName, value);
//        element.setAttribute("value", CAppEnv::absToRel(m_projectParameter->projectName, value));

//        element = getMemInputElement(xml, 3);
//        value = xmlResolver.findMemInput(3).attribute("value");
//        xmlParameter.inMemPath0 = CAppEnv::relToAbs(m_projectParameter->projectName, value);
//        element.setAttribute("value", CAppEnv::absToRel(m_projectParameter->projectName, value));

//        element = getMemInputElement(xml, 4);
//        value = xmlResolver.findMemInput(4).attribute("value");
//        xmlParameter.inMemPath0 = CAppEnv::relToAbs(m_projectParameter->projectName, value);
//        element.setAttribute("value", CAppEnv::absToRel(m_projectParameter->projectName, value));

//        element = getCodeChildElement(xml, "ONLYSIM");
//        value = xmlResolver.getCodeChildElement("ONLYSIM").attribute("value");
//        xmlParameter.onlyRsm = value.toInt();
//        element.setAttribute("value", value);

//        element = getCodeChildElement(xml, "RMODE0");
//        value = xmlResolver.getCodeChildElement("RMODE0").attribute("value");
//        xmlParameter.rMode0 = value.toInt();
//        element.setAttribute("value", value);

//        element = getCodeChildElement(xml, "RMODE1");
//        value = xmlResolver.getCodeChildElement("RMODE1").attribute("value");
//        xmlParameter.rMode1 = value.toInt();
//        element.setAttribute("value", value);

//        element = getCodeChildElement(xml, "GAP");
//        value = xmlResolver.getCodeChildElement("GAP").attribute("value");
//        xmlParameter.gap = value.toInt();
//        element.setAttribute("value", value);

//        element = getCodeSourceElement(xml);
//        value = xmlResolver.getCodeSourceElement().attribute("value");
//        xmlParameter.source = value;
//        element.setAttribute("value", value);

//        element = getCodeDestElement(xml);
//        value =  xmlResolver.getCodeDestElement().attribute("value");
//        xmlParameter.dest = value;
//        element.setAttribute("value", value);

//        element = getCodeChildElement(xml, "BURSTNUM");
//        value = xmlResolver.getCodeChildElement("BURSTNUM").attribute("value");
//        xmlParameter.burstNum = value.toInt();
//        element.setAttribute("value", value);

//        element = getCodeChildElement(xml, "LOOPNUM");
//        value = xmlResolver.getCodeChildElement("LOOPNUM").attribute("value");
//        xmlParameter.loopNum = value.toInt();
//        element.setAttribute("value", value);

//        element = getCodeChildElement(xml, "REPEATNUM");
//        value = xmlResolver.getCodeChildElement("REPEATNUM").attribute("value");
//        xmlParameter.repeatNum = value.toInt();
//        element.setAttribute("value", value);
//    }
//    else
//    {
        getFifoInputElement(xml, 0);
        getFifoInputElement(xml, 1);
        getFifoInputElement(xml, 2);
        getFifoInputElement(xml, 3);
        getMemInputElement(xml, 0);
        getMemInputElement(xml, 1);
        getMemInputElement(xml, 2);
        getMemInputElement(xml, 3);
        getMemInputElement(xml, 4);
        getCodeElement(xml);
        getCodeChildElement(xml, "ONLYSIM");
        getCodeChildElement(xml, "RMODE0");
        getCodeChildElement(xml, "RMODE1");
        getCodeChildElement(xml, "GAP");
        getCodeSourceElement(xml);
        getCodeDestElement(xml);
        getCodeChildElement(xml, "BURSTNUM");
        getCodeChildElement(xml, "LOOPNUM");
        getCodeChildElement(xml, "REPEATNUM");

        xmlParameter.onlyRsm = 0;
        xmlParameter.rMode0 = 0;
        xmlParameter.rMode1 = 0;
        xmlParameter.gap = 0;
        xmlParameter.source = "MemoryPort0";
        xmlParameter.dest = "MemoryPort0";
        xmlParameter.burstNum = 0;
        xmlParameter.loopNum = 0;
        xmlParameter.repeatNum = 0;
//    }
    m_projectParameter->xmlParameterList->append(xmlParameter);
    save();
}

/**
 * CProjectResolver::removeXmlElement
 * \brief   项目中移除XML文件
 * \param   xmlName
 * \author  zhangjun
 * \date    2016-10-12
 */
void CProjectResolver::removeXmlElement(const QString &xmlName)
{
    QDomNodeList list = m_root.childNodes();
    for(int i = 0; i < list.count(); ++i)
    {
        QDomElement xml = list.at(i).toElement();
        if(xml.tagName() == "XML" && CAppEnv::relToAbs(m_projectParameter->projectName, xml.attribute("value")) == xmlName)
        {
            m_root.removeChild(xml);
        }
    }
    for (int i = 0; i < m_projectParameter->xmlParameterList->count(); ++i)
    {
        SXmlParameter xmlParameter = m_projectParameter->xmlParameterList->at(i);
        if(xmlParameter.xmlPath == xmlName)
        {
//            delete xmlParameter;
            m_projectParameter->xmlParameterList->removeAt(i);
            break;
        }
    }
    save();
}

//xml重命名
/**
 * CProjectResolver::changeXmlElement
 * \brief   重命名项目中的XML文件
 * \param   oldXmlName
 * \param   newXmlName
 * \author  zhangjun
 * \date    2016-10-12
 */
void CProjectResolver::changeXmlElement(const QString &oldXmlName, const QString &newXmlName)
{
    QDomNodeList list = m_root.childNodes();

    for(int i = 0; i < list.count(); ++i)
    {
        QDomElement xml = list.at(i).toElement();
        if(xml.tagName() == "XML" && CAppEnv::relToAbs(m_projectParameter->projectName, xml.attribute("value")) == newXmlName)
        {
            m_root.removeChild(xml);
        }
    }
    for(int i = 0; i < list.count(); ++i)
    {
        QDomElement xml = list.at(i).toElement();
        if(xml.tagName() == "XML" && CAppEnv::relToAbs(m_projectParameter->projectName, xml.attribute("value")) == oldXmlName)
        {
            xml.setAttribute("value", CAppEnv::absToRel(m_projectParameter->projectName, newXmlName));
        }
    }
    for (int i = 0; i < m_projectParameter->xmlParameterList->count(); ++i)
    {
        SXmlParameter xmlParameter = m_projectParameter->xmlParameterList->at(i);
        if(xmlParameter.xmlPath == oldXmlName)
        {
            xmlParameter.xmlPath = newXmlName;
            m_projectParameter->xmlParameterList->replace(i, xmlParameter);

        }
    }
    save();
}

/**
 * CProjectResolver::appendSrcElement
 * \brief   项目内增加资源文件
 * \param   fileName
 * \author  zhangjun
 * \date    2016-10-12
 */
void CProjectResolver::appendSrcElement(const QString &fileName)
{
    QDomNodeList list = m_root.childNodes();
    for(int i = 0; i < list.count(); ++i)
    {
        QDomElement src = list.at(i).toElement();
        if(src.tagName() == "SRC" && CAppEnv::relToAbs(m_projectParameter->projectName, src.attribute("value")) == fileName)
        {
            m_root.removeChild(src);
        }
    }

    QDomElement src = m_document->createElement("SRC");
    src.setAttribute("value", CAppEnv::absToRel(m_projectParameter->projectName, fileName));
    m_root.appendChild(src);

    m_projectParameter->resourceMap.insert(":/" + CAppEnv::stpToNm(fileName), fileName);
    save();
}

/**
 * CProjectResolver::removeSrcElement
 * \brief   项目内移除资源文件
 * \param   srcName
 * \author  zhangjun
 * \date    2016-10-12
 */
void CProjectResolver::removeSrcElement(const QString &srcName)
{
    QDomNodeList list = m_root.childNodes();
    for(int i = 0; i < list.count(); ++i)
    {
        QDomElement src = list.at(i).toElement();
        if(src.tagName() == "SRC" && CAppEnv::relToAbs(m_projectParameter->projectName, src.attribute("value")) == srcName)
        {
            m_root.removeChild(src);
        }
    }

    m_projectParameter->resourceMap.remove(":/" + CAppEnv::stpToNm(srcName));
    save();
}

/**
 * CProjectResolver::changeSrcElement
 * \brief   项目内重命名资源文件
 * \param   oldSrcName
 * \param   newSrcName
 * \author  zhangjun
 * \date    2016-10-12
 */
void CProjectResolver::changeSrcElement(const QString &oldSrcName, const QString &newSrcName)
{
    QDomNodeList list = m_root.childNodes();

    for(int i = 0; i < list.count(); ++i)
    {
        QDomElement src = list.at(i).toElement();
        if(src.tagName() == "SRC" && CAppEnv::relToAbs(m_projectParameter->projectName, src.attribute("value")) == newSrcName)
        {
            m_root.removeChild(src);
        }
    }
    for(int i = 0; i < list.count(); ++i)
    {
        QDomElement src = list.at(i).toElement();
        if(src.tagName() == "SRC" && CAppEnv::relToAbs(m_projectParameter->projectName, src.attribute("value")) == oldSrcName)
        {
            src.setAttribute("value", CAppEnv::absToRel(m_projectParameter->projectName, newSrcName));
        }
    }

    m_projectParameter->resourceMap.remove(":/" + CAppEnv::stpToNm(oldSrcName));
    m_projectParameter->resourceMap.insert(":/" + CAppEnv::stpToNm(newSrcName), newSrcName);
    save();
}

std::vector<int> CProjectResolver::getOutfifoRunResult(const int coreIndex) const
{
    if(coreIndex > 15 || coreIndex < 0)
        return std::vector<int>();
    return m_outfifoRunResult[coreIndex];
}

std::vector<int> CProjectResolver::getMemoryRunResult(const int coreIndex) const
{
    if(coreIndex > 15 || coreIndex < 0)
        return std::vector<int>();
    return m_memoryRunResult[coreIndex];
}

void CProjectResolver::setDirty()
{
    m_isModified = true;
}

QStringList CProjectResolver::getOutfifoList()
{
    return m_outfifoList;
}

QStringList CProjectResolver::getMemoryList()
{
    return m_memoryList;
}

void CProjectResolver::setOutfifoRunResult(const int coreIndex, const std::shared_ptr<std::vector<int>> &v)
{
    if(v == nullptr || coreIndex > 15 || coreIndex < 0)
        return;

    m_outfifoRunResult[coreIndex].clear();
    m_outfifoRunResult[coreIndex] = *(v.get());
}

void CProjectResolver::setMemoryRunResult(const int coreIndex, const std::shared_ptr<std::vector<int>> &v)
{
    if(v == nullptr || coreIndex > 15 || coreIndex < 0)
        return;

    m_memoryRunResult[coreIndex].clear();
    m_memoryRunResult[coreIndex] = *(v.get());
}

void CProjectResolver::setOutfifoList(const QStringList &strList)
{
    m_outfifoList.clear();
    m_outfifoList = strList;
}

void CProjectResolver::setMemoryList(const QStringList &strList)
{
    m_memoryList.clear();
    m_memoryList = strList;
}


//xml信息改变
/**
 * CProjectResolver::paraChanged
 * \brief   项目信息发生改变
 * \author  zhangjun
 * \date    2016-10-12
 */
void CProjectResolver::paraChanged()
{
    qInfo() << tr(u8"项目解析器：项目信息发生改变，写入项目信息");
//    QMapIterator<QString, SParaInfo *> itor(m_proInfo->xmlParaMap);
    m_root.setAttribute("cipherType", m_projectParameter->cipherType);
    m_root.setAttribute("lfsrMode", m_projectParameter->lfsrMode);
    m_root.setAttribute("lfsrWidthDeep", m_projectParameter->lfsrWidthDeep);
    m_root.setAttribute("slnname", m_projectParameter->solutionName);
    int sort = 0;
    foreach (SXmlParameter xmlParameter, *m_projectParameter->xmlParameterList)
    {
        xmlParameter.sort = sort++;
        QDomElement xml = getXmlElement(xmlParameter.xmlPath);
        xml.setAttribute("sort", QString::number(xmlParameter.sort));
        xml.setAttribute("project", xmlParameter.projectName);
        if(xmlParameter.selected == Qt::Checked)
            xml.setAttribute("select", QString::number(1));
        else
            xml.setAttribute("select", QString::number(0));
        if(xml.isNull())
            continue;

        QDomElement element;/* = getCycleElement(xml);*/
        element = getFifoInputElement(xml, 0);
        element.setAttribute("value", CAppEnv::absToRel(m_projectParameter->projectName, xmlParameter.inFifoPath0));
        element = getFifoInputElement(xml, 1);
        element.setAttribute("value", CAppEnv::absToRel(m_projectParameter->projectName, xmlParameter.inFifoPath1));
        element = getFifoInputElement(xml, 2);
        element.setAttribute("value", CAppEnv::absToRel(m_projectParameter->projectName, xmlParameter.inFifoPath2));
        element = getFifoInputElement(xml, 3);
        element.setAttribute("value", CAppEnv::absToRel(m_projectParameter->projectName, xmlParameter.inFifoPath3));

        element = getMemInputElement(xml, 0);
        element.setAttribute("value", CAppEnv::absToRel(m_projectParameter->projectName, xmlParameter.inMemPath0));
        element = getMemInputElement(xml, 1);
        element.setAttribute("value", CAppEnv::absToRel(m_projectParameter->projectName, xmlParameter.inMemPath1));
        element = getMemInputElement(xml, 2);
        element.setAttribute("value", CAppEnv::absToRel(m_projectParameter->projectName, xmlParameter.inMemPath2));
        element = getMemInputElement(xml, 3);
        element.setAttribute("value", CAppEnv::absToRel(m_projectParameter->projectName, xmlParameter.inMemPath3));
        element = getMemInputElement(xml, 4);
        element.setAttribute("value", CAppEnv::absToRel(m_projectParameter->projectName, xmlParameter.inMemPath4));

        QDomElement codeChild;
        codeChild = getCodeChildElement(xml, "ONLYSIM");
        codeChild.setAttribute("value", QString::number(xmlParameter.onlyRsm));
        codeChild = getCodeChildElement(xml, "RMODE0");
        codeChild.setAttribute("value", QString::number(xmlParameter.rMode0));
        codeChild = getCodeChildElement(xml, "RMODE1");
        codeChild.setAttribute("value", QString::number(xmlParameter.rMode1));
        codeChild = getCodeChildElement(xml, "GAP");
        codeChild.setAttribute("value", QString::number(xmlParameter.gap));
        codeChild = getCodeSourceElement(xml);
        codeChild.setAttribute("value", xmlParameter.source);
        codeChild = getCodeDestElement(xml);
        codeChild.setAttribute("value", xmlParameter.dest);
        codeChild = getCodeChildElement(xml, "BURSTNUM");
        codeChild.setAttribute("value", QString::number(xmlParameter.burstNum));
        codeChild = getCodeChildElement(xml, "LOOPNUM");
        codeChild.setAttribute("value", QString::number(xmlParameter.loopNum));
        codeChild = getCodeChildElement(xml, "REPEATNUM");
        codeChild.setAttribute("value", QString::number(xmlParameter.repeatNum));
    }
//    foreach (SCMDWordInfo *cmdWordInfo, m_proInfo->cmdInfoList)
//    {
//        QDomElement xml = getXmlElement(cmdWordInfo->xmlFile);
//        if(xml.isNull())
//            continue;

//        QDomElement codeChild = getCodeChildElement(xml, "ONLYSIM");
//        codeChild.setAttribute("value", QString::number(cmdWordInfo->onlyRsm));
//        codeChild = getCodeChildElement(xml, "RMODE0");
//        codeChild.setAttribute("value", QString::number(cmdWordInfo->rMode0));
//        codeChild = getCodeChildElement(xml, "RMODE1");
//        codeChild.setAttribute("value", QString::number(cmdWordInfo->rMode1));
//        codeChild = getCodeChildElement(xml, "GAP");
//        codeChild.setAttribute("value", QString::number(cmdWordInfo->gap));
//        codeChild = getCodeSourceElement(xml);
//        codeChild.setAttribute("value", cmdWordInfo->source);
//        codeChild = getCodeDestElement(xml);
//        codeChild.setAttribute("value", cmdWordInfo->dest);
//        codeChild = getCodeChildElement(xml, "BURSTNUM");
//        codeChild.setAttribute("value", QString::number(cmdWordInfo->burstNum));
//        codeChild = getCodeChildElement(xml, "LOOPNUM");
//        codeChild.setAttribute("value", QString::number(cmdWordInfo->loopNum));
//        codeChild = getCodeChildElement(xml, "REPEATNUM");
//        codeChild.setAttribute("value", QString::number(cmdWordInfo->repeatNum));
//    }
}

/**
 * CProjectResolver::getSrcElement
 * \brief
 * \param   value
 * \return  QDomElement
 * \author  zhangjun
 * \date    2016-10-12
 */
QDomElement CProjectResolver::getSrcElement(const QString &value)
{
    QDomNodeList list = m_root.childNodes();
    for(int i = 0; i<list.count(); ++i)
    {
        QDomElement src = list.at(i).toElement();
        if(src.tagName() == "Src" && CAppEnv::relToAbs(m_projectParameter->projectName, src.attribute("value")) == value)
            return src;
    }
    return QDomElement();
}

/**
 * CProjectResolver::getXmlElement
 * \brief
 * \param   value
 * \return  QDomElement
 * \author  zhangjun
 * \date    2016-10-12
 */
QDomElement CProjectResolver::getXmlElement(const QString &value)
{
    QDomNodeList list = m_root.childNodes();
    for(int i = 0; i<list.count(); ++i)
    {
        QDomElement xml = list.at(i).toElement();
        if(xml.tagName() == "XML" && CAppEnv::relToAbs(m_projectParameter->projectName, xml.attribute("value")) == value)
            return xml;
    }
    return QDomElement();
}

/**
 * CProjectResolver::getCycleElement
 * \brief
 * \param   xml
 * \return  QDomElement
 * \author  zhangjun
 * \date    2016-10-12
 */
QDomElement CProjectResolver::getCycleElement(QDomElement xml)
{
    QDomNodeList list = xml.childNodes();
    for(int i = 0; i<list.count(); ++i)
    {
        QDomElement cycle = list.at(i).toElement();
        if(cycle.tagName() == "CYCLE")
            return cycle;
    }

    //没找到 新建
    QDomElement element = m_document->createElement("CYCLE");
    element.setAttribute("value", QString::number(0));
    xml.appendChild(element);
    return element;
}


/**
 * CProjectResolver::getIntervalElement
 * \brief
 * \param   xml
 * \return  QDomElement
 * \author  zhangjun
 * \date    2016-10-12
 */
QDomElement CProjectResolver::getIntervalElement(QDomElement xml)
{
    QDomNodeList list = xml.childNodes();
    for(int i = 0; i < list.count(); ++i)
    {
        QDomElement interval = list.at(i).toElement();
        if(interval.tagName() == "INTERVAL")
            return interval;
    }

    //没找到 新建
    QDomElement element = m_document->createElement("INTERVAL");
    element.setAttribute("value", QString::number(0));
    xml.appendChild(element);
    return element;

}

/**
 * CProjectResolver::getFifoElement
 * \brief
 * \param   xml
 * \return  QDomElement
 * \author  zhangjun
 * \date    2016-10-12
 */
QDomElement CProjectResolver::getFifoElement(QDomElement xml)
{
    QDomNodeList list = xml.childNodes();
    for(int i = 0; i < list.count(); ++i)
    {
        QDomElement fifo = list.at(i).toElement();
        if(fifo.tagName() == "FIFO")
            return fifo;
    }

    //没找到 新建
    QDomElement element = m_document->createElement("FIFO");
    xml.appendChild(element);
    return element;
}

/**
 * CProjectResolver::getFifoInputElement
 * \brief
 * \param   xml
 * \param   id
 * \return  QDomElement
 * \author  zhangjun
 * \date    2016-10-12
 */
QDomElement CProjectResolver::getFifoInputElement(QDomElement xml, int id)
{
    QDomElement fifo = getFifoElement(xml);
    QDomNodeList list = fifo.childNodes();
    for(int i = 0; i < list.count(); ++i)
    {
        QDomElement fifoInput = list.at(i).toElement();
        if(fifoInput.tagName() == "INPUT" && fifoInput.attribute("id").toInt() == id)
            return fifoInput;
    }

    //没找到 新建
    QDomElement element = m_document->createElement("INPUT");
    element.setAttribute("id", QString::number(id));
    element.setAttribute("value", "");
    fifo.appendChild(element);
    return element;
}

/**
 * CProjectResolver::getMemElement
 * \brief
 * \param   xml
 * \return  QDomElement
 * \author  zhangjun
 * \date    2016-10-12
 */
QDomElement CProjectResolver::getMemElement(QDomElement xml)
{
    QDomNodeList list = xml.childNodes();
    for(int i = 0; i < list.count(); ++i)
    {
        QDomElement mem = list.at(i).toElement();
        if(mem.tagName() == "MEM")
            return mem;
    }

    //没找到 新建
    QDomElement element = m_document->createElement("MEM");
    xml.appendChild(element);
    return element;
}

/**
 * CProjectResolver::getMemInputElement
 * \brief
 * \param   xml
 * \param   id
 * \return  QDomElement
 * \author  zhangjun
 * \date    2016-10-12
 */
QDomElement CProjectResolver::getMemInputElement(QDomElement xml, int id)
{
    QDomElement mem = getMemElement(xml);
    QDomNodeList list = mem.childNodes();
    for(int i = 0; i < list.count(); ++i)
    {
        QDomElement memInput = list.at(i).toElement();
        if(memInput.tagName() == "INPUT" && memInput.attribute("id").toInt() == id)
            return memInput;
    }

    //没找到 新建
    QDomElement element = m_document->createElement("INPUT");
    element.setAttribute("id", QString::number(id));
    element.setAttribute("value", "");
    mem.appendChild(element);
    return element;
}

/**
 * CProjectResolver::getCodeElement
 * \brief
 * \param   xml
 * \return  QDomElement
 * \author  zhangjun
 * \date    2016-10-12
 */
QDomElement CProjectResolver::getCodeElement(QDomElement xml)
{
    QDomNodeList list = xml.childNodes();
    for(int i = 0; i<list.count(); ++i)
    {
        QDomElement code = list.at(i).toElement();
        if(code.tagName() == "CODE")
            return code;
    }

    //没找到 新建
    QDomElement element = m_document->createElement("CODE");
    xml.appendChild(element);
    return element;
}

/**
 * CProjectResolver::getCodeChildElement
 * \brief
 * \param   xml
 * \param   childName
 * \return  QDomElement
 * \author  zhangjun
 * \date    2016-10-12
 */
QDomElement CProjectResolver::getCodeChildElement(QDomElement xml, const QString &childName)
{
    QDomElement code = getCodeElement(xml);
    QDomNodeList list = code.childNodes();
    for(int i = 0; i < list.count(); ++i)
    {
        QDomElement ele = list.at(i).toElement();
        if(ele.tagName() == childName)
            return ele;
    }

    //没找到 新建
    QDomElement element = m_document->createElement(childName);
    element.setAttribute("value", "0");
    code.appendChild(element);
    return element;

}

/**
 * CProjectResolver::getCodeSourceElement
 * \brief
 * \param   xml
 * \return  QDomElement
 * \author  zhangjun
 * \date    2016-10-12
 */
QDomElement CProjectResolver::getCodeSourceElement(QDomElement xml)
{
    QDomElement code = getCodeElement(xml);
    QDomNodeList list = code.childNodes();
    for(int i = 0; i < list.count(); ++i)
    {
        QDomElement ele = list.at(i).toElement();
        if(ele.tagName() == "SOURCE")
            return ele;
    }

    //没找到 新建
    QDomElement element = m_document->createElement("SOURCE");
    element.setAttribute("value", "MemoryPort0");
    code.appendChild(element);
    return element;
}

/**
 * CProjectResolver::getCodeDestElement
 * \brief
 * \param   xml
 * \return  QDomElement
 * \author  zhangjun
 * \date    2016-10-12
 */
QDomElement CProjectResolver::getCodeDestElement(QDomElement xml)
{
    QDomElement code = getCodeElement(xml);
    QDomNodeList list = code.childNodes();
    for(int i = 0; i < list.count(); ++i)
    {
        QDomElement ele = list.at(i).toElement();
        if(ele.tagName() == "DEST")
            return ele;
    }

    //没找到 新建
    QDomElement element = m_document->createElement("DEST");
    element.setAttribute("value", "MemoryPort0");
    code.appendChild(element);
    return element;
}

