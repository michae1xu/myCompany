#ifndef CSESSION_H
#define CSESSION_H

#include <QObject>
#include <QDateTime>

class QAction;

class CSession : public QObject
{
    Q_OBJECT

    friend QDataStream &operator<<(QDataStream &out, const CSession &session);
    friend QDataStream &operator>>(QDataStream &in, CSession &session);
    friend class CSessionManager;
public:
    explicit CSession(QObject *parent = nullptr);
    ~CSession();

    QString getSessionName() const;
    QString getLastModify();
    void setSessionName(const QString &getSessionName);
    void setCurrentSession();
    void updateActonText();

    CSession *clone(QObject *parent = nullptr);
    QAction *action() const;

    QStringList getOpenedSlnList() const;
    QString getCurSln() const;
    QString getCurProject() const;
    QStringList getOpenedXmlList() const;
    QString getCurXml() const;
    int getHorizontalScrollBarPos() const;
    int getVerticalScrollBarPos() const;
    QString getCurSrcProject() const;
    QString getCurSrc() const;
    int getCurStackedWidgetIndex() const;
    void setCurStackedWidgetIndex(int curStackedWidgetIndex);

private:
    QString m_sessionName;
    QAction *m_action;
    QDateTime m_lastModify;

    int m_curStackedWidgetIndex = 0;
    QString m_curXml;
    int m_horizontalScrollBarPos = 0;
    int m_verticalScrollBarPos = 0;
    QStringList m_openedXmlList;
    QString m_curSln;
    QString m_curProject;
    QStringList m_openedSlnList;
    QString m_curSrcProject;
    QString m_curSrc;

signals:
    void applyMe(QString);

public slots:
    void applySession();
};





QDataStream &operator<<(QDataStream &out, const CSession &session);
QDataStream &operator>>(QDataStream &in, CSession &session);
#endif // CSESSION_H
