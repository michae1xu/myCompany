#include "DebugModifyData.h"
#include "ui_DebugModifyData.h"
#include <QRegExpValidator>
#include <QPushButton>

const int EditTextMaxLength(8);
const QRegExp EditTextExp(QString("[0-9A-Fa-f]{%1}").arg(EditTextMaxLength));

DebugModifyData::DebugModifyData(ElementTypeId_del typeId,QStringList outputList,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebugModifyData)
  , outputCount(mapElementOutput_del[typeId])
  , outputStrList(outputList)
{
    ui->setupUi(this);
    initEdit(typeId);
}

DebugModifyData::~DebugModifyData()
{
    delete ui;
}

void DebugModifyData::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button == ui->buttonBox->button(QDialogButtonBox::Ok))
    {
        outputStrList.clear();
        if(ui->outputEdit_1->isVisible())
        {
            outputStrList.append(str2ItemData(ui->outputEdit_1->text()));
        }
        if(ui->outputEdit_2->isVisible())
        {
            outputStrList.append(str2ItemData(ui->outputEdit_2->text()));
        }
        if(ui->outputEdit_3->isVisible())
        {
            outputStrList.append(str2ItemData(ui->outputEdit_3->text()));
        }
        if(ui->outputEdit_4->isVisible())
        {
            outputStrList.append(str2ItemData(ui->outputEdit_4->text()));
        }
        this->accept();
    }
    else
    {
        this->reject();
    }
}

QStringList DebugModifyData::getOutputStrList() const
{
    return outputStrList;
}

void DebugModifyData::initEdit(ElementTypeId_del typeId)
{
    QValidator *validator = new QRegExpValidator(EditTextExp);
    ui->outputEdit_1->setValidator(validator);
    ui->outputEdit_2->setValidator(validator);
    ui->outputEdit_3->setValidator(validator);
    ui->outputEdit_4->setValidator(validator);

    if(outputStrList.size() >= outputCount)
    {
        switch(typeId)
        {
        case DEBUG_BFU_del:
        {
            ui->outputLabel_1->setVisible(true);
            ui->outputLabel_1->setText(tr(u8"输出X"));
            ui->outputEdit_1->setVisible(true);
            ui->outputEdit_1->setText(outputStrList.at(0));

            ui->outputLabel_2->setVisible(true);
            ui->outputLabel_2->setText(tr(u8"输出Y"));
            ui->outputEdit_2->setVisible(true);
            ui->outputEdit_2->setText(outputStrList.at(1));

            ui->outputLabel_3->setVisible(false);
            ui->outputEdit_3->setVisible(false);

            ui->outputLabel_4->setVisible(false);
            ui->outputEdit_4->setVisible(false);
            return;
        }
        case DEBUG_SBOX_del:
        {
            ui->outputLabel_1->setVisible(true);
            ui->outputLabel_1->setText(tr(u8"输出0"));
            ui->outputEdit_1->setVisible(true);
            ui->outputEdit_1->setText(outputStrList.at(0));

            ui->outputLabel_2->setVisible(true);
            ui->outputLabel_2->setText(tr(u8"输出1"));
            ui->outputEdit_2->setVisible(true);
            ui->outputEdit_2->setText(outputStrList.at(1));

            ui->outputLabel_3->setVisible(true);
            ui->outputLabel_3->setText(tr(u8"输出2"));
            ui->outputEdit_3->setVisible(true);
            ui->outputEdit_3->setText(outputStrList.at(2));

            ui->outputLabel_4->setVisible(true);
            ui->outputLabel_4->setText(tr(u8"输出3"));
            ui->outputEdit_4->setVisible(true);
            ui->outputEdit_4->setText(outputStrList.at(3));
            return;
        }
        case DEBUG_BENES_del:
        {
            ui->outputLabel_1->setVisible(true);
            ui->outputLabel_1->setText(tr(u8"输出0"));
            ui->outputEdit_1->setVisible(true);
            ui->outputEdit_1->setText(outputStrList.at(0));

            ui->outputLabel_2->setVisible(true);
            ui->outputLabel_2->setText(tr(u8"输出1"));
            ui->outputEdit_2->setVisible(true);
            ui->outputEdit_2->setText(outputStrList.at(1));

            ui->outputLabel_3->setVisible(true);
            ui->outputLabel_3->setText(tr(u8"输出2"));
            ui->outputEdit_3->setVisible(true);
            ui->outputEdit_3->setText(outputStrList.at(2));

            ui->outputLabel_4->setVisible(true);
            ui->outputLabel_4->setText(tr(u8"输出3"));
            ui->outputEdit_4->setVisible(true);
            ui->outputEdit_4->setText(outputStrList.at(3));
            return;
        }
        default:break;
        }
    }

    ui->outputLabel_1->setVisible(false);
    ui->outputEdit_1->setVisible(false);

    ui->outputLabel_2->setVisible(false);
    ui->outputEdit_2->setVisible(false);

    ui->outputLabel_3->setVisible(false);
    ui->outputEdit_3->setVisible(false);

    ui->outputLabel_4->setVisible(false);
    ui->outputEdit_4->setVisible(false);
}

QString DebugModifyData::str2ItemData(QString str)
{
    if(str.size() >= EditTextMaxLength) return str;
    QString result = str;
    for(int i=0;i<EditTextMaxLength - str.size();i++)
    {
        result.insert(0,"0");
    }
    return result;
}
