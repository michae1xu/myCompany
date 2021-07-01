#include <QFinalState>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QDebug>
#include "cdebugalg.h"
#include "common/cappenv.hpp"
#include "fileresolver/cprojectresolver.h"
#include "csimulatorcontrol.h"

QHash<QString, QChar> CDebugAlg::m_bitIndex =
{
    {"0000", QChar('0')},
    {"0001", QChar('1')},
    {"0010", QChar('2')},
    {"0011", QChar('3')},
    {"0100", QChar('4')},
    {"0101", QChar('5')},
    {"0110", QChar('6')},
    {"0111", QChar('7')},
    {"1000", QChar('8')},
    {"1001", QChar('9')},
    {"1010", QChar('A')},
    {"1011", QChar('B')},
    {"1100", QChar('C')},
    {"1101", QChar('D')},
    {"1110", QChar('E')},
    {"1111", QChar('F')},
};

QHash<QChar, QString> CDebugAlg::m_hexIndex =
{
    {QChar('0'), "0000"},
    {QChar('1'), "0001"},
    {QChar('2'), "0010"},
    {QChar('3'), "0011"},
    {QChar('4'), "0100"},
    {QChar('5'), "0101"},
    {QChar('6'), "0110"},
    {QChar('7'), "0111"},
    {QChar('8'), "1000"},
    {QChar('9'), "1001"},
    {QChar('A'), "1010"},
    {QChar('B'), "1011"},
    {QChar('C'), "1100"},
    {QChar('D'), "1101"},
    {QChar('E'), "1110"},
    {QChar('F'), "1111"},
};

CDebugAlg::CDebugAlg(QObject *parent) :
    QObject(parent)
{
    m_ctrl = new rpu_esl_model::CSimulatorControl(CAppEnv::getEtcSimulatorConfigFilePath().toStdString());

//    QState *stateGroup = new QState();
//    QFinalState *debugFinishState = new QFinalState();
//    m_stateMachine.addState(stateGroup);
//    m_stateMachine.addState(debugFinishState);
//    m_stateMachine.setInitialState(stateGroup);
//    connect(&m_stateMachine, SIGNAL(finished()), this, SLOT(onFinished()));

//    QState *debugStateGroup = new QState(stateGroup);
//    QState *runToFinishState = new QState(stateGroup);
//    stateGroup->setInitialState(debugStateGroup);

//    QState *loadProjectState = new QState(debugStateGroup);
//    QState *nextCycleState = new QState(debugStateGroup);
//    QState *jumpToCycleState = new QState(debugStateGroup);
//    QState *runToBreakState = new QState(debugStateGroup);
//    QState *modifyValueState = new QState(debugStateGroup);
//    QState *setBreakState = new QState(debugStateGroup);
//    QState *unsetBreakState = new QState(debugStateGroup);

//    debugStateGroup->setInitialState(loadProjectState);
//    debugStateGroup->addTransition(this, SIGNAL(nextCycle()), nextCycleState);
//    debugStateGroup->addTransition(this, SIGNAL(jumpToCycle()), jumpToCycleState);
//    debugStateGroup->addTransition(this, SIGNAL(runToBreak()), runToBreakState);
//    debugStateGroup->addTransition(this, SIGNAL(runToFinish()), runToFinishState);
//    debugStateGroup->addTransition(this, SIGNAL(loadProject()), loadProjectState);
//    debugStateGroup->addTransition(this, SIGNAL(stopDebug()), debugFinishState);
//    debugStateGroup->addTransition(this, SIGNAL(modifyValue()), modifyValueState);
//    debugStateGroup->addTransition(this, SIGNAL(setBreak()), setBreakState);
//    debugStateGroup->addTransition(this, SIGNAL(unsetBreak()), unsetBreakState);
//    //    nextCycleState->addTransition(this, SIGNAL(nextCycle()), nextCycleState);
//    //    nextCycleState->addTransition(this, SIGNAL(jumpToCycle()), jumpToCycleState);
//    //    nextCycleState->addTransition(this, SIGNAL(runToBreak()), runToBreakState);
//    //    nextCycleState->addTransition(this, SIGNAL(runToFinish()), runToFinishState);
//    //    nextCycleState->addTransition(this, SIGNAL(loadNextXml()), loadXmlState);
//    //    nextCycleState->addTransition(this, SIGNAL(stopDebug()), debugFinishState);
//    //    jumpToCycleState->addTransition(this, SIGNAL(nextCycle()), nextCycleState);
//    //    jumpToCycleState->addTransition(this, SIGNAL(jumpToCycle()), jumpToCycleState);
//    //    jumpToCycleState->addTransition(this, SIGNAL(runToBreak()), runToBreakState);
//    //    jumpToCycleState->addTransition(this, SIGNAL(runToFinish()), runToFinishState);
//    //    jumpToCycleState->addTransition(this, SIGNAL(loadNextXml()), loadXmlState);
//    //    jumpToCycleState->addTransition(this, SIGNAL(stopDebug()), debugFinishState);
//    //    runToBreakState->addTransition(this, SIGNAL(nextCycle()), nextCycleState);
//    //    runToBreakState->addTransition(this, SIGNAL(jumpToCycle()), jumpToCycleState);
//    //    runToBreakState->addTransition(this, SIGNAL(runToBreak()), runToBreakState);
//    //    runToBreakState->addTransition(this, SIGNAL(runToFinish()), runToFinishState);
//    //    runToBreakState->addTransition(this, SIGNAL(loadNextXml()), loadXmlState);
//    //    runToBreakState->addTransition(this, SIGNAL(stopDebug()), debugFinishState);
//    //    modifyValueState->addTransition(this, SIGNAL(nextCycle()), nextCycleState);
//    //    modifyValueState->addTransition(this, SIGNAL(jumpToCycle()), jumpToCycleState);
//    //    modifyValueState->addTransition(this, SIGNAL(runToBreak()), runToBreakState);
//    //    modifyValueState->addTransition(this, SIGNAL(runToFinish()), runToFinishState);
//    //    modifyValueState->addTransition(this, SIGNAL(loadNextXml()), loadXmlState);
//    //    modifyValueState->addTransition(this, SIGNAL(stopDebug()), debugFinishState);
//    //    setBreakState->addTransition(this, SIGNAL(nextCycle()), nextCycleState);
//    //    setBreakState->addTransition(this, SIGNAL(jumpToCycle()), jumpToCycleState);
//    //    setBreakState->addTransition(this, SIGNAL(runToBreak()), runToBreakState);
//    //    setBreakState->addTransition(this, SIGNAL(runToFinish()), runToFinishState);
//    //    setBreakState->addTransition(this, SIGNAL(loadNextXml()), loadXmlState);
//    //    setBreakState->addTransition(this, SIGNAL(stopDebug()), debugFinishState);
//    //    unsetBreakState->addTransition(this, SIGNAL(nextCycle()), nextCycleState);
//    //    unsetBreakState->addTransition(this, SIGNAL(jumpToCycle()), jumpToCycleState);
//    //    unsetBreakState->addTransition(this, SIGNAL(runToBreak()), runToBreakState);
//    //    unsetBreakState->addTransition(this, SIGNAL(runToFinish()), runToFinishState);
//    //    unsetBreakState->addTransition(this, SIGNAL(loadNextXml()), loadXmlState);
//    //    unsetBreakState->addTransition(this, SIGNAL(stopDebug()), debugFinishState);
//    runToFinishState->addTransition(this, SIGNAL(loadProject()), loadProjectState);
//    runToFinishState->addTransition(this, SIGNAL(stopDebug()), debugFinishState);

//    connect(loadProjectState, SIGNAL(entered()), this, SLOT(onLoadProject()));
//    connect(nextCycleState, SIGNAL(entered()), this, SLOT(onNextCycle()));
//    connect(jumpToCycleState, SIGNAL(entered()), this, SLOT(onJumpToCycle()));
//    connect(runToBreakState, SIGNAL(entered()), this, SLOT(onRunToBreak()));
//    connect(modifyValueState, SIGNAL(entered()), this, SLOT(onModifyValue()));
//    connect(setBreakState, SIGNAL(entered()), this, SLOT(onSetBreak()));
//    connect(unsetBreakState, SIGNAL(entered()), this, SLOT(onUnsetBreak()));
//    connect(runToFinishState, SIGNAL(entered()), this, SLOT(onRunToFinish()));
}

CDebugAlg::~CDebugAlg()
{
    delete m_ctrl;
}

rpu_esl_model::ESIM_RET CDebugAlg::loadProject(const QString &projectFileName)
{
    ESIM_RET ret = m_ctrl->loadProject(projectFileName.toStdString());
    if(checkRet(ret))
    {
        m_ctrl->setDebugMode();
    } else
    {
//        emit stopDebugWithError();
    }
    return ret;
}

void CDebugAlg::getAllPEValue(vector<rpu_esl_model::SPEValue> &peValueList)
{
    ESIM_RET ret = m_ctrl->getAllPEValue(peValueList);
    if(!checkRet(ret))
    {
        emit stopDebugWithError();
    }
}

void CDebugAlg::onNextCycle()
{
    ESIM_RET ret = m_ctrl->debug_next();
    if(!checkRet(ret))
    {
        emit stopDebugWithError();
        return;
    }
    emit feedBackInfo(m_ctrl->getCurCycle(), !m_ctrl->getIsFinish());
}

void CDebugAlg::onJumpToCycle(int cycle)
{
    ESIM_RET ret = m_ctrl->debug_jump(cycle);
    if(!checkRet(ret))
    {
        emit stopDebugWithError();
        return;
    }
    emit feedBackInfo(m_ctrl->getCurCycle(), !m_ctrl->getIsFinish());
}

void CDebugAlg::onContinueToBreak()
{
    ESIM_RET ret = m_ctrl->debug_continue();
    if(!checkRet(ret))
    {
        emit stopDebugWithError();
        return;
    }
    emit feedBackInfo(m_ctrl->getCurCycle(), !m_ctrl->getIsFinish());
}

void CDebugAlg::onRunToFinish()
{
    ESIM_RET ret = m_ctrl->debug_finish();
    if(!checkRet(ret))
    {
        emit stopDebugWithError();
        return;
    }
    emit feedBackInfo(m_ctrl->getCurCycle(), !m_ctrl->getIsFinish());
}

void CDebugAlg::onModifyValue()
{
    ESIM_RET ret = m_ctrl->debug_finish();
    if(!checkRet(ret))
    {
        emit stopDebugWithError();
    }
}

void CDebugAlg::onSetBreak(SBreak &breakHandle)
{
    ESIM_RET ret = m_ctrl->debug_break_add(breakHandle);
    if(!checkRet(ret))
    {
        emit stopDebugWithError();
    }
}

void CDebugAlg::onRemoveBreak(SBreak &breakHandle)
{
    ESIM_RET ret = m_ctrl->debug_break_remove(breakHandle);
    if(!checkRet(ret))
    {
        emit stopDebugWithError();
    }
}

void CDebugAlg::onFinished()
{
    emit debugIsStopped();
}

bool CDebugAlg::checkRet()
{
    return checkRet(m_ctrl->getLastRet());
}

bool CDebugAlg::checkRet(ESIM_RET ret)
{
    switch (ret) {
    case ESIM_RET::RET_SUCCESS:
        return true;
        break;
    case ESIM_RET::RET_INFOMATION:
        emit sendMsg("<a><font color=#000000>Information: </font></a>");
        emit sendMsg(QString::fromStdString(m_ctrl->getRetMsg()));
        return true;
        break;
    case ESIM_RET::RET_WARNING:
        emit sendMsg(QString("<a><font color=#D87300>Warning: %1</font></a>").arg(QString::fromStdString(m_ctrl->getRetMsg())));
        return true;
        break;
    case ESIM_RET::RET_CRITICAL:
        emit sendMsg(QString("<a><font color=#DA0000>Critical: %1</font></a>").arg(QString::fromStdString(m_ctrl->getRetMsg())));
        return false;
        break;
    case ESIM_RET::RET_FATAL:
        emit sendMsg(QString("<a><font color=#DA0000>Fatal: %1</font></a>").arg(QString::fromStdString(m_ctrl->getRetMsg())));
        return false;
        break;
    default:
        Q_ASSERT(false);
        break;
    }
    return false;
}

QString CDebugAlg::bin2Hex(QString str, bool &ok)
{
    if(str.length() <= 64)
    {
        qint64 value = str.toInt(&ok, 2);
        return QString::number(value, 16);
    }
    else
    {
        QString ret;
        QString key;
        while(str.length()%4)
        {
            str.insert(0, "0");
        }
        for(int i = 0; i < str.length(); ++i)
        {
            key.append(str.at(i));
            if((i+1)%4 == 0 && i)
            {
                if(m_bitIndex.contains(key))
                {
                    ret.append(m_bitIndex[key]);
                }
                else
                {
                    ok = false;
                    return QString();
                }
                key.clear();
            }
        }
        ok = true;
        return ret;
    }
}

QString CDebugAlg::hex2Bin(QString str, bool &ok)
{
    qint64 value = str.toInt(&ok, 16);
    return QString::number(value, 2);
}

QString CDebugAlg::sourceToAbsolute(const QString &fileName)
{
    qInfo() << tr(u8"资源文件转为文件系统文件路径……");
    if(fileName.startsWith(":"))
    {
        return m_projectResolver->getProjectParameter()->resourceMap.value(fileName, fileName);
    }
    return fileName;
}

//void CDebugAlg::sendCmd(const QString &cmd)
//{
//    if(cmd.startsWith("print") || cmd.startsWith("info"))
//    {
//        if(m_stateMachine.isRunning())
//        {
//            m_ret.clear();
//            m_ctrl->executeCmd(cmd.toLocal8Bit().toStdString(), m_ret);
//            if(!checkRet())
//            {
//                emit stopDebugWithError();
//                return;
//            }
//            emit sendMsg(QString("<a><font color=#000000>Command:%1</font></a>").arg(cmd));
//            emit sendMsg(QString::fromStdString(m_ret));
//        }
//        else
//        {
//            emit sendMsg(QString("<a><font color=#DA0000>调试未开始！</font></a>"));
//        }
//    }
//    else if(cmd.startsWith("game"))
//    {
//#if defined(Q_OS_LINUX)
//        QProcess::startDetached(CAppPathEnv::getBinDirPath() + "/qmltest.sh");
//        emit sendMsg(QString(u8"发现了一个彩蛋，仅用于QML测试，请勿沉迷！"));
//#endif
//    }
//    else if(cmd.startsWith("help"))
//    {
//        emit sendMsg(QString("<a><font color=#000000>Usage: command [options=]\n</font></a>"));
//        QString str = tr("Commands:\n"
//                         "  print   打印算子当前值\n"
//                         "    Options:\n"
//                         "      bcu=     算子bcu编号\n"
//                         "      rcu=     算子rcu编号\n"
//                         "      rc=      算子编号\n"
//                         "      imd=     imd编号\n"
//                         "      rch=     rch编号\n"
//                         "      infifo=  infifo编号\n"
//                         "      outfifo= outfifo编号\n"
//                         "      mem=     memory编号\n"
//                         "  info    打印阵列状态信息\n"
//                         "  bin2hex 2进制转16进制\n"
//                         "  hex2bin 16进制转2进制\n"
//                         "  help    打印帮助信息\n");

//        emit sendMsg(str);
//    }
//    else if(cmd.startsWith("hex2bin ") || cmd.startsWith("bin2hex"))
//    {
//        QStringList list = cmd.split(" ");
//        bool fileFlag = false;
//        foreach (QString cell, list) {
//            if(cell == "-f")
//            {
//                fileFlag = true;
//                break;
//            }
//        }
//        if(fileFlag)
//        {
//            QString fileName = cmd.mid(cmd.indexOf(" -f ") + 3, cmd.length()).trimmed();
//            if(fileName.startsWith("\""))
//            {
//                int last = fileName.lastIndexOf("\"");
//                if(last == 0)
//                {
//                    emit sendMsg(tr(u8"文件路径有误！"));
//                    return;
//                }
//                fileName = fileName.mid(1, last - 1);
//                if(fileName.contains("\""))
//                {
//                    emit sendMsg(tr(u8"文件路径有误！"));
//                    return;
//                }
//            }
//            QFile file(fileName);
//            if(!file.open(QIODevice::ReadOnly))
//            {
//                emit sendMsg(tr(u8"文件打开失败！"));
//                return;
//            }
//            QTextStream in(&file);
//            while(!in.atEnd())
//            {
//                QString str = in.readLine();
//                bool ok;
//                QString num;
//                if(cmd.startsWith("hex2bin "))
//                    num = hex2Bin(str, ok);
//                else if(cmd.startsWith("bin2hex"))
//                    num = bin2Hex(str, ok);
//                if(ok)
//                    emit sendMsg(num);
//                else
//                    emit sendMsg("null");
//            }
//            file.close();
//        }
//        else
//        {
//            QString str = cmd.right(cmd.length() - 8);
//            bool ok;
//            QString num;
//            if(cmd.startsWith("hex2bin "))
//                num = hex2Bin(str, ok);
//            else if(cmd.startsWith("bin2hex"))
//                num = bin2Hex(str, ok);
//            if(ok)
//                emit sendMsg(num);
//            else
//                emit sendMsg("null");
//        }

//    }
//    else
//    {
//        emit sendMsg(QString("<a><font color=#000000>Command not found.\n</font></a>"));
//    }
//}
