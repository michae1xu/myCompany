#include "ctitlebar.h"
#include "ui_ctitlebar.h"
#include "common/cappenv.hpp"
#include "common/debug.h"
#include <QMouseEvent>

CTitleBar::CTitleBar(QWidget *parent):
    QWidget(parent),
    ui(new Ui::CTitleBar)
{
    ui->setupUi(this);

    ui->pushButton->setIcon(QIcon(CAppEnv::getImageDirPath() + "/close.png"));
    ui->pushButton->setIconSize(ui->pushButton->iconSize() * CAppEnv::m_dpiFactor);

    QPalette pe;
    pe.setColor(QPalette::WindowText, QColor("#D2D2D2"));
    ui->label->setPalette(pe);
}

CTitleBar::CTitleBar(QString text, QWidget *parent) : CTitleBar(parent){
    ui->label->setText(text);
}

CTitleBar::~CTitleBar()
{
    delete ui;
}

void CTitleBar::setTitleText(const QString &text)
{
    ui->label->setText(text);
}

QString CTitleBar::getTitleText() const
{
    return ui->label->text();
}

void CTitleBar::on_pushButton_clicked()
{
    emit closeDockWidget(false);
}

void CTitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    event->accept();
}
