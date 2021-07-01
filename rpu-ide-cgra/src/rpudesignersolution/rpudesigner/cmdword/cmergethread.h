#ifndef CMERGETHREAD_H
#define CMERGETHREAD_H

#include <QObject>
#include <QThread>

class CMergeThread : public QThread
{
    Q_OBJECT
public:
    explicit CMergeThread(QString &cmd);

    void run();

signals:
    void mergeExit(int);

private:
    QString m_cmd;
};

#endif // CMERGETHREAD_H
