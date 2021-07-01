#include <QMessageBox>
#include "sessionmanagerform.h"
#include "ui_sessionmanagerform.h"
#include "session/csessionmanager.h"
#include "session/csession.h"
#include "newsessiondialog.h"
#include "common/cappenv.hpp"

SessionManagerForm::SessionManagerForm(CSessionManager *manager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SessionManagerForm),
    m_sessionManager(manager)
{
    ui->setupUi(this);
    ui->checkBox_restore->setChecked(manager->getSetting()->restoreSessionFlag);
    ui->tableWidget_session->setColumnWidth(1, 150);

    addTableWidgetRow(manager->getDefaultSession());
    QList<CSession*> &&sessionList = m_sessionManager->getSessionList();
    for(int i = 0; i < sessionList.count(); ++i)
    {
        addTableWidgetRow(sessionList.at(i));
    }
    ui->tableWidget_session->setCurrentCell(0, 0);

    resize(geometry().size() * CAppEnv::m_dpiFactor);
}

SessionManagerForm::~SessionManagerForm()
{
    delete ui;
}

void SessionManagerForm::addTableWidgetRow(CSession *session)
{
    const int &row = ui->tableWidget_session->rowCount();
    ui->tableWidget_session->insertRow(row);
    QTableWidgetItem *item0 = new QTableWidgetItem(session->getSessionName());
    QTableWidgetItem *item1 = new QTableWidgetItem(session->getLastModify());
    ui->tableWidget_session->setItem(row, 0, item0);
    ui->tableWidget_session->setItem(row, 1, item1);
    if(m_sessionManager->isCurrentSession(session))
        setCurrentSession(session);
}

void SessionManagerForm::setCurrentSession(const CSession *session)
{
    for(int i = 0; i < ui->tableWidget_session->rowCount(); ++i)
    {
        QTableWidgetItem *item0 = ui->tableWidget_session->item(i, 0);
        QTableWidgetItem *item1 = ui->tableWidget_session->item(i, 1);
        QFont font = item0->font();
        font.setBold((item0->text() == session->getSessionName()));
        font.setItalic((item0->text() == session->getSessionName()));
        item1->setFont(font);
        item0->setFont(font);
    }
}

void SessionManagerForm::on_pushButton_new_clicked()
{
    if(m_sessionManager->getSessionList().count() >= 4)
    {
        QMessageBox::information(this, tr("会话管理"), tr("会话数已达上限！"));
        return;
    }
    NewSessionDialog dlg(this);
    dlg.setDialogTitle(tr("新建会话"));
    if(dlg.exec() == QDialog::Accepted)
    {
        QString sessionName = dlg.getSessionName().trimmed();
        getAvailableSessionName(sessionName);
        CSession *session = m_sessionManager->addSession(sessionName, dlg.getOpenFlag());
        addTableWidgetRow(session);
//        setCurrentSession(m_sessionManager->getCurrentSession()); //addTableWidgetRow完成此功能
    }
}

void SessionManagerForm::on_pushButton_rename_clicked()
{
    QTableWidgetItem *item = ui->tableWidget_session->item(ui->tableWidget_session->currentRow(), 0);
    if(!item)
        return;
    NewSessionDialog dlg(this);
    dlg.setDefaultSessionName(item->text());
    dlg.setDialogTitle(tr("重命名会话"));
    if(dlg.exec() == QDialog::Accepted)
    {
        QString sessionName = dlg.getSessionName();
        getAvailableSessionName(sessionName);
        m_sessionManager->renameSession(item->text(), sessionName, dlg.getOpenFlag());
        item->setText(sessionName);
        setCurrentSession(m_sessionManager->getCurrentSession());
    }
}

void SessionManagerForm::on_pushButton_clone_clicked()
{
    if(m_sessionManager->getSessionList().count() >= 4)
    {
        QMessageBox::information(this, tr("会话管理"), tr("会话数已达上限！"));
        return;
    }
    QTableWidgetItem *item = ui->tableWidget_session->item(ui->tableWidget_session->currentRow(), 0);
    if(!item)
        return;
    NewSessionDialog dlg(this);
    dlg.setDefaultSessionName(item->text());
    dlg.setDialogTitle(tr("克隆会话"));
    if(dlg.exec() == QDialog::Accepted)
    {
        QString sessionName = dlg.getSessionName().trimmed();
        getAvailableSessionName(sessionName);
        CSession *session = m_sessionManager->cloneSession(item->text(), sessionName, dlg.getOpenFlag());
        addTableWidgetRow(session);
        setCurrentSession(m_sessionManager->getCurrentSession());
    }
}

void SessionManagerForm::on_pushButton_delete_clicked()
{
    QTableWidgetItem *item = ui->tableWidget_session->item(ui->tableWidget_session->currentRow(), 0);
    if(!item)
        return;

    m_sessionManager->deleteSession(item->text());
    ui->tableWidget_session->removeRow(item->row());
    setCurrentSession(m_sessionManager->getCurrentSession());
//    ui->tableWidget_session->setRowCount(m_sessionManager->getSessionList().count() + 1);
}

void SessionManagerForm::on_pushButton_changeTo_clicked()
{
    QTableWidgetItem *item = ui->tableWidget_session->item(ui->tableWidget_session->currentRow(), 0);
    if(!item)
        return;

    m_sessionManager->applySession(item->text());
    setCurrentSession(m_sessionManager->getCurrentSession());
}

void SessionManagerForm::getAvailableSessionName(QString &sessionName)
{
    if(isAvailableSessionName(sessionName))
        return;

    int count = 2;
    sessionName += " (2)";
    while(!isAvailableSessionName(sessionName))
    {
        sessionName.chop(2);
        sessionName += QString("%0)").arg(QString::number(++count));
    }
}

bool SessionManagerForm::isAvailableSessionName(const QString &sessionName) const
{
    if(m_sessionManager->getDefaultSession()->getSessionName() == sessionName)
        return false;
    foreach (CSession *session, m_sessionManager->getSessionList()) {
        if(session->getSessionName() == sessionName) {
            return false;
        }
    }
    return true;
}

void SessionManagerForm::on_pushButton_close_clicked()
{
    m_sessionManager->getSetting()->restoreSessionFlag = ui->checkBox_restore->isChecked();
    this->accept();
}

void SessionManagerForm::on_tableWidget_session_cellClicked(int row, int column)
{
    Q_UNUSED(column)
    ui->pushButton_rename->setEnabled(row);
    ui->pushButton_delete->setEnabled(row);
}

void SessionManagerForm::on_tableWidget_session_cellDoubleClicked(int, int)
{
    on_pushButton_changeTo_clicked();
}
