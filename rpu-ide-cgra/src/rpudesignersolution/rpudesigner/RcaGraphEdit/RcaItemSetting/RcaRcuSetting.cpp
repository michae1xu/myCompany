#include "RcaRcuSetting.h"
#include "ui_RcaRcuSetting.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>

RcaRcuSetting* RcaRcuSetting::rcuSetingInstance = NULL;

RcaRcuSetting::RcaRcuSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RcaRcuSetting)
//    , rcaScene(NULL)
    , moduleRcu(NULL)
{
    ui->setupUi(this);
    rcuSetingInstance = this;
    Qt::WindowFlags flags = windowFlags();
    setWindowFlags(flags | Qt::WindowStaysOnTopHint);
}

RcaRcuSetting::~RcaRcuSetting()
{
    delete ui;
}

//void RcaRcuSetting::setRcaScene(RcaGraphScene* scene)
//{
//    if(scene == NULL) return;
//    rcaScene = scene;
//}

void RcaRcuSetting::handleRcuBeDeleted(BaseItem *deletedItem)
{
    Q_UNUSED(deletedItem);
    disconnect(moduleRcu,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleRcuBeDeleted(BaseItem*)));
    moduleRcu = NULL;
    hide();
}


void RcaRcuSetting::showRcu(ModuleRcu* rcu, QPoint pos)
{
    QL_DEBUG;
    if(rcu == NULL) return;
    if(moduleRcu != rcu)
    {
        if(moduleRcu)
        {
            disconnect(moduleRcu,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleRcuBeDeleted(BaseItem*)));
        }
        moduleRcu = rcu;
        connect(moduleRcu,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleRcuBeDeleted(BaseItem*)));
    }
    QL_DEBUG;

    int indexRcu = moduleRcu->getIndexRcu() < RCU_MAX_COUNT ? moduleRcu->getIndexRcu() : 0;
    ui->comboBox_rcu_id->setCurrentIndex(indexRcu);

    int readMemMode128_32 = moduleRcu->readMemMode128_32 <= ui->comboBox_read_post->count() ? moduleRcu->readMemMode128_32 : 0;
    ui->comboBox_read_post->setCurrentIndex(readMemMode128_32);

    int readMemMode = moduleRcu->readMemMode <= ui->comboBox_read_mode->count() ? moduleRcu->readMemMode : 0;
    ui->comboBox_read_mode->setCurrentIndex(readMemMode);

    ui->spinBox_address1->setValue(moduleRcu->readMemAddr1);
    ui->spinBox_address2->setValue(moduleRcu->readMemAddr2);
    ui->spinBox_address3->setValue(moduleRcu->readMemAddr3);
    ui->spinBox_address4->setValue(moduleRcu->readMemAddr4);

    ui->spinBox_read_offset->setValue(moduleRcu->readMemOffset);
    ui->spinBox_read_rwTimes->setValue(moduleRcu->readMemThreashold);

    int writeMemMode = moduleRcu->writeMemMode <= ui->comboBox_write_mode->count() ? moduleRcu->writeMemMode : 0;
    ui->comboBox_write_mode->setCurrentIndex(writeMemMode);

    ui->spinBox_address->setValue(moduleRcu->writeMemAddr);

    int writeMemMask = moduleRcu->writeMemMask <= ui->comboBox_mask->count() ? moduleRcu->writeMemMask : 0;
    ui->comboBox_mask->setCurrentIndex(writeMemMask);

    ui->spinBox_write_offset->setValue(moduleRcu->writeMemOffset);
    ui->spinBox_write_rwTimes->setValue(moduleRcu->writeMemThreashold);
    on_comboBox_read_mode_currentIndexChanged(readMemMode);
    on_comboBox_write_mode_currentIndexChanged(writeMemMode);
    show();
    if(!pos.isNull())
    {
        move(pos);
    }
}

void RcaRcuSetting::on_buttonBox_clicked(QAbstractButton *button)
{
    if(moduleRcu == NULL) return;
    if(button == ui->buttonBox->button(QDialogButtonBox::Ok))
    {
        QL_DEBUG;

        int uiIndexRcu = ui->comboBox_rcu_id->currentIndex();
        if(moduleRcu->getIndexRcu() != uiIndexRcu)
        {
            emit rcuIndexChanged();
            moduleRcu->setIndexRcu(uiIndexRcu);
        }

        moduleRcu->readMemMode128_32 = ui->comboBox_read_post->currentIndex();
        moduleRcu->readMemMode = ui->comboBox_read_mode->currentIndex();
        moduleRcu->readMemAddr1 = ui->spinBox_address1->value();
        moduleRcu->readMemAddr2 = ui->spinBox_address2->value();
        moduleRcu->readMemAddr3 = ui->spinBox_address3->value();
        moduleRcu->readMemAddr4 = ui->spinBox_address4->value();
        moduleRcu->readMemOffset = ui->spinBox_read_offset->value();
        moduleRcu->readMemThreashold = ui->spinBox_read_rwTimes->value();

        moduleRcu->writeMemMode = ui->comboBox_write_mode->currentIndex();
        moduleRcu->writeMemAddr = ui->spinBox_address->value();
        moduleRcu->writeMemMask = ui->comboBox_mask->currentIndex();
        moduleRcu->writeMemOffset = ui->spinBox_write_offset->value();
        moduleRcu->writeMemThreashold = ui->spinBox_write_rwTimes->value();
        moduleRcu->sendChangedItemToScene();
    }
}

void RcaRcuSetting::on_comboBox_read_mode_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        ui->label_address1_6->setText(tr(u8"读写(阈值)："));
        ui->label_address1_6->setEnabled(false);
        ui->spinBox_read_rwTimes->setEnabled(false);
        ui->spinBox_read_rwTimes->setDisplayIntegerBase(16);
        break;
    case 1:
        ui->label_address1_6->setText(tr(u8"读写(阈值)："));
        ui->label_address1_6->setEnabled(false);
        ui->spinBox_read_rwTimes->setEnabled(false);
        ui->spinBox_read_rwTimes->setDisplayIntegerBase(16);
        break;
    case 2:
        ui->label_address1_6->setText(tr(u8"读写次数"));
        ui->label_address1_6->setEnabled(true);
        ui->spinBox_read_rwTimes->setEnabled(true);
        ui->spinBox_read_rwTimes->setDisplayIntegerBase(10);
        break;
    case 3:
        ui->label_address1_6->setText(tr(u8"边界地址"));
        ui->label_address1_6->setEnabled(true);
        ui->spinBox_read_rwTimes->setEnabled(true);
        ui->spinBox_read_rwTimes->setDisplayIntegerBase(16);
        break;
    default:
        break;
    }
}

void RcaRcuSetting::on_comboBox_write_mode_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        ui->label_address1_12->setText(tr(u8"读写(阈值)："));
        ui->label_address1_12->setEnabled(false);
        ui->spinBox_write_rwTimes->setEnabled(false);
        ui->spinBox_write_rwTimes->setDisplayIntegerBase(16);
        break;
    case 1:
        ui->label_address1_12->setText(tr(u8"读写(阈值)："));
        ui->label_address1_12->setEnabled(false);
        ui->spinBox_write_rwTimes->setEnabled(false);
        ui->spinBox_write_rwTimes->setDisplayIntegerBase(16);
        break;
    case 2:
        ui->label_address1_12->setText(tr(u8"读写次数"));
        ui->label_address1_12->setEnabled(true);
        ui->spinBox_write_rwTimes->setEnabled(true);
        ui->spinBox_write_rwTimes->setDisplayIntegerBase(10);
        break;
    case 3:
        ui->label_address1_12->setText(tr(u8"边界地址"));
        ui->label_address1_12->setEnabled(true);
        ui->spinBox_write_rwTimes->setEnabled(true);
        ui->spinBox_write_rwTimes->setDisplayIntegerBase(16);
        break;
    default:
        break;
    }
}
