#include "csessionmanager.h"
#include "common/cappenv.hpp"
#include <QDebug>

CSessionManager::CSessionManager(SSetting *setting, QObject *parent) : QObject(parent),
    m_setting(setting)
{
    QDataStream in(m_setting->sessions);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    in.setVersion(QDataStream::Qt_5_10);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
    in.setVersion(QDataStream::Qt_5_9);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
    in.setVersion(QDataStream::Qt_5_8);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    in.setVersion(QDataStream::Qt_5_7);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    in.setVersion(QDataStream::Qt_5_6);
#endif
    m_defaultSession = new CSession(this);
    m_defaultSession->updateActonText();
    connect(m_defaultSession, SIGNAL(applyMe(QString)), this, SLOT(applySession(QString)));
    if(!m_setting->sessions.isEmpty())
    {
        in >> *m_defaultSession;
    }
    m_defaultSession->setSessionName("default");
    m_currentSession = m_defaultSession;
    m_currentSession->setCurrentSession();

    for(int i = 0; i < m_setting->sessionCount; ++i)
    {
        CSession *session = new CSession(this);
        connect(session, SIGNAL(applyMe(QString)), this, SLOT(applySession(QString)));
        in >> *session;
        session->updateActonText();
        if(!session->m_sessionName.isEmpty())
            m_sessionList.append(session);
        else
            delete session;
    }
}

CSessionManager::~CSessionManager()
{
}

void CSessionManager::applySession(const QString &sessionName)
{
    emit updateCurrentSession();
    restoreSession(sessionName);
}

void CSessionManager::restoreSession(const QString &sessionName)
{
    if(m_defaultSession->m_sessionName == sessionName)
    {
        m_currentSession = m_defaultSession;
    }
    else
    {
        foreach (CSession *session, m_sessionList) {
            if(session->m_sessionName == sessionName)
            {
                m_currentSession = session;
                break;
            }
        }
    }
    m_currentSession->setCurrentSession();
    emit aboutToLoadSession();
}

SSetting *CSessionManager::getSetting() const
{
    return m_setting;
}

void CSessionManager::updateSession(const QByteArray &array)
{
    QDataStream in(array);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    in.setVersion(QDataStream::Qt_5_10);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
    in.setVersion(QDataStream::Qt_5_9);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
    in.setVersion(QDataStream::Qt_5_8);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    in.setVersion(QDataStream::Qt_5_7);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    in.setVersion(QDataStream::Qt_5_6);
#endif
    in >> *m_currentSession;
//    DBG << m_currentSession->m_sessionName << m_currentSession->m_curXml << m_currentSession->m_lastModify
//        << m_currentSession->m_horizontalScrollBarPos << m_currentSession->m_verticalScrollBarPos
//        << m_currentSession->m_openedXmlList << m_currentSession->m_curSln << m_currentSession->m_curProject
//        << m_currentSession->m_openedSlnList << m_currentSession->m_curSrcProject
//        << m_currentSession->m_curSrc << m_currentSession->m_curStackedWidgetIndex;
}

CSession *CSessionManager::addSession(const QString &sessionName, const bool &openFlag)
{
    if(sessionName.isEmpty())
        return nullptr;

    emit updateCurrentSession();
    CSession *session = m_currentSession->clone(this);
    connect(session, SIGNAL(applyMe(QString)), this, SLOT(applySession(QString)));
    session->setSessionName(sessionName);
    emit addSessionAction(session->action());
    m_sessionList.append(session);
    if(openFlag)
    {
        session->applySession();
    }
    return session;
}

bool CSessionManager::renameSession(const QString &oldSessionName, const QString &newSessionName, const bool &openFlag)
{
    if(newSessionName.isEmpty())
        return false;

    Q_ASSERT(m_defaultSession->m_sessionName != oldSessionName);
    foreach (CSession *session, m_sessionList) {
        if(session->m_sessionName == oldSessionName)
        {
            emit updateCurrentSession();
            session->setSessionName(newSessionName);
            if(openFlag)
            {
                session->applySession();
            }
            break;
        }
    }
    return true;
}

CSession *CSessionManager::cloneSession(const QString &sourceSessionName, const QString &destSessionName, const bool &openFlag)
{
    if(destSessionName.isEmpty())
        return nullptr;

    CSession *sourceSession = nullptr;
    if(m_defaultSession->m_sessionName == sourceSessionName)
    {
        sourceSession = m_defaultSession;
    }
    else
    {
        foreach (CSession *session, m_sessionList) {
            if(session->m_sessionName == sourceSessionName)
            {
                sourceSession = session;
                break;
            }
        }
    }
    if(!sourceSession)
        return sourceSession;

    emit updateCurrentSession();
    CSession *destSession = sourceSession->clone(this);
    connect(destSession, SIGNAL(applyMe(QString)), this, SLOT(applySession(QString)));
    destSession->setSessionName(destSessionName);
    emit addSessionAction(destSession->action());
    m_sessionList.append(destSession);
    if(openFlag)
    {
        destSession->applySession();
    }
    return destSession;
}

void CSessionManager::deleteSession(const QString &sessionName)
{
    Q_ASSERT(m_defaultSession->m_sessionName != sessionName);
    emit updateCurrentSession();
    foreach (CSession *session, m_sessionList) {
        if(session->m_sessionName == sessionName)
        {
            if(session == m_defaultSession)
                return;
            if(session == m_currentSession)
            {
//                m_currentSession = m_defaultSession;
//                m_currentSession->applySession();
                m_defaultSession->applySession();
            }
            m_sessionList.removeOne(session);
            delete session;
            break;
        }
    }
}

void CSessionManager::saveAllSession()
{
    m_setting->sessionCount = m_sessionList.count();
    m_setting->sessions.clear();
    QDataStream out(&m_setting->sessions, QIODevice::WriteOnly);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    out.setVersion(QDataStream::Qt_5_10);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
    out.setVersion(QDataStream::Qt_5_9);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
    out.setVersion(QDataStream::Qt_5_8);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    out.setVersion(QDataStream::Qt_5_7);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    out.setVersion(QDataStream::Qt_5_6);
#endif
    out << *m_defaultSession;
    foreach (CSession *session, m_sessionList) {
        out << *session;
    }
}

bool CSessionManager::isCurrentSession(const CSession *session)
{
    return (session == m_currentSession);
}

CSession *CSessionManager::getCurrentSession() const
{
    return m_currentSession;
}

QString CSessionManager::getCurrentSessionName() const
{
    return m_currentSession->getSessionName();
}

QList<CSession *> CSessionManager::getSessionList() const
{
    return m_sessionList;
}

CSession *CSessionManager::getDefaultSession() const
{
    return m_defaultSession;
}
