#include <QFile>
#include <QTextStream>
#include "csetfifoandgprfform.h"
#include "ui_csetfifoandgprfform.h"

CSetFifoAndGprfForm::CSetFifoAndGprfForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CSetFifoAndGprfForm)
{
    ui->setupUi(this);

    for(int i = 0; i < 5; ++i)
    {
        m_memorySet[i].size =  0;
        m_memorySet[i].data = NULL;
    }
    for(int i = 0; i < 4; ++i)
    {
        m_fifoSet[i].size = 0;
        m_fifoSet[i].data = NULL;
    }
}

CSetFifoAndGprfForm::~CSetFifoAndGprfForm()
{
    for(int i = 0; i < 5; ++i)
    {
        if(m_memorySet[i].size)
            delete[] m_memorySet[i].data;
    }
    for(int i = 0; i < 4; ++i)
    {
        if(m_fifoSet[i].size)
            delete[] m_fifoSet[i].data;
    }
    delete ui;
}

SMemorySet *CSetFifoAndGprfForm::getMemorySet()
{
    return m_memorySet;
}

SFifoSet *CSetFifoAndGprfForm::getFifoSet()
{
    return m_fifoSet;
}

void CSetFifoAndGprfForm::on_buttonBox_accepted()
{
    m_memorySet[0].enable = ui->pushButton_memory1->isChecked();
    m_memorySet[1].enable = ui->pushButton_memory2->isChecked();
    m_memorySet[2].enable = ui->pushButton_memory3->isChecked();
    m_memorySet[3].enable = ui->pushButton_memory4->isChecked();
    m_memorySet[4].enable = ui->pushButton_memoryGlobal->isChecked();

    m_memorySet[0].offset = ui->spinBox_memory1->value();
    m_memorySet[1].offset = ui->spinBox_memory2->value();
    m_memorySet[2].offset = ui->spinBox_memory3->value();
    m_memorySet[3].offset = ui->spinBox_memory4->value();
    m_memorySet[4].offset = ui->spinBox_memoryGlobal->value();

    readData(m_memorySet[0].data, m_memorySet[0].size, ui->lineEdit_memory1->text());
    readData(m_memorySet[1].data, m_memorySet[1].size, ui->lineEdit_memory2->text());
    readData(m_memorySet[2].data, m_memorySet[2].size, ui->lineEdit_memory3->text());
    readData(m_memorySet[3].data, m_memorySet[3].size, ui->lineEdit_memory4->text());
    readData(m_memorySet[4].data, m_memorySet[4].size, ui->lineEdit_memoryGlobal->text());

    m_fifoSet[0].enable = ui->pushButton_inputFifo1->isChecked();
    m_fifoSet[1].enable = ui->pushButton_inputFifo2->isChecked();
    m_fifoSet[2].enable = ui->pushButton_inputFifo3->isChecked();
    m_fifoSet[3].enable = ui->pushButton_inputFifo4->isChecked();

    readData(m_fifoSet[0].data, m_fifoSet[0].size, ui->lineEdit_inputFifo1->text());
    readData(m_fifoSet[1].data, m_fifoSet[1].size, ui->lineEdit_inputFifo2->text());
    readData(m_fifoSet[2].data, m_fifoSet[2].size, ui->lineEdit_inputFifo3->text());
    readData(m_fifoSet[3].data, m_fifoSet[3].size, ui->lineEdit_inputFifo4->text());
}

bool CSetFifoAndGprfForm::readData(uint *&data , uint &size, QString fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    QTextStream in(&file);
    QStringList strList = in.readAll().trimmed().split("\r\n");
    size = (uint)strList.count();
    data = new uint[strList.count()];
    for (int i = 0; i < strList.count(); ++i)
    {
        bool ok;
        data[i] = QString(strList.at(i)).toUInt(&ok, 16);
        if(!ok)
            return false;
    }
    file.close();
    return true;
}
