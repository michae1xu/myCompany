#include <QIODevice>
#include <QDataStream>
#include "common/debug.h"
#include "cdebugsession.h"

CDebugSession::CDebugSession(QObject *parent) : QObject(parent)
{

}

CDebugSession::~CDebugSession()
{
}

QString CDebugSession::getCurrentProjectName() const
{
    return m_currentProjectName;
}

void CDebugSession::setCurrentProjectName(const QString &currentProjectName)
{
    m_currentProjectName = currentProjectName;
}

QMap<QString, bool> CDebugSession::getBreakPointMap() const
{
    return m_breakPointMap;
}

QList<SPeValue> CDebugSession::getModifyPeValueList() const
{
    return m_modifyPeValueList;
}

uint CDebugSession::getLastCycle() const
{
    return m_lastCycle;
}

int CDebugSession::getCurrentXmlIndex() const
{
    return m_currentXmlIndex;
}

int CDebugSession::getStackedWidgetIndex() const
{
    return m_stackedWidgetIndex;
}

int CDebugSession::getVerticalScrollBarPos() const
{
    return m_verticalScrollBarPos;
}

int CDebugSession::getHorizontalScrollBarPos() const
{
    return m_horizontalScrollBarPos;
}

void CDebugSession::setCurrentXmlIndex(int currentXmlIndex)
{
    m_currentXmlIndex = currentXmlIndex;
}

void CDebugSession::setStackedWidgetIndex(int curStackedWidgetIndex)
{
    m_stackedWidgetIndex = curStackedWidgetIndex;
}

void CDebugSession::setVerticalScrollBarPos(int verticalScrollBarPos)
{
    m_verticalScrollBarPos = verticalScrollBarPos;
}

void CDebugSession::setHorizontalScrollBarPos(int horizontalScrollBarPos)
{
    m_horizontalScrollBarPos = horizontalScrollBarPos;
}

//void CDebugSession::exportData(QDataStream &out)
//{
//    out << m_sessionName << m_currentProjectName << m_lastCycle << m_horizontalScrollBarPos
//        << m_verticalScrollBarPos << m_currentXmlIndex << m_curStackedWidgetIndex
//        << m_modifyPeValueList.count() << m_breakPointMap.count();

//    foreach (const SPeValue &peValue, m_modifyPeValueList) {
//        out << peValue;
//    }

//    QMapIterator<QString, bool> i(m_breakPointMap);
//    while(i.hasNext())
//    {
//        i.next();
//        out << i.key() << i.value();
//    }

//}

//void CDebugSession::importData(QDataStream &in)
//{
//    int modifyPeValueListCount = 0;
//    int breakPointMap = 0;
//    in >> m_sessionName >> m_currentProjectName >> m_lastCycle >> m_horizontalScrollBarPos
//            >> m_verticalScrollBarPos >> m_currentXmlIndex >> m_curStackedWidgetIndex
//            >> modifyPeValueListCount >> breakPointMap;

//    m_modifyPeValueList.clear();
//    SPeValue peValue;
//    for(int i = 0; i < modifyPeValueListCount; ++i)
//    {
//        in >> peValue;
//        m_modifyPeValueList.append(peValue);
//    }

//    m_breakPointMap.clear();
//    QString breakPoint;
//    bool enable;
//    for(int i = 0; i < m_breakPointMap.count(); ++i)
//    {
//        in >> breakPoint >> enable;
//        m_breakPointMap.insert(breakPoint, enable);
//    }
//}

void CDebugSession::addModifyPeValue(const SPeValue &peValue)
{
    m_modifyPeValueList.append(peValue);
}

void CDebugSession::addBreakPoint(const QString &str, const bool &enable)
{
    m_breakPointMap.insert(str, enable);
}

void CDebugSession::setLastCycle(uint lastCycle)
{
    m_lastCycle = lastCycle;
}

QDataStream &operator<<(QDataStream &out, const SPeValue &peValue)
{
    out << peValue.cycle << peValue.element << peValue.input << peValue.output;
    return out;
}

QDataStream &operator>>(QDataStream &in, SPeValue &peValue)
{
    in >> peValue.cycle >> peValue.element >> peValue.input >> peValue.output;
    return in;
}

QDataStream &operator<<(QDataStream &out, const CDebugSession &session)
{
    out << session.m_sessionName << session.m_currentProjectName << session.m_lastCycle << session.m_horizontalScrollBarPos
        << session.m_verticalScrollBarPos << session.m_currentXmlIndex << session.m_stackedWidgetIndex
        << session.m_modifyPeValueList << session.m_breakPointMap;
    return out;
}

QDataStream &operator>>(QDataStream &out, CDebugSession &session)
{
    out >> session.m_sessionName >> session.m_currentProjectName >> session.m_lastCycle >> session.m_horizontalScrollBarPos
        >> session.m_verticalScrollBarPos >> session.m_currentXmlIndex >> session.m_stackedWidgetIndex
        >> session.m_modifyPeValueList >> session.m_breakPointMap;
    return out;
}

