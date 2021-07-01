#ifndef CPROGRESSSTATUSBAR_H
#define CPROGRESSSTATUSBAR_H

#include <QWidget>

class QMovie;
class QWinTaskbarButton;

namespace Ui {
class CProgressStatusBar;
}

class CProgressStatusBar : public QWidget
{
    Q_OBJECT

public:
#if defined(Q_OS_WIN)
    explicit CProgressStatusBar(QWinTaskbarButton *progress, QWidget *parent = nullptr);
#else
    explicit CProgressStatusBar(QWidget *parent = nullptr);
#endif
    ~CProgressStatusBar();

    void readyToStart(const QString &iconFile);
    void finished();
    void setShowText(const QString &text);
public slots:
    void setProgressBarValue(int value);
	
private slots:
    void timeOut();
    void on_progressBar_valueChanged(int value);

private:
    Ui::CProgressStatusBar *ui;

    QMovie *m_movie = nullptr;
    QWinTaskbarButton *m_taskbarButton;
};

#endif // CPROGRESSSTATUSBAR_H
