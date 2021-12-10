#include "newBloodDialog.h"
#include "ui_newBloodDialog.h"

newBloodDialog::newBloodDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newBloodDialog)
{
    ui->setupUi(this);
    QStringList pGenderList;
    pGenderList << "男" << "女";
    ui->pGender->addItems(pGenderList);
}

newBloodDialog::~newBloodDialog()
{
    QString strName = ui->pName->text().trimmed();
    QString strGender = ui->pGender->itemText(ui->pGender->currentIndex());
    delete ui;
}

void newBloodDialog::on_buttonBoxOk_clicked()
{
}

