#include <QDataStream>
#include <QAction>
#include "csession.h"

CSession::CSession(QObject *parent) : QObject(parent)
{
    m_lastModify = QDateTime::currentDateTime();
    m_action = new QAction(this);
    m_action->setCheckable(true);
    connect(m_action, SIGNAL(triggered(bool)), SLOT(applySession()));
}

CSession::~CSession()
{
}

CSession *CSession::clone(QObject *parent)
{
    CSession *session = new CSession(parent);
    QByteArray bty;
    QDataStream out(&bty, QIODevice::WriteOnly);
    out << *this;
    QDataStream in(bty);
    in >> *session;
    return session;
}

QString CSession::getLastModify()
{
    if(m_lastModify.date().daysTo(QDate::fromString("1970/01/01", "yyyy/MM/dd")) > 0)
    {
        m_lastModify = QDateTime::currentDateTime();
    }
    return m_lastModify.toString("yyyy/MM/dd hh:mm");
}


QAction *CSession::action() const
{
    return m_action;
}

QStringList CSession::getOpenedSlnList() const
{
    return m_openedSlnList;
}

QString CSession::getCurSln() const
{
    return m_curSln;
}

QString CSession::getCurProject() const
{
    return m_curProject;
}

QStringList CSession::getOpenedXmlList() const
{
    return m_openedXmlList;
}

QString CSession::getCurXml() const
{
    return m_curXml;
}

int CSession::getHorizontalScrollBarPos() const
{
    return m_horizontalScrollBarPos;
}

int CSession::getVerticalScrollBarPos() const
{
    return m_verticalScrollBarPos;
}

QString CSession::getCurSrcProject() const
{
    return m_curSrcProject;
}

QString CSession::getCurSrc() const
{
    return m_curSrc;
}

int CSession::getCurStackedWidgetIndex() const
{
    return m_curStackedWidgetIndex;
}

void CSession::setCurStackedWidgetIndex(int curStackedWidgetIndex)
{
    m_curStackedWidgetIndex = curStackedWidgetIndex;
}

void CSession::applySession()
{
    emit applyMe(m_sessionName);
}

QString CSession::getSessionName() const
{
    return m_sessionName;
}

void CSession::setSessionName(const QString &sessionName)
{
    m_sessionName = sessionName;
    m_action->setText(m_sessionName);
}

void CSession::setCurrentSession()
{
    m_action->setChecked(true);
}

void CSession::updateActonText()
{
    m_action->setText(m_sessionName);
}


QDataStream &operator<<(QDataStream &out, const CSession &session)
{
    out << session.m_sessionName << session.m_curXml << session.m_lastModify
        << session.m_horizontalScrollBarPos << session.m_verticalScrollBarPos
        << session.m_openedXmlList << session.m_curSln << session.m_curProject
        << session.m_openedSlnList << session.m_curSrcProject
        << session.m_curSrc << session.m_curStackedWidgetIndex;
    return out;
}

QDataStream &operator>>(QDataStream &in, CSession &session)
{
    in >> session.m_sessionName >> session.m_curXml >> session.m_lastModify
            >> session.m_horizontalScrollBarPos >> session.m_verticalScrollBarPos
            >> session.m_openedXmlList >> session.m_curSln >> session.m_curProject
            >> session.m_openedSlnList >> session.m_curSrcProject
            >> session.m_curSrc >> session.m_curStackedWidgetIndex;
    return in;
}
