#ifndef DLG_WAIT_H
#define DLG_WAIT_H

#include <QDialog>
#include <QPointer>
#include <QMutex>

namespace Ui {
class dlg_wait;
}

class dlg_wait : public QDialog
{
    Q_OBJECT
    
public:
    explicit dlg_wait(QWidget *parent);
    ~dlg_wait();

    void setText(const QString &str);

protected:
    void keyPressEvent(QKeyEvent *event);

    void keyReleaseEvent(QKeyEvent *event);

    void closeEvent(QCloseEvent *event);
private:
    Ui::dlg_wait *ui;

    bool m_closeFlag = false;

};

class CWaitDialog : public QObject
{
    Q_OBJECT
public:
    static CWaitDialog *getInstance(QWidget *parent = nullptr);
    static void releaseInstance();
    static void readyToShowDlg();
    static void finishShowing();

    static CWaitDialog *m_inst;
    static QMutex m_mutex;
public:
    CWaitDialog(QWidget *parent);
    ~CWaitDialog();

    QPointer<dlg_wait> m_dlg;
    int m_currTimerId;

    void setText(const QString &str = "");

protected:
    void timerEvent(QTimerEvent *ev);
};

#endif // DLG_WAIT_H
