#include "ItemDataForm.h"
#include "ui_ItemDataForm.h"
#include "common/debug.h"
#include <QRegExpValidator>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QRegExp>

const int EditTextMaxLength(8);
const QRegExp EditTextExp(QString("[0-9A-Fa-f]{%1}").arg(EditTextMaxLength));

ItemDataForm::ItemDataForm(ElementTypeId typeId,QStringList inputList,QStringList outputList,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ItemDataForm)
    ,inputCount(mapElementInput[typeId])
    ,outputCount(mapElementOutput[typeId])
    ,inputStrList(inputList)
    ,outputStrList(outputList)
{
    ui->setupUi(this);
    initEdit(typeId);
    ui->groupBox_input->setVisible(false);
}

ItemDataForm::~ItemDataForm()
{
    delete ui;
}

void ItemDataForm::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button == ui->buttonBox->button(QDialogButtonBox::Ok))
    {
        inputStrList.clear();
        if(ui->inputEdit_1->isVisible())
        {
            inputStrList.append(str2ItemData(ui->inputEdit_1->text()));
        }
        if(ui->inputEdit_2->isVisible())
        {
            inputStrList.append(str2ItemData(ui->inputEdit_2->text()));
        }
        if(ui->inputEdit_3->isVisible())
        {
            inputStrList.append(str2ItemData(ui->inputEdit_3->text()));
        }
        if(ui->inputEdit_4->isVisible())
        {
            inputStrList.append(str2ItemData(ui->inputEdit_4->text()));
        }

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


QStringList ItemDataForm::getInputStrList()
{
    return inputStrList;
}

QStringList ItemDataForm::getOutputStrList()
{
    return outputStrList;
}

void ItemDataForm::initEdit(ElementTypeId typeId)
{
    QValidator *validator = new QRegExpValidator(EditTextExp);
    ui->inputEdit_1->setValidator(validator);
    ui->inputEdit_2->setValidator(validator);
    ui->inputEdit_3->setValidator(validator);
    ui->inputEdit_4->setValidator(validator);

    ui->outputEdit_1->setValidator(validator);
    ui->outputEdit_2->setValidator(validator);
    ui->outputEdit_3->setValidator(validator);
    ui->outputEdit_4->setValidator(validator);

    if((inputStrList.size() >= inputCount) && (outputStrList.size() >= outputCount))
    {
        switch(typeId)
        {
        case DEBUG_BFU:
        {
            ui->inputLabel_1->setVisible(true);
            ui->inputLabel_1->setText(tr(u8"输入A"));
            ui->inputEdit_1->setVisible(true);
            ui->inputEdit_1->setText(inputStrList.at(0));

            ui->inputLabel_2->setVisible(true);
            ui->inputLabel_2->setText(tr(u8"输入B"));
            ui->inputEdit_2->setVisible(true);
            ui->inputEdit_2->setText(inputStrList.at(1));

            ui->inputLabel_3->setVisible(true);
            ui->inputLabel_3->setText(tr(u8"输入T"));
            ui->inputEdit_3->setVisible(true);
            ui->inputEdit_3->setText(inputStrList.at(2));

            ui->inputLabel_4->setVisible(false);
            ui->inputEdit_4->setVisible(false);

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
        case DEBUG_SBOX:
        {
            ui->inputLabel_1->setVisible(true);
            ui->inputLabel_1->setText(tr(u8"输入"));
            ui->inputEdit_1->setVisible(true);
            ui->inputEdit_1->setText(inputStrList.at(0));

            ui->inputLabel_2->setVisible(false);
            ui->inputEdit_2->setVisible(false);

            ui->inputLabel_3->setVisible(false);
            ui->inputEdit_3->setVisible(false);

            ui->inputLabel_4->setVisible(false);
            ui->inputEdit_4->setVisible(false);

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
        case DEBUG_BENES:
        {
            ui->inputLabel_1->setVisible(true);
            ui->inputLabel_1->setText(tr(u8"输入0"));
            ui->inputEdit_1->setVisible(true);
            ui->inputEdit_1->setText(inputStrList.at(0));

            ui->inputLabel_2->setVisible(true);
            ui->inputLabel_2->setText(tr(u8"输入1"));
            ui->inputEdit_2->setVisible(true);
            ui->inputEdit_2->setText(inputStrList.at(1));

            ui->inputLabel_3->setVisible(true);
            ui->inputLabel_3->setText(tr(u8"输入2"));
            ui->inputEdit_3->setVisible(true);
            ui->inputEdit_3->setText(inputStrList.at(2));

            ui->inputLabel_4->setVisible(true);
            ui->inputLabel_4->setText(tr(u8"输入3"));
            ui->inputEdit_4->setVisible(true);
            ui->inputEdit_4->setText(inputStrList.at(3));

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
    ui->inputLabel_1->setVisible(false);
    ui->inputEdit_1->setVisible(false);

    ui->inputLabel_2->setVisible(false);
    ui->inputEdit_2->setVisible(false);

    ui->inputLabel_3->setVisible(false);
    ui->inputEdit_3->setVisible(false);

    ui->inputLabel_4->setVisible(false);
    ui->inputEdit_4->setVisible(false);

    ui->outputLabel_1->setVisible(false);
    ui->outputEdit_1->setVisible(false);

    ui->outputLabel_2->setVisible(false);
    ui->outputEdit_2->setVisible(false);

    ui->outputLabel_3->setVisible(false);
    ui->outputEdit_3->setVisible(false);

    ui->outputLabel_4->setVisible(false);
    ui->outputEdit_4->setVisible(false);
}

QString ItemDataForm::str2ItemData(QString str)
{
    if(str.size() >= EditTextMaxLength) return str;
    QString result = str;
    for(int i=0;i<EditTextMaxLength - str.size();i++)
    {
        result.insert(0,"0");
    }
    return result;
}
