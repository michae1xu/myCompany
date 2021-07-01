#ifndef CSESSIONMANAGER_H
#define CSESSIONMANAGER_H

#include <QObject>
#include <QList>
#include "datafile.h"
#include "csession.h"

class CSession;

class CSessionManager : public QObject
{
    Q_OBJECT
public:
    explicit CSessionManager(SSetting *setting, QObject *parent = nullptr);
    ~CSessionManager();

    void updateSession(const QByteArray &array);
    CSession *addSession(const QString &sessionName, const bool &openFlag);
    bool renameSession(const QString &oldSessionName, const QString &newSessionName, const bool &openFlag);
    CSession *cloneSession(const QString &sourceSessionName, const QString &destSessionName, const bool &openFlag);
    void deleteSession(const QString &sessionName);
    void saveAllSession();
    bool isCurrentSession(const CSession *session);

    CSession *getDefaultSession() const;
    CSession *getCurrentSession() const;
    QString getCurrentSessionName() const;
    QList<CSession *> getSessionList() const;

    SSetting *getSetting() const;

signals:
    void addSessionAction(QAction *);
    void updateCurrentSession();
    void aboutToLoadSession();

public slots:
    void applySession(const QString &sessionName);
    void restoreSession(const QString &sessionName);

private:
    SSetting *m_setting;
    CSession *m_defaultSession;
    CSession *m_currentSession = nullptr;
    QList<CSession*> m_sessionList;
};

#endif // CSESSIONMANAGER_H
