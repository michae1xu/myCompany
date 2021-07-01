#include "cmergethread.h"
#include <QProcess>
#include <QDebug>

CMergeThread::CMergeThread(QString &cmd) : m_cmd(cmd)
{

}

void CMergeThread::run()
{
    int exitCode = QProcess::execute(m_cmd);
    qDebug() << "exitCode";
    emit mergeExit(exitCode);
}
