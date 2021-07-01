#include "RcaRWMemSetting.h"
#include "ui_RcaRWMemSetting.h"

#include "ModuleReadMem.h"
#include "ModuleWriteMem.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>

RcaRWMemSetting* RcaRWMemSetting::rwMemSetingInstance = NULL;

RcaRWMemSetting::RcaRWMemSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RcaRWMemSetting)
//    ,rcaScene(NULL)
    , rwMem(NULL)
{
    ui->setupUi(this);
    rwMemSetingInstance = this;
    Qt::WindowFlags flags = windowFlags();
    setWindowFlags(flags | Qt::WindowStaysOnTopHint);
}

RcaRWMemSetting::~RcaRWMemSetting()
{
    delete ui;
}

//void RcaRWMemSetting::setRcaScene(RcaGraphScene* scene)
//{
//    if(scene == NULL) return;
//    rcaScene = scene;
//}

void RcaRWMemSetting::handleRWMemBeDeleted(BaseItem* deletedItem)
{
    Q_UNUSED(deletedItem);
    disconnect(rwMem,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleBcuBeDeleted(BaseItem*)));
    rwMem = NULL;
    hide();
}

void RcaRWMemSetting::showRWMem(BasePort* mem, QPoint pos)
{
    QL_DEBUG;
    if(mem == NULL) return;
    if(rwMem != mem)
    {
        if(rwMem)
        {
            disconnect(rwMem,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleBcuBeDeleted(BaseItem*)));
        }
        rwMem = mem;
        connect(rwMem,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleBcuBeDeleted(BaseItem*)));
    }
    ModuleReadMem* readMem = dynamic_cast<ModuleReadMem*>(rwMem);
    ModuleWriteMem* writeMem = dynamic_cast<ModuleWriteMem*>(rwMem);

    if(readMem)
    {
        ui->comboBox_memId->setCurrentIndex(readMem->getReadMemIndex());
    }

    else if(writeMem)
    {
        ui->comboBox_memId->setCurrentIndex(writeMem->getWriteMemIndex());
    }
    show();
    if(!pos.isNull())
    {
        move(pos);
    }
    QL_DEBUG;
}

void RcaRWMemSetting::on_buttonBox_clicked(QAbstractButton *button)
{
    if(rwMem == NULL) return;
    if(button == ui->buttonBox->button(QDialogButtonBox::Ok))
    {
        QL_DEBUG;
        ModuleReadMem* readMem = dynamic_cast<ModuleReadMem*>(rwMem);
        ModuleWriteMem* writeMem = dynamic_cast<ModuleWriteMem*>(rwMem);
        if(readMem)
        {
            readMem->setReadMemIndex(ui->comboBox_memId->currentIndex());
        }
        else if(writeMem)
        {
            writeMem->setWriteMemIndex(ui->comboBox_memId->currentIndex());
        }
        rwMem->sendChangedItemToScene();
    }

}
