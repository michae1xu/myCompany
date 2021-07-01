#include "cnoclosewidget.h"
#include "common/cappenv.hpp"
#include "common/debug.h"
#include <QCloseEvent>

CNoCloseWidget::CNoCloseWidget(QWidget *parent) : QWidget(parent)
{
    this->setWindowIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Tools1-128.png"));
    this->setWindowTitle(tr("XML编辑器"));
}

CNoCloseWidget::~CNoCloseWidget()
{
    DBG << "~CNoCloseWidget()";
}

void CNoCloseWidget::showAsTool()
{
    setWindowFlags(Qt::Tool);
    move(parentWidget()->pos() + QPoint(20, 10));
    show();
}

void CNoCloseWidget::showAsSubwindow()
{
    setWindowFlags(Qt::SubWindow);
    showNormal();
}

void CNoCloseWidget::closeEvent(QCloseEvent *event)
{
    DBG << "close event: refuse to close";
    event->ignore();
    showAsSubwindow();
}
