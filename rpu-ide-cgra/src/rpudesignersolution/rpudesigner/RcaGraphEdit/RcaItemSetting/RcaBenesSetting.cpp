#include "RcaBenesSetting.h"
#include "ui_RcaBenesSetting.h"

#include "fileresolver/xmlresolver.h"
#include "fileresolver/cprojectresolver.h"
#include "capppathenv.h"
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QFileIconProvider>

RcaBenesSetting* RcaBenesSetting::benesSetingInstance = NULL;

RcaBenesSetting::RcaBenesSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RcaBenesSetting)
//    ,rcaScene(NULL)
    , elementBenes(NULL)
    , isTrue(true)
{
    ui->setupUi(this);
    benesSetingInstance = this;
    Qt::WindowFlags flags = windowFlags();
    setWindowFlags(flags | Qt::WindowStaysOnTopHint);
}

RcaBenesSetting::~RcaBenesSetting()
{
    delete ui;
}

//void RcaBenesSetting::setRcaScene(RcaGraphScene* scene)
//{
//    if(scene == NULL) return;
//    rcaScene = scene;
//}

void RcaBenesSetting::resetResourceMap(RcaGraphScene* scene)
{
    if(scene == NULL) return;
    rcaScene = scene;
    if(scene->getProject())
    {
        ui->listWidget->clear();
        QMapIterator<QString, QString> i(scene->getProject()->getProjectParameter()->resourceMap);
        while(i.hasNext())
        {
            i.next();
            QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
            QString showName = i.key().mid(2);
            item->setText(showName);
            item->setData(Qt::UserRole, i.value());
            QFile file(i.value());
            QFileIconProvider fileIcon;
            QIcon icon = fileIcon.icon(file);
            item->setIcon(icon);
            item->setToolTip(i.value());
        }
        this->adjustSize();
        ui->widget->setVisible(true);
    }
    else
    {
        ui->listWidget->clear();
        this->adjustSize();
        ui->widget->setVisible(false);
    }
}

void RcaBenesSetting::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    XmlResolver* m_xml = qobject_cast<XmlResolver*>(rcaScene->xmlInView);
    QString txt = rcaScene->getProject()->getProjectParameter()->resourceMap.value(":/" + item->text());
    ui->lineEdit->setText(CAppPathEnv::absToRel(m_xml->m_curFile, txt));
}


void RcaBenesSetting::handleBenesBeDeleted(BaseItem* deletedItem)
{
    Q_UNUSED(deletedItem);
    disconnect(elementBenes,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleBenesBeDeleted(BaseItem*)));
    elementBenes = NULL;
    hide();
}

void RcaBenesSetting::showBenes(ElementBenes *benes, QPoint pos)
{
    QL_DEBUG;
    if(benes == NULL) return;
    if(elementBenes != benes)
    {
        if(elementBenes)
        {
            disconnect(elementBenes,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleBenesBeDeleted(BaseItem*)));
        }
        elementBenes = benes;
        connect(elementBenes,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleBenesBeDeleted(BaseItem*)));
    }
    QL_DEBUG;

    int indexInRcu = elementBenes->getIndexInRcu() < BENES_MAX_COUNT ? elementBenes->getIndexInRcu() : 0;
    ui->comboBox_benesId->setCurrentIndex(indexInRcu);

    InputPortType inputType0 = elementBenes->inPort0->getInputType() <= InputPort_Mem ? elementBenes->inPort0->getInputType() : InputPort_NULL;
    ui->comboBox_input0Type->setCurrentIndex(inputType0);

    setComboBox(ui->comboBox_input0Choice,inputType0);
    int inputPort0 = elementBenes->inPort0->getInputIndex() < ui->comboBox_input0Choice->count() ? elementBenes->inPort0->getInputIndex() : 0;
    ui->comboBox_input0Choice->setCurrentIndex(inputPort0);

    InputPortType inputType1 = elementBenes->inPort1->getInputType() <= InputPort_Mem ? elementBenes->inPort1->getInputType() : InputPort_NULL;
    ui->comboBox_input1Type->setCurrentIndex(inputType1);

    setComboBox(ui->comboBox_input1Choice,inputType1);
    int inputPort1 = elementBenes->inPort1->getInputIndex() < ui->comboBox_input1Choice->count() ? elementBenes->inPort1->getInputIndex() : 0;
    ui->comboBox_input1Choice->setCurrentIndex(inputPort1);

    InputPortType inputType2 = elementBenes->inPort2->getInputType() <= InputPort_Mem ? elementBenes->inPort2->getInputType() : InputPort_NULL;
    ui->comboBox_input2Type->setCurrentIndex(inputType2);

    setComboBox(ui->comboBox_input2Choice,inputType2);
    int inputPort2 = elementBenes->inPort2->getInputIndex() < ui->comboBox_input2Choice->count() ? elementBenes->inPort2->getInputIndex() : 0;
    ui->comboBox_input2Choice->setCurrentIndex(inputPort2);

    InputPortType inputType3 = elementBenes->inPort3->getInputType() <= InputPort_Mem ? elementBenes->inPort3->getInputType() : InputPort_NULL;
    ui->comboBox_input3Type->setCurrentIndex(inputType3);

    setComboBox(ui->comboBox_input3Choice,inputType3);
    int inputPort3 = elementBenes->inPort3->getInputIndex() < ui->comboBox_input3Choice->count() ? elementBenes->inPort3->getInputIndex() : 0;
    ui->comboBox_input3Choice->setCurrentIndex(inputPort3);

//    ui->pushButton_explorer->setText(elementBenes->srcConfig.isEmpty() ? tr(u8"浏览") : elementBenes->srcConfig);
    ui->lineEdit->setText(elementBenes->srcConfig);
    show();
    if(!pos.isNull())
    {
        move(pos);
    }
}


void RcaBenesSetting::setComboBox(QComboBox* box,InputPortType type)
{
    box->clear();
    switch(type)
    {
        case InputPort_CurrentBfuX:
        case InputPort_LastBfuX: setComboBoxBfuX(box);break;
        case InputPort_CurrentBfuY:
        case InputPort_LastBfuY: setComboBoxBfuY(box);break;
        case InputPort_CurrentSbox:
        case InputPort_LastSbox: setComboBoxSbox(box);break;
        case InputPort_CurrentBenes:
        case InputPort_LastBenes: setComboBoxBenes(box);break;
        case InputPort_InFifo: setComboBoxFifo(box);break;
        case InputPort_Mem:setComboBoxMem(box);break;
        case InputPort_NULL:
        default: break;
    }
}

void RcaBenesSetting::setComboBoxBfuX(QComboBox* box)
{
    box->clear();
    box->insertItem(0,"0");
    box->insertItem(1,"1");
    box->insertItem(2,"2");
    box->insertItem(3,"3");
    box->insertItem(4,"4");
    box->insertItem(5,"5");
    box->insertItem(6,"6");
    box->insertItem(7,"7");
}

void RcaBenesSetting::setComboBoxBfuY(QComboBox* box)
{
    box->clear();
    box->insertItem(0,"0");
    box->insertItem(1,"1");
    box->insertItem(2,"2");
    box->insertItem(3,"3");
    box->insertItem(4,"4");
    box->insertItem(5,"5");
    box->insertItem(6,"6");
    box->insertItem(7,"7");
}

void RcaBenesSetting::setComboBoxSbox(QComboBox* box)
{
    box->clear();
    box->insertItem(0,"0");
    box->insertItem(1,"1");
    box->insertItem(2,"2");
    box->insertItem(3,"3");
    box->insertItem(4,"4");
    box->insertItem(5,"5");
    box->insertItem(6,"6");
    box->insertItem(7,"7");
    box->insertItem(8,"8");
    box->insertItem(9,"9");
    box->insertItem(10,"10");
    box->insertItem(11,"11");
    box->insertItem(12,"12");
    box->insertItem(13,"13");
    box->insertItem(14,"14");
    box->insertItem(15,"15");
}

void RcaBenesSetting::setComboBoxBenes(QComboBox* box)
{
    box->clear();
    box->insertItem(0,"0");
    box->insertItem(1,"1");
    box->insertItem(2,"2");
    box->insertItem(3,"3");
}


void RcaBenesSetting::setComboBoxMem(QComboBox* box)
{
    box->clear();
    box->insertItem(0,"0");
    box->insertItem(1,"1");
    box->insertItem(2,"2");
    box->insertItem(3,"3");
}

void RcaBenesSetting::setComboBoxFifo(QComboBox* box)
{
    box->clear();
    box->insertItem(0,"0");
    box->insertItem(1,"1");
    box->insertItem(2,"2");
    box->insertItem(3,"3");
}

void RcaBenesSetting::on_pushButton_explorer_clicked()
{
    RcaGraphScene* rcaScene =NULL;
    if(elementBenes && elementBenes->scene())
    {
        rcaScene = dynamic_cast<RcaGraphScene*>(elementBenes->scene());
    }
    if(rcaScene && rcaScene->xmlInView && elementBenes)
    {
        XmlResolver* m_xml = qobject_cast<XmlResolver*>(rcaScene->xmlInView);
        if(m_xml)
        {
            QString filename = QFileDialog::getOpenFileName(
                        this, tr(u8"请选择数据文件路径"), QFileInfo(m_xml->m_curFile).absolutePath()
                        + elementBenes->srcConfig, tr(u8"BENES (*.benes);;文本文档 (*.txt);;* (*.*)"));
            if(!filename.trimmed().isEmpty())
            {
                ui->lineEdit->setText(filename.trimmed());
            }
        }
    }
}

void RcaBenesSetting::on_comboBox_input0Type_currentIndexChanged(int index)
{
    QL_DEBUG;
    setComboBox(ui->comboBox_input0Choice,InputPortType(index));
}


void RcaBenesSetting::on_comboBox_input1Type_currentIndexChanged(int index)
{
    QL_DEBUG;
    setComboBox(ui->comboBox_input1Choice,InputPortType(index));
}


void RcaBenesSetting::on_comboBox_input2Type_currentIndexChanged(int index)
{
    QL_DEBUG;
    setComboBox(ui->comboBox_input2Choice,InputPortType(index));
}


void RcaBenesSetting::on_comboBox_input3Type_currentIndexChanged(int index)
{
    QL_DEBUG;
    setComboBox(ui->comboBox_input3Choice,InputPortType(index));
}

void RcaBenesSetting::on_buttonBox_clicked(QAbstractButton *button)
{
    if(elementBenes == NULL) return;
    if(button == ui->buttonBox->button(QDialogButtonBox::Ok))
    {
        QL_DEBUG;

        if(!isTrue)
        {
            QMessageBox::information(this, tr(u8"提示"), tr(u8"存在无效参数"));
            return;
        }

        elementBenes->setIndexInRcu(ui->comboBox_benesId->currentIndex());

        InputPortType uiInput0Type = InputPortType(ui->comboBox_input0Type->currentIndex());
        int uiInput0Index = ui->comboBox_input0Choice->currentIndex();
        if((uiInput0Type != elementBenes->inPort0->getInputType())
                || uiInput0Index != elementBenes->inPort0->getInputIndex())
        {
            //重新找到port口进行连接，如果没有则提示。
            QL_DEBUG << uiInput0Type << uiInput0Index;
            if(elementBenes->inPort0->resetInputSignal(uiInput0Type,uiInput0Index) == false)
            {
                QMessageBox::information(this, tr(u8"提示"), tr(u8"不存在输入0选择的输入"));
            }
        }

        InputPortType uiInput1Type = InputPortType(ui->comboBox_input1Type->currentIndex());
        int uiInput1Index = ui->comboBox_input1Choice->currentIndex();
        if((uiInput1Type != elementBenes->inPort1->getInputType())
                || uiInput1Index != elementBenes->inPort1->getInputIndex())
        {
            //重新找到port口进行连接，如果没有则提示。
            QL_DEBUG << uiInput1Type << uiInput1Index;
            if(elementBenes->inPort1->resetInputSignal(uiInput1Type,uiInput1Index) == false)
            {
                QMessageBox::information(this, tr(u8"提示"), tr(u8"不存在输入1选择的输入"));
            }
        }
        InputPortType uiInput2Type = InputPortType(ui->comboBox_input2Type->currentIndex());
        int uiInput2Index = ui->comboBox_input2Choice->currentIndex();
        if((uiInput2Type != elementBenes->inPort2->getInputType())
                || uiInput2Index != elementBenes->inPort2->getInputIndex())
        {
            //重新找到port口进行连接，如果没有则提示。
            QL_DEBUG << uiInput2Type << uiInput2Index;
            if(elementBenes->inPort2->resetInputSignal(uiInput2Type,uiInput2Index) == false)
            {
                QMessageBox::information(this, tr(u8"提示"), tr(u8"不存在输入2选择的输入"));
            }
        }
        InputPortType uiInput3Type = InputPortType(ui->comboBox_input3Type->currentIndex());
        int uiInput3Index = ui->comboBox_input3Choice->currentIndex();
        if((uiInput3Type != elementBenes->inPort3->getInputType())
                || uiInput3Index != elementBenes->inPort3->getInputIndex())
        {
            //重新找到port口进行连接，如果没有则提示。
            QL_DEBUG << uiInput3Type << uiInput3Index;
            if(elementBenes->inPort3->resetInputSignal(uiInput3Type,uiInput3Index) == false)
            {
                QMessageBox::information(this, tr(u8"提示"), tr(u8"不存在输入3选择的输入"));
            }
        }
//        elementBenes->srcConfig = ui->pushButton_explorer->text() == tr(u8"浏览") ? "" : ui->pushButton_explorer->text();
        elementBenes->srcConfig = ui->lineEdit->text().trimmed();
        elementBenes->sendChangedItemToScene();
        accept();
    }
}

void RcaBenesSetting::on_lineEdit_textChanged(const QString &arg1)
{
    RcaGraphScene* rcaScene =NULL;
    if(elementBenes && elementBenes->scene())
    {
        rcaScene = dynamic_cast<RcaGraphScene*>(elementBenes->scene());
    }
    if(rcaScene && rcaScene->xmlInView && elementBenes)
    {
        XmlResolver* m_xml = qobject_cast<XmlResolver*>(rcaScene->xmlInView);
        if(m_xml)
        {
            QDir temp_d(m_xml->m_curFile);
            QString temp_dir = temp_d.absolutePath();
            temp_dir = temp_dir.left(temp_dir.lastIndexOf("/")+1);
            QL_DEBUG << temp_dir + arg1.trimmed();

            if((arg1.trimmed().isEmpty()) || (QFile(temp_dir + arg1.trimmed()).exists()))
            {
                QL_DEBUG;
                QPalette palette;
                palette.setColor(QPalette::Text, Qt::black);
                ui->lineEdit->setPalette(palette);
                isTrue = true;
            }
            else
            {
                QL_DEBUG;
                QPalette palette;
                palette.setColor(QPalette::Text, Qt::red);
                ui->lineEdit->setPalette(palette);
                isTrue = false;
            }
        }
    }
}

