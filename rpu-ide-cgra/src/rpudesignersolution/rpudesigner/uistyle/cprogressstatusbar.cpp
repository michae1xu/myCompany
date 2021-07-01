#include <QTimer>
#include <QMovie>
#include "common/cappenv.hpp"
#include "cprogressstatusbar.h"
#include "ui_cprogressstatusbar.h"
#if defined(Q_OS_WIN)
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#endif

#if defined(Q_OS_WIN)
CProgressStatusBar::CProgressStatusBar(QWinTaskbarButton *progress, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CProgressStatusBar),
    m_taskbarButton(progress)
#else
CProgressStatusBar::CProgressStatusBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CProgressStatusBar)
#endif
{
    ui->setupUi(this);
    QFontMetrics fontMetrics(ui->label_text->font());
    setFixedWidth(fontMetrics.width(tr(u8"请稍后")) + 100);
    ui->progressBar->setFixedSize(60, 10);

    m_movie = new QMovie(CAppEnv::getImageDirPath() + "/waitting72.gif", "", ui->label_movie);
    m_movie->setScaledSize(QSize(12, 12));
    ui->label_movie->setFixedSize(12, 12);
    ui->label_movie->setMovie(m_movie);
//    ui->label_text->setFixedWidth(60);
    m_movie->start();
    timeOut();
}

CProgressStatusBar::~CProgressStatusBar()
{
    delete ui;
//    delete m_movie;
}

void CProgressStatusBar::readyToStart(const QString &iconFile)
{
    Q_UNUSED(iconFile)
//    m_movie->start();
//    ui->label_movie->setVisible(true);
    this->setVisible(true);
#if defined(Q_OS_WIN)
    m_taskbarButton->progress()->setVisible(true);
    m_taskbarButton->setOverlayIcon(QIcon(iconFile));
#endif
}

void CProgressStatusBar::finished()
{
//    m_movie->stop();
//    ui->label_movie->setVisible(false);
    QTimer::singleShot(2000, this, SLOT(timeOut()));
}

void CProgressStatusBar::setProgressBarValue(int value)
{
    ui->progressBar->setValue(value);
#if defined(Q_OS_WIN)
    m_taskbarButton->progress()->setValue(value);
#endif
}

void CProgressStatusBar::setShowText(const QString &text)
{
    ui->label_text->setText(text);
}

void CProgressStatusBar::timeOut()
{
#if defined(Q_OS_WIN)
    m_taskbarButton->progress()->setVisible(false);
    m_taskbarButton->clearOverlayIcon();
#endif
    this->setVisible(false);
    ui->label_text->setText(tr(u8"请稍后"));
}

void CProgressStatusBar::on_progressBar_valueChanged(int value)
{
    if(value == 100)
    {
//        ui->label_text->setText(tr(u8"完成"));
    }
}
