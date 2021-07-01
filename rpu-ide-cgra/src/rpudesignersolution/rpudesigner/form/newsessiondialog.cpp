#include "newsessiondialog.h"
#include "ui_newsessiondialog.h"
#include "common/cappenv.hpp"

NewSessionDialog::NewSessionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewSessionDialog)
{
    ui->setupUi(this);

    resize(geometry().size() * CAppEnv::m_dpiFactor);
}

NewSessionDialog::~NewSessionDialog()
{
    delete ui;
}

void NewSessionDialog::setDialogTitle(const QString &text)
{
    setWindowTitle(text);
}

void NewSessionDialog::setDefaultSessionName(const QString &sessionName)
{
    ui->lineEdit_sessionName->setText(sessionName);
}

void NewSessionDialog::on_pushButton_create_clicked()
{
    m_sessionName = ui->lineEdit_sessionName->text().trimmed();
}

void NewSessionDialog::on_pushButton_createAndOpen_clicked()
{
    m_sessionName = ui->lineEdit_sessionName->text().trimmed();
    m_openFlag = true;
}

QString NewSessionDialog::getSessionName() const
{
    return m_sessionName;
}

bool NewSessionDialog::getOpenFlag() const
{
    return m_openFlag;
}
