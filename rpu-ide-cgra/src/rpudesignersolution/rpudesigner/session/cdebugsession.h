#ifndef CDEBUGSESSION_H
#define CDEBUGSESSION_H

#include <QObject>
#include <QMap>

typedef struct _tagPeValue
{
    uint cycle;
    QString element;
    QStringList input;
    QStringList output;
} SPeValue;


class CDebugSession : public QObject
{
    Q_OBJECT

    friend QDataStream &operator<<(QDataStream &out, const SPeValue &peValue);
    friend QDataStream &operator>>(QDataStream &in, SPeValue &peValue);
    friend QDataStream &operator<<(QDataStream &out, const CDebugSession &session);
    friend QDataStream &operator>>(QDataStream &out, CDebugSession &session);
public:
    explicit CDebugSession(QObject *parent = nullptr);
    ~CDebugSession();

    void setCurrentProjectName(const QString &getCurrentProjectName);
    QString getCurrentProjectName() const;
    void setLastCycle(uint getLastCycle);
    uint getLastCycle() const;
    void setHorizontalScrollBarPos(int getHorizontalScrollBarPos);
    int getHorizontalScrollBarPos() const;
    void setVerticalScrollBarPos(int getVerticalScrollBarPos);
    int getVerticalScrollBarPos() const;
    void setStackedWidgetIndex(int curStackedWidgetIndex);
    int getStackedWidgetIndex() const;
    void setCurrentXmlIndex(int getCurrentXmlIndex);
    int getCurrentXmlIndex() const;
    void addModifyPeValue(const SPeValue &peValue);
    QList<SPeValue> getModifyPeValueList() const;
    void addBreakPoint(const QString &str, const bool &enable);
    QMap<QString, bool> getBreakPointMap() const;

//    void exportData(QDataStream &out);
//    void importData(QDataStream &in);

private:
    QString m_sessionName = "default";
    QString m_currentProjectName;
    uint m_lastCycle = 0;
    int m_horizontalScrollBarPos = 0;
    int m_verticalScrollBarPos = 0;
    int m_currentXmlIndex = 0;
    int m_stackedWidgetIndex = 0;
    QList<SPeValue> m_modifyPeValueList;
    QMap<QString, bool> m_breakPointMap;
};










QDataStream &operator<<(QDataStream &out, const SPeValue &peValue);
QDataStream &operator>>(QDataStream &in, SPeValue &peValue);
QDataStream &operator<<(QDataStream &out, const CDebugSession &session);
QDataStream &operator>>(QDataStream &out, CDebugSession &session);

#endif // CDEBUGSESSION_H
