#ifndef CDEBUGALG_H
#define CDEBUGALG_H

#include <QObject>
#include <QStateMachine>
#include <string>
#include "datafile.h"
#include "export_typedef.h"

namespace rpu_esl_model {
class CSimulatorControl;
}
class CProjectResolver;

using namespace std;

class CDebugAlg : public QObject
{
    Q_OBJECT
public:
    explicit CDebugAlg(QObject *parent = nullptr);
    ~CDebugAlg();

    rpu_esl_model::ESIM_RET loadProject(const QString &projectFileName);

    void getAllPEValue(vector<rpu_esl_model::SPEValue> &peValueList);

public slots:
    void onNextCycle();
    void onJumpToCycle(int cycle);
    void onContinueToBreak();
    void onRunToFinish();
    void onModifyValue();
    void onSetBreak(rpu_esl_model::SBreak &breakHandle);
    void onRemoveBreak(rpu_esl_model::SBreak &breakHandle);
    void onFinished();


signals:
    void loadProject();
    void nextCycle();
    void jumpToCycle();
    void runToBreak();
    void runToFinish();
    void stopDebug();
    void stopDebugWithError();
    void modifyValue();
    void setBreak();
    void unsetBreak();


    void feedBackInfo(int curCycle, bool hasNextCycle);
    void feedBackCurXmlIndex(int xmlIndex);
    void debugIsStopped();
    void sendMsg(QString);


private:
    rpu_esl_model::CSimulatorControl *m_ctrl = nullptr;
    int m_jumpToCycle;
    int m_jumpToXml;
    int m_curXmlIndex;
    int m_maxCycle;
    string m_cmd;
    CProjectResolver *m_projectResolver;

    bool checkRet();
    bool checkRet(rpu_esl_model::ESIM_RET ret);

public:
    QString sourceToAbsolute(const QString &fileName);

    static QHash<QChar, QString> m_hexIndex;
    static QHash<QString, QChar> m_bitIndex;
    static QString bin2Hex(QString str, bool &ok);
    static QString hex2Bin(QString str, bool &ok);

};

#endif // CDEBUGALG_H
