#include "resourcetypeform.h"
#include "ui_resourcetypeform.h"
#include "common/cappenv.hpp"

ResourceTypeForm::ResourceTypeForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResourceTypeForm)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);

    resize(geometry().size() * CAppEnv::m_dpiFactor);
}

ResourceTypeForm::~ResourceTypeForm()
{
    delete ui;
}

void ResourceTypeForm::on_buttonBox_accepted()
{
    if(ui->radioButton_sbox->isChecked())
    {
        m_type = "sbox";
    }
    else if(ui->radioButton_benes->isChecked())
    {
        m_type = "benes";
    }
    else if(ui->radioButton_fifo->isChecked())
    {
        m_type = "fifo";
    }
    else if(ui->radioButton_imd->isChecked())
    {
        m_type = "imd";
    }
    else if(ui->radioButton_memory->isChecked())
    {
        m_type = "memory";
    }

    m_save = ui->checkBox_save->isChecked();
}
