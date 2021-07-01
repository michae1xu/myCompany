/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    resultform.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include "resultform.h"
#include "ui_resultform.h"
#include "capppathenv.h"
#include <QFileInfo>
#include <QTextStream>
#include <QTextBrowser>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>
#include <QMessageBox>

/**
 * ResultForm::ResultForm
 * \brief   constructor of ResultForm
 * \param   paraInfoList
 * \param   outfifo
 * \param   memory
 * \param   parent
 * \author  zhangjun
 * \date    2016-10-12
 */
ResultForm::ResultForm(QList<SXmlParameter> *xmlParameterList, QStringList outfifoList, QStringList memoryList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResultForm),
    m_xmlParameterList(xmlParameterList),
    m_outfifoList(outfifoList),
    m_memoryList(memoryList)
{
    ui->setupUi(this);

//    ui->textBrowser->setVisible(false);

    ui->treeWidget->setColumnCount(2); //设置列数
    QStringList strList;
    strList << tr(u8"名称") << tr(u8"说明");
    ui->treeWidget->setHeaderLabels(strList);
//    QFont font = ui->textBrowser->font();
//    font.setPointSize(14);
//    ui->textBrowser->setFont(font);
    for(int i = 0; i < m_xmlParameterList->count(); ++i)
    {
        SXmlParameter xmlParameter = m_xmlParameterList->at(i);
        if(xmlParameter.selected ==  Qt::Unchecked)
            continue;

        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
        item->setIcon(0, QIcon(CAppPathEnv::getImageDirPath() + "/128/Xml-tool128.png"));
        item->setText(0, CAppPathEnv::stpToNm(xmlParameter.xmlPath));
        item->setText(1, tr(u8"XML文件"));

        QTreeWidgetItem *fifoOut = new QTreeWidgetItem(item);
        fifoOut->setText(0, tr(u8"FIFO0"));
        fifoOut->setText(1, tr(u8"FIFO输出"));
        fifoOut = new QTreeWidgetItem(item);
        fifoOut->setText(0, tr(u8"FIFO1"));
        fifoOut->setText(1, tr(u8"FIFO输出"));
        fifoOut = new QTreeWidgetItem(item);
        fifoOut->setText(0, tr(u8"FIFO2"));
        fifoOut->setText(1, tr(u8"FIFO输出"));
        fifoOut = new QTreeWidgetItem(item);
        fifoOut->setText(0, tr(u8"FIFO3"));
        fifoOut->setText(1, tr(u8"FIFO输出"));

        QTreeWidgetItem *memOut = new QTreeWidgetItem(item);
        memOut->setText(0, tr(u8"Memory0"));
        memOut->setText(1, tr(u8"Memory输出"));
        memOut = new QTreeWidgetItem(item);
        memOut->setText(0, tr(u8"Memory1"));
        memOut->setText(1, tr(u8"Memory输出"));
        memOut = new QTreeWidgetItem(item);
        memOut->setText(0, tr(u8"Memory2"));
        memOut->setText(1, tr(u8"Memory输出"));
        memOut = new QTreeWidgetItem(item);
        memOut->setText(0, tr(u8"Memory3"));
        memOut->setText(1, tr(u8"Memory输出"));
        memOut = new QTreeWidgetItem(item);
        memOut->setText(0, tr(u8"Memory4"));
        memOut->setText(1, tr(u8"Memory输出"));
    }

    ui->treeWidget->resizeColumnToContents(0);
    ui->treeWidget->resizeColumnToContents(1);

//    QAction *action = new QAction(this);
//    action->setText(tr(u8"保存..."));
//    action->setIcon(QIcon(":images/saveas.png"));
//    connect(action, SIGNAL(triggered()), this, SLOT(save()));
//    ui->textBrowser->addAction(action);
//    action = new QAction(this);
//    action->setText(tr(u8"复制"));
//    action->setIcon(QIcon(":images/copy.png"));
//    connect(action, SIGNAL(triggered()), ui->textBrowser, SLOT(copy()));
//    ui->textBrowser->addAction(action);
//    action = new QAction(this);
//    action->setText(tr(u8"全选"));
//    action->setIcon(QIcon(":images/select.png"));
//    connect(action, SIGNAL(triggered()), ui->textBrowser, SLOT(selectAll()));
//    ui->textBrowser->addAction(action);

//    ui->textBrowser->setContextMenuPolicy(Qt::ActionsContextMenu);

}

/**
 * ResultForm::~ResultForm
 * \brief   destructor of ResultForm
 * \author  zhangjun
 * \date    2016-10-12
 */
ResultForm::~ResultForm()
{
    delete ui;
    qDebug() << "~ResultForm()";
}

/**
 * ResultForm::stpToNm
 * \brief   
 * \param   fullFileName
 * \return  QString
 * \author  zhangjun
 * \date    2016-10-12
 */
//QString ResultForm::stpToNm(const QString &fullFileName)
//{
//    return QFileInfo(fullFileName).fileName();
//}

/**
 * ResultForm::showText
 * \brief   
 * \param   row
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void ResultForm::showText(int row, int index)
{
//    m_file.setFileName(fileName);
//    if(!m_file.open(QIODevice::ReadOnly))
//    {
//        ui->textBrowser->setText(tr(u8"打开文件“%1”失败！").arg(fileName));
//        return;
//    }

//    QTextStream in(&m_file);
//    QString str = in.readAll();
//    if(str.isEmpty())
//        ui->textBrowser->setText(tr(u8"没有内容输出！"));
//    else
//        ui->textBrowser->setText(str);
//    m_file.close();
    if(index < 4 && index >=0)
        showFifo(row, index);
    else if(index < 9 && index >=4)
        showMem(row ,index - 4);

}

/**
 * ResultForm::showFifo
 * \brief   
 * \param   row
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void ResultForm::showFifo(int row, int index)
{
//    ui->tableWidget->clear();
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    QStringList strList = QString(m_outfifoList.at(row)).split("\r\n");
    if(strList.count() < 1)
        return;
    strList.removeAt(0);

    foreach (QString line, strList)
    {
        QStringList list = line.split(" ");
        if(list.count() != 2)
            return;
        if(QString(list.at(0).at(list.at(0).length() - 1)).toInt() == index)
        {
            QStringList cellList = list.at(1).split(",");
            if(cellList.count() != 4)
                return;

            int row = ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(row);
            for(int i = 0; i < cellList.count(); ++i)
            {
                QTableWidgetItem *item = new QTableWidgetItem();
                item->setText(cellList.at(i));
                item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
                ui->tableWidget->setItem(row, i, item);
            }
        }
    }
}

/**
 * ResultForm::showMem
 * \brief   
 * \param   row
 * \param   index
 * \author  zhangjun
 * \date    2016-10-12
 */
void ResultForm::showMem(int row, int index)
{
//    ui->tableWidget->clear();
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    QStringList strList = QString(m_memoryList.at(row)).split("\r\n");
    if(strList.count() < 1)
        return;
    strList.removeAt(0);

    bool start = false;
    foreach (QString line, strList)
    {
        if(line.startsWith("MEM"))
        {
            if(QString(line.at(line.length() - 1)).toInt() == index)
            {
                start = true;
                continue;
            }
            else
            {
                start = false;
            }
        }
        if(!start)
            continue;

        QStringList list = line.split(":");
        if(list.count() != 2)
            return;

        QStringList cellList = list.at(1).split(",");
        if(cellList.count() != 4)
            return;

        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setText(list.at(0));
        ui->tableWidget->setVerticalHeaderItem(row, item);
        for(int i = 0; i < cellList.count(); ++i)
        {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setText(cellList.at(i));
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget->setItem(row, i, item);
        }
    }

}

/**
 * ResultForm::on_treeWidget_itemClicked
 * \brief   
 * \param   item
 * \param   column
 * \author  zhangjun
 * \date    2016-10-12
 */
void ResultForm::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    QTreeWidgetItem *parent = item->parent();
    if(parent == NULL)
        return;

    int row = -1;
    for(int i = 0; i < m_xmlParameterList->count(); ++i)
    {
        qDebug() << CAppPathEnv::stpToNm(m_xmlParameterList->at(i).xmlPath) << parent->text(0);
        if(CAppPathEnv::stpToNm(m_xmlParameterList->at(i).xmlPath) == parent->text(0))
        {
            row = i;
            break;
        }
    }
    if(row == -1)
        return;

    int at = parent->indexOfChild(item); //item在父项中的节点行号(从0开始)
    showText(row, at);

//    switch (at) {
//    case 0:
//        showText(0);
//        break;
//    case 1:
//        showText(1);
//        break;
//    case 2:
//        showText(2);
//        break;
//    case 3:
//        showText(3);
//        break;
//    case 4:
//        showText(4);
//        break;
//    case 5:
//        showText(5);
//        break;
//    case 6:
//        showText(6);
//        break;
//    case 7:
//        showText(7);
//        break;
//    case 8:
//        showText(8);
//        break;
//    default:
//        break;
//    }
}

/**
 * ResultForm::save
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void ResultForm::save()
{
//    QString fileName = QFileDialog::getSaveFileName(this, tr(u8"保存"), QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), tr(u8"txt 文件 (*.txt)"));
//    if (fileName.isEmpty())
//        return;

//    QFile file(fileName);
//    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
//    {
//        QMessageBox::critical(this, tr(u8"错误"), tr(u8"保存到文件失败！"));
//        return;
//    }

//    QTextStream out(&file);


//    file.close();



}

