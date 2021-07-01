#include "RcaSboxSetting.h"
#include "ui_RcaSboxSetting.h"

#include "fileresolver/xmlresolver.h"
#include "fileresolver/cprojectresolver.h"
#include "capppathenv.h"
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QFileIconProvider>

RcaSboxSetting* RcaSboxSetting::sboxSetingInstance = NULL;

RcaSboxSetting::RcaSboxSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RcaSboxSetting)
//    , rcaScene(NULL)
    , elementSbox(NULL)
    , isTrue(true)
{
    ui->setupUi(this);
    sboxSetingInstance = this;
    Qt::WindowFlags flags = windowFlags();
    setWindowFlags(flags | Qt::WindowStaysOnTopHint);
}

RcaSboxSetting::~RcaSboxSetting()
{
    delete ui;
}

void RcaSboxSetting::resetResourceMap(RcaGraphScene* scene)
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

void RcaSboxSetting::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    XmlResolver* m_xml = qobject_cast<XmlResolver*>(rcaScene->xmlInView);
    QString txt = rcaScene->getProject()->getProjectParameter()->resourceMap.value(":/" + item->text());
    ui->lineEdit->setText(CAppPathEnv::absToRel(m_xml->m_curFile, txt));
}


//void RcaSboxSetting::setRcaScene(RcaGraphScene* scene)
//{
//    if(scene == NULL) return;
//    rcaScene = scene;
//    if(rcaScene->getProject())
//    {
//        ui->listWidget->clear();
//        QMapIterator<QString, QString> i(rcaScene->getProject()->getProjectParameter()->resourceMap);
//        while(i.hasNext())
//        {
//            i.next();
//            QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
//            QString showName = i.key().mid(2);
//            item->setText(showName);
//            item->setData(Qt::UserRole, i.value());
//            QFile file(i.value());
//            QFileIconProvider fileIcon;
//            QIcon icon = fileIcon.icon(file);
//            item->setIcon(icon);
//            item->setToolTip(i.value());
//        }
//    }
//    else
//    {
//        ui->listWidget->setVisible(false);
//    }
//}

void RcaSboxSetting::handleSboxBeDeleted(BaseItem* deletedItem)
{
    Q_UNUSED(deletedItem);
    disconnect(elementSbox,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleSboxBeDeleted(BaseItem*)));
    elementSbox = NULL;
    hide();
}

void RcaSboxSetting::showSbox(ElementSbox *sbox, QPoint pos)
{
    QL_DEBUG;
    if(sbox == NULL) return;
    if(elementSbox != sbox)
    {
        if(elementSbox)
        {
            disconnect(elementSbox,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleSboxBeDeleted(BaseItem*)));
        }
        elementSbox = sbox;
        connect(elementSbox,SIGNAL(itemBeDeleted(BaseItem*)),this,SLOT(handleSboxBeDeleted(BaseItem*)));
    }
    QL_DEBUG;

    int indexInRcu = elementSbox->getIndexInRcu() < SBOX_MAX_COUNT ? elementSbox->getIndexInRcu() : 0;
    ui->comboBox_sboxId->setCurrentIndex(indexInRcu);

    InputPortType inputType0 = elementSbox->inPort0->getInputType() <= InputPort_Mem ? elementSbox->inPort0->getInputType() : InputPort_NULL;
    ui->comboBox_input0Type->setCurrentIndex(inputType0);

    setComboBox(ui->comboBox_input0Choice,inputType0);
    int inputPort0 = elementSbox->inPort0->getInputIndex() < ui->comboBox_input0Choice->count() ? elementSbox->inPort0->getInputIndex() : 0;
    ui->comboBox_input0Choice->setCurrentIndex(inputPort0);

    ui->comboBox_group0->setCurrentIndex(elementSbox->groupCnt0);
    ui->comboBox_group1->setCurrentIndex(elementSbox->groupCnt1);
    ui->comboBox_group2->setCurrentIndex(elementSbox->groupCnt2);
    ui->comboBox_group3->setCurrentIndex(elementSbox->groupCnt3);

    ui->comboBox_mode->setCurrentIndex(elementSbox->sboxMode);
//    ui->pushButton_explorer->setText(elementSbox->srcConfig.isEmpty() ? tr(u8"浏览") : elementSbox->srcConfig);
    ui->lineEdit->setText(elementSbox->srcConfig);
    show();
    on_lineEdit_textChanged(elementSbox->srcConfig);
    if(!pos.isNull())
    {
        move(pos);
    }
}

void RcaSboxSetting::setComboBox(QComboBox* box,InputPortType type)
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

void RcaSboxSetting::setComboBoxBfuX(QComboBox* box)
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

void RcaSboxSetting::setComboBoxBfuY(QComboBox* box)
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

void RcaSboxSetting::setComboBoxSbox(QComboBox* box)
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

void RcaSboxSetting::setComboBoxBenes(QComboBox* box)
{
    box->clear();
    box->insertItem(0,"0");
    box->insertItem(1,"1");
    box->insertItem(2,"2");
    box->insertItem(3,"3");
}


void RcaSboxSetting::setComboBoxMem(QComboBox* box)
{
    box->clear();
    box->insertItem(0,"0");
    box->insertItem(1,"1");
    box->insertItem(2,"2");
    box->insertItem(3,"3");
}

void RcaSboxSetting::setComboBoxFifo(QComboBox* box)
{
    box->clear();
    box->insertItem(0,"0");
    box->insertItem(1,"1");
    box->insertItem(2,"2");
    box->insertItem(3,"3");
}

void RcaSboxSetting::on_comboBox_input0Type_currentIndexChanged(int index)
{
    QL_DEBUG;
    setComboBox(ui->comboBox_input0Choice,InputPortType(index));
}

void RcaSboxSetting::on_buttonBox_clicked(QAbstractButton *button)
{
    if(elementSbox == NULL) return;
    if(button == ui->buttonBox->button(QDialogButtonBox::Ok))
    {
        QL_DEBUG;

        if(!isTrue)
        {
            QMessageBox::information(this, tr(u8"提示"), tr(u8"存在无效参数"));
            return;
        }

        elementSbox->setIndexInRcu(ui->comboBox_sboxId->currentIndex());

        InputPortType uiInput0Type = InputPortType(ui->comboBox_input0Type->currentIndex());
        int uiInput0Index = ui->comboBox_input0Choice->currentIndex();
        if((uiInput0Type != elementSbox->inPort0->getInputType())
                || uiInput0Index != elementSbox->inPort0->getInputIndex())
        {
            //重新找到port口进行连接，如果没有则提示。
            QL_DEBUG << uiInput0Type << uiInput0Index;
            if(elementSbox->inPort0->resetInputSignal(uiInput0Type,uiInput0Index) == false)
            {
                QMessageBox::information(this, tr(u8"提示"), tr(u8"不存在输入0选择的输入"));
            }
        }
        elementSbox->groupCnt0 = SboxGroupValue(ui->comboBox_group0->currentIndex());
        elementSbox->groupCnt1 = SboxGroupValue(ui->comboBox_group1->currentIndex());
        elementSbox->groupCnt2 = SboxGroupValue(ui->comboBox_group2->currentIndex());
        elementSbox->groupCnt3 = SboxGroupValue(ui->comboBox_group3->currentIndex());

        elementSbox->sboxMode = ui->comboBox_mode->currentIndex();

//        elementSbox->srcConfig = ui->pushButton_explorer->text() == tr(u8"浏览") ? "" : ui->pushButton_explorer->text();
        elementSbox->srcConfig = ui->lineEdit->text().trimmed();
        elementSbox->sendChangedItemToScene();
        accept();
    }
}

void RcaSboxSetting::on_pushButton_explorer_clicked()
{
    RcaGraphScene* rcaScene =NULL;
    if(elementSbox && elementSbox->scene())
    {
        rcaScene = dynamic_cast<RcaGraphScene*>(elementSbox->scene());
    }
    if(rcaScene && rcaScene->xmlInView && elementSbox)
    {
        XmlResolver* m_xml = qobject_cast<XmlResolver*>(rcaScene->xmlInView);
        if(m_xml)
        {
            QString filename = QFileDialog::getOpenFileName(
                        this, tr(u8"请选择数据文件路径"), QFileInfo(m_xml->m_curFile).absolutePath()
                        + elementSbox->srcConfig, tr(u8"SBOX (*.sbox);;文本文档 (*.txt);;* (*.*)"));
            if(!filename.trimmed().isEmpty())
            {
                ui->lineEdit->setText(filename.trimmed());
            }
        }
    }
}

void RcaSboxSetting::on_lineEdit_textChanged(const QString &arg1)
{
    RcaGraphScene* rcaScene =NULL;
    if(elementSbox && elementSbox->scene())
    {
        rcaScene = dynamic_cast<RcaGraphScene*>(elementSbox->scene());
    }
    if(rcaScene && rcaScene->xmlInView && elementSbox)
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

