#ifndef CPROJECTRESOLVER_H
#define CPROJECTRESOLVER_H

#include <QObject>
#include <QDomElement>
#include <QDateTime>
#include <memory>
#include "datafile.h"

class QFile;
class QDomDocument;

class CProjectResolver : public QObject
{
    Q_OBJECT
public:
    CProjectResolver(const QString &fileName, const QString &solutionName, ECipherType cipherType);
    ~CProjectResolver();

    SProjectParameter* getProjectParameter() const {return m_projectParameter;}
    bool initProInfo();
    void paraChanged();
    bool save();
    bool isStarted(){return m_isStart;}
    QString getProjectFileName() const {return m_fileName;}
    bool renameProject(const QString &oldName, const QString &newName);

    void sortXml(int type = 1);
    bool okToContinue();
    void setDirty();
    bool getIsDirty() const {return m_isModified;}

    bool getHasResult(){return m_hasResult;}
    QStringList getOutfifoList();  // FIXME remove it
    QStringList getMemoryList();  // FIXME remove it
    QString getDateTime(){return m_dateTime.toString("yyyy/MM/dd hh:mm:ss");}
    void setHasResult(bool hasResult){m_hasResult = hasResult;}
    void setOutfifoRunResult(const int coreIndex, const std::shared_ptr<std::vector<int> > &v);
    void setMemoryRunResult(const int coreIndex, const std::shared_ptr<std::vector<int> > &v);
    void setOutfifoList(const QStringList &strList);  // FIXME remove it
    void setMemoryList(const QStringList &strList);  // FIXME remove it
    std::vector<int> getOutfifoRunResult(const int coreIndex) const;
    std::vector<int> getMemoryRunResult(const int coreIndex) const;
    inline void setDateTime(){m_dateTime = QDateTime::currentDateTime();}
    QList<SXmlParameter> m_lastRunXmlParameterList;   //上一次运行时选择的xml

    void appendXmlElement(const QString &fileName);
    void removeXmlElement(const QString &xmlName);
    void changeXmlElement(const QString &oldXmlName, const QString &newXmlName);

    void appendSrcElement(const QString &fileName);
    void removeSrcElement(const QString &srcName);
    void changeSrcElement(const QString &oldSrcName, const QString &newSrcName);


signals:
    void refreshed();
    void projectSaved(QString);

private:
    QFile *m_file = nullptr;
    QString m_fileName;
    QDomDocument* m_document = nullptr;
    QDomElement m_root;
    bool m_isStart = false;
    bool m_isModified = false;
    SProjectParameter *m_projectParameter;
//    QByteArray m_md5Code ;
//    QByteArray makeMd5();
//    bool md5Check();

    bool m_hasResult = false;               //是否有运行结果
    QDateTime m_dateTime;
    QStringList m_outfifoList;      //记录运行输出结果
    QStringList m_memoryList;       //记录运行输出结果

    std::vector<int> m_outfifoRunResult[16];
    std::vector<int> m_memoryRunResult[16];

    void refreshProjectParameter();

    QDomElement getXmlElement(const QString &value);
    QDomElement getSrcElement(const QString &value);
    QDomElement getCycleElement(QDomElement xml);
    QDomElement getIntervalElement(QDomElement xml);
    QDomElement getFifoElement(QDomElement xml);
    QDomElement getFifoInputElement(QDomElement xml, int id);
    QDomElement getMemElement(QDomElement xml);
    QDomElement getMemInputElement(QDomElement xml, int id);
    QDomElement getCodeElement(QDomElement xml);
    QDomElement getCodeChildElement(QDomElement xml, const QString &childName);
    QDomElement getCodeSourceElement(QDomElement xml);
    QDomElement getCodeDestElement(QDomElement xml);
};

#endif // CPROJECTRESOLVER_H
