/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    createcommandwordform.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include "createcommandwordform.h"
#include "ui_createcommandwordform.h"
#include "cmdword/ccreatecommandwordmodel.h"
#include "cmdword/ccreatecommandworddetegate.h"
#include "form/waittingform.h"
#include "cmdword/cspinboxdelegate.h"
#include "cmdword/ccomboboxdelegate.h"
#include "capppathenv.h"
#include "cmergethread.h"
#include "form/waittingform.h"
#include <QFileInfo>
#include <QProcess>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>

/**
 * CreateCommandWordForm::CreateCommandWordForm
 * \brief   constructor of CreateCommandWordForm
 * \param   cmdWordInfoList
 * \param   priority
 * \param   parent
 * \author  zhangjun
 * \date    2016-10-12
 */
CreateCommandWordForm::CreateCommandWordForm(QList<SCMDWordInfo *> *cmdWordInfoList, int priority, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateCommandWordForm)
{
    ui->setupUi(this);
    //    setAttribute(Qt::WA_DeleteOnClose);
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowMinimizeButtonHint;
    flags |= Qt::WindowMaximizeButtonHint;
//    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags(flags);

    ui->pushButton_start->setIcon(QIcon(CAppPathEnv::getImageDirPath() + "/128/start128.png"));
    ui->pushButton_cancel->setIcon(QIcon(CAppPathEnv::getImageDirPath() + "/128/cancel128.png"));

    m_priority = priority;
    m_cmdWordInfoList = cmdWordInfoList;
    m_model = new CCreateCommandWordModel(m_cmdWordInfoList);
    ui->tableView->setModel(m_model);
    //    ui->tableView->resizeColumnToContents(XMLFILE);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setColumnWidth(PROGRESS, 150);
    ui->tableView->setColumnWidth(STATUS, 100);
    ui->tableView->setColumnWidth(GAP, 50);
    ui->tableView->setColumnWidth(SOURCE, 100);
    ui->tableView->setColumnWidth(DEST, 100);
    CSpinBoxDelegate *spinBoxDelegate = new CSpinBoxDelegate;
    CComboBoxDelegate *comboBoxDelegate = new CComboBoxDelegate;
    CCreateCommandWordDetegate *progressDelegate = new CCreateCommandWordDetegate(m_cmdWordInfoList);
    ui->tableView->setItemDelegateForColumn(PROGRESS, progressDelegate);
    ui->tableView->setItemDelegateForColumn(XMLFILE, spinBoxDelegate);
    ui->tableView->setItemDelegateForColumn(ONLYSIM, spinBoxDelegate);
    ui->tableView->setItemDelegateForColumn(RMODE0, spinBoxDelegate);
    ui->tableView->setItemDelegateForColumn(RMODE1, spinBoxDelegate);
    ui->tableView->setItemDelegateForColumn(GAP, spinBoxDelegate);
    ui->tableView->setItemDelegateForColumn(BURSTNUM, spinBoxDelegate);
    ui->tableView->setItemDelegateForColumn(LOOPNUM, spinBoxDelegate);
    ui->tableView->setItemDelegateForColumn(REPEATNUM, spinBoxDelegate);
    ui->tableView->setItemDelegateForColumn(SOURCE, comboBoxDelegate);
    ui->tableView->setItemDelegateForColumn(DEST, comboBoxDelegate);

    qInfo() << tr(u8"生成命令字：初始化生成命令字参数配置对话框完毕");
}

/**
 * CreateCommandWordForm::~CreateCommandWordForm
 * \brief   destructor of CreateCommandWordForm
 * \author  zhangjun
 * \date    2016-10-12
 */
CreateCommandWordForm::~CreateCommandWordForm()
{
    delete ui;
    qDebug() << "~CreateCommandWordForm()";
}

/**
 * CreateCommandWordForm::on_pushButton_start_clicked
 * \brief   开始生成命令字
 * \author  zhangjun
 * \date    2016-10-12
 */
void CreateCommandWordForm::on_pushButton_start_clicked()
{
    qInfo() << tr(u8"生成命令字：开始生成命令字……");
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setEnabled(false);
    ui->pushButton_start->setEnabled(false);
    ui->pushButton_cancel->setEnabled(false);

    qInfo() << tr(u8"生成命令字->开始生成命令字：开始生成bin……");
    for(int i = 0; i < m_cmdWordInfoList->count(); ++i)
    {
        if(m_cmdWordInfoList->at(i)->selected == Qt::Unchecked)
        {
            qreal percent = qreal(i + 1) / m_cmdWordInfoList->count();
            int range = 33;
            percent *= range;
            ui->progressBar->setValue(qRound(percent));
            continue;
        }
        SCMDWordInfo *cmdWordInfo = m_cmdWordInfoList->at(i);
        qInfo() << tr(u8"生成命令字->开始生成命令字：文件“%1”生成bin").arg(cmdWordInfo->xmlFile);
        QString arraySimCmd = CAppPathEnv::getBinDirPath() + tr(u8"/ArraySim_Z.exe");
        arraySimCmd = CAppPathEnv::addQtaMrk(arraySimCmd);
        arraySimCmd += " -n 1";
        QString c = " -c ";
        QString bin = " -bin ";
        QString binOut = CAppPathEnv::getBinOutDirPath() + "/" + QFileInfo(cmdWordInfo->xmlFile).baseName() + ".bin.txt";
        cmdWordInfo->binFile = binOut;
        bin += CAppPathEnv::addQtaMrk(binOut);
        c += CAppPathEnv::addQtaMrk(cmdWordInfo->xmlFile);

        arraySimCmd += c;
        arraySimCmd += bin;

        qInfo() << tr(u8"生成命令字->开始生成命令字：生成bin文件命令“%1”").arg(arraySimCmd);
        int exitCode = QProcess::execute(arraySimCmd);
        //    CWaitDialog::finishShowing();
        if(exitCode != 0)
        {
            qCritical() << tr(u8"生成命令字->开始生成命令字：生成bin文件失败，ARRAYSIM退出码“%1”，退出生成命令字").arg(exitCode);
            QMessageBox::critical(this, tr(u8"错误"), tr(u8"生成bin文件失败，点击“Yes”退出！"));
//            close();
            this->reject();
            return;
        }
        else
        {
            for(int j = 0; j < 34; ++j)
            {
                if(j == 33)
                    cmdWordInfo->status = tr(u8"生成bin成功");
                cmdWordInfo->progress = j;
                m_cmdWordInfoList->replace(i, cmdWordInfo);
                ui->tableView->reset();
                if(j % m_priority == 0 || j == 33)
                {
                    ui->tableView->scrollTo(ui->tableView->model()->index(i, PROGRESS));
                    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
                }
            }
        }

        qreal percent = qreal(i + 1) / m_cmdWordInfoList->count();
        int range = 33;
        percent *= range;
        ui->progressBar->setValue(qRound(percent));
        qInfo() << tr(u8"生成命令字->开始生成命令字：生成bin文件成功");
    }
    qInfo() << tr(u8"生成命令字->开始生成命令字：文件已全部生成bin");

    //merge
    qInfo() << tr(u8"生成命令字->开始生成命令字：开始merge……");
    QString mergeCmd = CAppPathEnv::getBinDirPath() + tr(u8"/merge.exe");
    mergeCmd = CAppPathEnv::addQtaMrk(mergeCmd);
    QString in = " -i";
    QString out = " -o";
    for(int i = 0; i < m_cmdWordInfoList->count(); ++i)
    {
        if(m_cmdWordInfoList->at(i)->selected == Qt::Unchecked)
            continue;

        SCMDWordInfo *cmdWordInfo = m_cmdWordInfoList->at(i);
        in += " " + CAppPathEnv::addQtaMrk(cmdWordInfo->binFile);
        cmdWordInfo->indexFile = CAppPathEnv::getMemOutDirPath() + "/" + QFileInfo(cmdWordInfo->xmlFile).baseName() + ".merge.txt";
        out += " " + CAppPathEnv::addQtaMrk(cmdWordInfo->indexFile);
        cmdWordInfo->progress = 67;
        cmdWordInfo->status = tr(u8"merge成功");
        m_cmdWordInfoList->replace(i, cmdWordInfo);

    }
    mergeCmd = mergeCmd + in + out + " -bfu " + CAppPathEnv::addQtaMrk(CAppPathEnv::getCmdwordOutDirPath() + "/[merge]mem_bfu.txt")
            + " -mem " + CAppPathEnv::addQtaMrk(CAppPathEnv::getCmdwordOutDirPath() + "/[merge]mem_mem.txt")
            + " -sbox " + CAppPathEnv::addQtaMrk(CAppPathEnv::getCmdwordOutDirPath() + "/[merge]mem_sbox.txt")
            + " -benes " + CAppPathEnv::addQtaMrk(CAppPathEnv::getCmdwordOutDirPath() + "/[merge]mem_benes.txt")
            + " -rcu " + CAppPathEnv::addQtaMrk(CAppPathEnv::getCmdwordOutDirPath() + "/[merge]mem_rcu.txt");
    qDebug() << tr(u8"merge") << mergeCmd;
    m_mergeThread = new CMergeThread(mergeCmd);
    connect(m_mergeThread, SIGNAL(mergeExit(int)), this, SLOT(mergeFinish(int))/*, Qt::DirectConnection*/);
    connect(m_mergeThread, SIGNAL(finished()), m_mergeThread, SLOT(deleteLater()));
    CWaitDialog::readyToShowDlg();
    m_mergeThread->start();
//    int exitCode = QProcess::execute(mergeCmd);

}

void CreateCommandWordForm::mergeFinish(int exitCode)
{
    CWaitDialog::finishShowing();
    if(exitCode != 0)
    {
        qCritical() << tr(u8"生成命令字->开始生成命令字：merge失败，退出生成命令字，merge退出码“%1”").arg(exitCode);
        QMessageBox::critical(this, tr(u8"错误"), tr(u8"merge失败，点击“Yes”退出！"));
//        close();
//        return;
        this->reject();
        return;
    }
    ui->tableView->reset();
    ui->progressBar->setValue(67);
    qInfo() << tr(u8"生成命令字->开始生成命令字：merge成功");

    //cmdgen
    qInfo() << tr(u8"生成命令字->开始生成命令字：开始cmdgen……");
    for(int i = 0; i < m_cmdWordInfoList->count(); ++i)
    {
        if(m_cmdWordInfoList->at(i)->selected == Qt::Unchecked)
        {
            qreal percent = qreal(i + 1) / m_cmdWordInfoList->count();
            int range = 33;
            percent *= range;
            percent += 67;
            ui->progressBar->setValue(qRound(percent));
            continue;
        }

        SCMDWordInfo *cmdWordInfo = m_cmdWordInfoList->at(i);
        qInfo() << tr(u8"生成命令字->开始生成命令字：文件“%1”开始cmdgen").arg(cmdWordInfo->xmlFile);
        QStringList txtOut;
        txtOut.append(QString::number(cmdWordInfo->onlySim));
        txtOut.append(QString::number(cmdWordInfo->rMode0));
        txtOut.append(QString::number(cmdWordInfo->rMode1));
        txtOut.append(QString::number(cmdWordInfo->gap));
        if(cmdWordInfo->source.startsWith("M", Qt::CaseInsensitive))
        {
            int num = QString(cmdWordInfo->source.at(cmdWordInfo->source.length() - 1)).toInt();
            int ten = QString(cmdWordInfo->source.at(cmdWordInfo->source.length() - 2)).toInt();
            qDebug() << num << ten;
            QString str;
            if(ten)
                str = QString::number(ten * 10 + num);
            else
                str = QString::number(num);
            txtOut.append(str);
        }
        else if(cmdWordInfo->source.startsWith("I", Qt::CaseInsensitive))
        {
            int num = QString(cmdWordInfo->source.at(cmdWordInfo->source.length() - 1)).toInt() + 16;
            txtOut.append(QString::number(num));
        }
        if(cmdWordInfo->dest.startsWith("M", Qt::CaseInsensitive))
        {
            int num = QString(cmdWordInfo->dest.at(cmdWordInfo->dest.length() - 1)).toInt();
            int ten = QString(cmdWordInfo->dest.at(cmdWordInfo->dest.length() - 2)).toInt();
            QString str;
            if(ten)
                str = QString::number(ten * 10 + num);
            else
                str = QString::number(num);
            txtOut.append(str);
        }
        else if(cmdWordInfo->dest.startsWith("O", Qt::CaseInsensitive))
        {
            int num = QString(cmdWordInfo->dest.at(cmdWordInfo->dest.length() - 1)).toInt() + 16;
            txtOut.append(QString::number(num));
        }
        txtOut.append(QString::number(cmdWordInfo->burstNum));
        txtOut.append(QString::number(cmdWordInfo->loopNum));
        txtOut.append(QString::number(cmdWordInfo->repeatNum));
        QString cmdhead = CAppPathEnv::getHeadInDirPaht() + tr(u8"/%1.txt").arg(QFileInfo(cmdWordInfo->xmlFile).baseName());
        QFile file(cmdhead);
        if(!file.open(QIODevice::WriteOnly))
        {
            qCritical() << tr(u8"生成命令字->开始生成命令字：生成head文件失败，退出生成命令字");
            QMessageBox::critical(this, tr(u8"错误"), tr(u8"保存head文件失败！"));
            ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::AnyKeyPressed | QAbstractItemView::EditKeyPressed);
            ui->tableView->setEnabled(true);
            ui->pushButton_start->setEnabled(true);
            ui->pushButton_cancel->setEnabled(true);
            return;
        }
        QTextStream textStream(&file);
        foreach (QString str, txtOut) {
            textStream << str << endl;
        }
        file.flush();
        file.close();
        qInfo() << tr(u8"生成命令字->开始生成命令字：生成head文件成功");

        QString gencmd = CAppPathEnv::getBinDirPath() + tr(u8"/CMDGen.exe");
        gencmd = CAppPathEnv::addQtaMrk(gencmd);
        gencmd += " -idx " + CAppPathEnv::addQtaMrk(cmdWordInfo->indexFile);
        gencmd += " -bin " + CAppPathEnv::addQtaMrk(cmdWordInfo->binFile);
        gencmd += " -head " + CAppPathEnv::addQtaMrk(cmdhead);
        QString cmdgenLog = CAppPathEnv::addQtaMrk(CAppPathEnv::getCmdwordOutDirPath() + tr(u8"/[cmdgen].%1.log.txt").arg(QFileInfo(cmdWordInfo->xmlFile).baseName()));
        QString cmdgenCmd = CAppPathEnv::addQtaMrk(CAppPathEnv::getCmdwordOutDirPath() + tr(u8"/[cmdgen].%1.cmd.txt").arg(QFileInfo(cmdWordInfo->xmlFile).baseName()));
        gencmd += " -log " + cmdgenLog + " -cmd " + cmdgenCmd;
        qInfo() << tr(u8"生成命令字->开始生成命令字：cmdgen命令“%1”").arg(gencmd);

        int exitCode = QProcess::execute(gencmd);
        //    CWaitDialog::finishShowing();
        if(exitCode != 0)
        {
            qCritical() << tr(u8"生成命令字->开始生成命令字：cmdgen执行失败，退出生成命令字，cmdgen退出码“%1”").arg(exitCode);
            QMessageBox::critical(this, tr(u8"错误"), tr(u8"生成命令字失败，点击“Yes”退出！"));
            this->reject();
            return;
//            close();
//            return;
        }

        for(int j = cmdWordInfo->progress; j < 101; ++j)
        {
            if(j == 100)
                cmdWordInfo->status = tr(u8"完成");

            cmdWordInfo->progress = j;
            m_cmdWordInfoList->replace(i, cmdWordInfo);
            ui->tableView->reset();
            if(j % m_priority == 0 || j == 100)
            {
                ui->tableView->scrollTo(ui->tableView->model()->index(i, PROGRESS));
                QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
            }
        }
        qreal percent = qreal(i + 1) / m_cmdWordInfoList->count();
        int range = 33;
        percent *= range;
        percent += 67;
        ui->progressBar->setValue(qRound(percent));
        qInfo() << tr(u8"生成命令字->开始生成命令字：cmdgen成功");
    }
    ui->pushButton_start->setText(tr(u8"完成"));
    ui->pushButton_start->setIcon(QIcon(CAppPathEnv::getImageDirPath() + "/128/Success128.png"));

    qInfo() << tr(u8"生成命令字->开始生成命令字：生成命令字成功");
    int ret = QMessageBox::information(this, tr(u8"生成命令字"), tr(u8"生成命令字成功，点击“Yes”浏览生成的文件并退出！"), QMessageBox::Yes | QMessageBox::No);
    if(ret == QMessageBox::Yes)
    {
        QDesktopServices::openUrl(QUrl("file:///" + CAppPathEnv::getCmdwordOutDirPath()));
        this->accept();
    }
    else
    {
        this->accept();
    }
}
