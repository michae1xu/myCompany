/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    parametersettingform.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include "parametersettingform.h"
#include "ui_parametersettingform.h"
#include "cparametersettingmodel.h"
//#include "parametersetting/ctoolbuttondelegate.h"
#include "parametersetting/sourceform.h"
#include "capppathenv.h"
#include <QFileInfo>
#include <QMenu>
#include <QFileDialog>
#include <QDebug>

/**
 * ParameterSettingForm::ParameterSettingForm
 * \brief   constructor of ParameterSettingForm
 * \param   list
 * \param   QString
 * \param   sourceMap
 * \param   parent
 * \author  zhangjun
 * \date    2016-10-12
 */
ParameterSettingForm::ParameterSettingForm(QList<SParaInfo *> *list, QMap<QString, QString> &sourceMap, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParameterSettingForm),
    m_paraInfoList(list),
    m_sourceMap(sourceMap)
{
    ui->setupUi(this);

    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowMinimizeButtonHint;
    flags |= Qt::WindowMaximizeButtonHint;
    flags |=Qt::WindowCloseButtonHint;
    setWindowFlags(flags);

    ui->pushButton_up->setIcon(QIcon(CAppPathEnv::getImageDirPath() + "/128/upload128.png"));
    ui->pushButton_down->setIcon(QIcon(CAppPathEnv::getImageDirPath() + "/128/download1128.png"));

    QPushButton *button = ui->buttonBox->button(QDialogButtonBox::Ok);
    button->setText(tr(u8"运行"));
    button->setIcon(QIcon(CAppPathEnv::getImageDirPath() + "/128/start128.png"));
    button = ui->buttonBox->button(QDialogButtonBox::Cancel);
    button->setText(tr(u8"取消"));
    button->setIcon(QIcon(CAppPathEnv::getImageDirPath() + "/128/cancel128.png"));

    m_contextMenu = NULL;
    m_exploreAction = NULL;
    m_sourceAction = NULL;
    m_sourceForm = NULL;


    m_model = new CParameterSettingModel(m_paraInfoList, m_sourceMap);
    ui->tableView_paraSetting->setModel(m_model);
    ui->tableView_paraSetting->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView_paraSetting, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showTableViewContextMenu(QPoint)));

    ui->tableView_paraSetting->resizeColumnsToContents();
    //    ui->tableView_paraSetting->setColumnWidth(1,50);
//    CToolButtonDelegate *toolButtonDelegate = new CToolButtonDelegate(this);
//    ui->tableView_paraSetting->setItemDelegateForColumn(INFIFOFNP0, toolButtonDelegate);
//    connect(toolButtonDelegate, SIGNAL(itemChanged(int,int,QString)), this, SLOT(itemChanged(int,int,QString)));
    qInfo() << tr(u8"运行或调试->运行调试参数配置：初始化运行调试参数配置对话框完毕");
}

/**
 * ParameterSettingForm::~ParameterSettingForm
 * \brief   destructor of ParameterSettingForm
 * \author  zhangjun
 * \date    2016-10-12
 */
ParameterSettingForm::~ParameterSettingForm()
{
    delete ui;
    delete m_model;
    delete m_exploreAction;
    delete m_sourceAction;
    delete m_sourceForm;
    //    if(m_contextMenu)
    delete m_contextMenu;
    qDebug() << "~ParameterSettingForm()";
}

/**
 * ParameterSettingForm::showTableViewContextMenu
 * \brief   
 * \param   point
 * \author  zhangjun
 * \date    2016-10-12
 */
void ParameterSettingForm::showTableViewContextMenu(QPoint point)
{
    if(!ui->tableView_paraSetting->indexAt(point).isValid())
    {
        return;
    }

    if(m_contextMenu == NULL)
    {
        m_contextMenu = new QMenu;

        m_sourceAction = m_contextMenu->addAction(tr(u8"选择资源..."));
        m_sourceAction->setIcon(QIcon(CAppPathEnv::getImageDirPath() + "/128/addons128.png"));
        connect(m_sourceAction, SIGNAL(triggered()), this, SLOT(sourceActionTriggered()));
        m_exploreAction = m_contextMenu->addAction(tr(u8"选择文件..."));
        m_exploreAction->setIcon(QIcon(CAppPathEnv::getImageDirPath() + "/128/file-explorer128.png"));
        connect(m_exploreAction, SIGNAL(triggered()), this, SLOT(exploreActionTriggered()));
        QAction *choice = m_contextMenu->addAction(tr(u8"选择"));
        choice->setIcon(QIcon(CAppPathEnv::getImageDirPath() + "/128/Success128.png"));
        connect(choice, SIGNAL(triggered()), this, SLOT(choiceActionTriggered()));
        QAction *choiceAll = m_contextMenu->addAction(tr(u8"全选"));
        choiceAll->setIcon(QIcon(""));
        connect(choiceAll, SIGNAL(triggered()), this, SLOT(choiceAllActionTriggered()));
        QAction *up = m_contextMenu->addAction(tr(u8"上移"));
        up->setIcon(QIcon(CAppPathEnv::getImageDirPath() + "/128/upload128.png"));
        connect(up, SIGNAL(triggered()), this, SLOT(upActionTriggered()));
        QAction *down = m_contextMenu->addAction(tr(u8"下移"));
        down->setIcon(QIcon(CAppPathEnv::getImageDirPath() + "/128/download1128.png"));
        connect(down, SIGNAL(triggered()), this, SLOT(downActionTriggered()));

    }

    int column = ui->tableView_paraSetting->indexAt(point).column();
    if(column < INFIFOFNP0)
    {
        m_sourceAction->setEnabled(false);
        m_exploreAction->setEnabled(false);
    }
    else
    {
        m_sourceAction->setEnabled(true);
        m_exploreAction->setEnabled(true);
    }

    m_contextMenu->exec(QCursor::pos());
}

/**
 * ParameterSettingForm::sourceActionTriggered
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void ParameterSettingForm::sourceActionTriggered()
{
    if(!m_sourceForm)
    {
        m_sourceForm = new SourceForm(m_sourceMap, this);
        connect(m_sourceForm, SIGNAL(choice(QString)), this, SLOT(setCellText(QString)));
    }
    m_sourceForm->show();
}

/**
 * ParameterSettingForm::exploreActionTriggered
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void ParameterSettingForm::exploreActionTriggered()
{
    if(!ui->tableView_paraSetting->currentIndex().isValid())
    {
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr(u8"打开"), "");
    if(fileName.isEmpty())
        return;

    int row = ui->tableView_paraSetting->currentIndex().row();
    int column = ui->tableView_paraSetting->currentIndex().column();

    SParaInfo *paraInfo = m_paraInfoList->at(row);
    switch (column) {
    case INFIFOFNP0:
        paraInfo->inputFifoFNP0 = fileName;
        break;
    case INFIFOFNP1:
        paraInfo->inputFifoFNP1 = fileName;
        break;
    case INFIFOFNP2:
        paraInfo->inputFifoFNP2 = fileName;
        break;
    case INFIFOFNP3:
        paraInfo->inputFifoFNP3 = fileName;
        break;
    case INMEMFNP0:
        paraInfo->inMemFNP0 = fileName;
        break;
    case INMEMFNP1:
        paraInfo->inMemFNP1 = fileName;
        break;
    case INMEMFNP2:
        paraInfo->inMemFNP2 = fileName;
        break;
    case INMEMFNP3:
        paraInfo->inMemFNP3 = fileName;
        break;
    case INMEMFNP4:
        paraInfo->inMemFNP4 = fileName;
        break;
    default:
        break;
    }
    ui->tableView_paraSetting->resizeColumnsToContents();
}

/**
 * ParameterSettingForm::choiceActionTriggered
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void ParameterSettingForm::choiceActionTriggered()
{
    if(!ui->tableView_paraSetting->currentIndex().isValid())
    {
        return;
    }

    int row = ui->tableView_paraSetting->currentIndex().row();

    SParaInfo *paraInfo = m_paraInfoList->at(row);
    if(paraInfo->selected == Qt::Checked)
        paraInfo->selected = Qt::Unchecked;
    else
        paraInfo->selected = Qt::Checked;
}

/**
 * ParameterSettingForm::choiceAllActionTriggered
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void ParameterSettingForm::choiceAllActionTriggered()
{
    for(int i = 0; i < m_paraInfoList->count(); ++i)
    {
        SParaInfo *paraInfo = m_paraInfoList->at(i);
        paraInfo->selected = Qt::Checked;
    }
}

/**
 * ParameterSettingForm::upActionTriggered
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void ParameterSettingForm::upActionTriggered()
{

    if(!ui->tableView_paraSetting->currentIndex().isValid())
    {
        return;
    }

    int row = ui->tableView_paraSetting->currentIndex().row();
    int column = ui->tableView_paraSetting->currentIndex().column();
    if(row == 0)
    {
        ui->tableView_paraSetting->setFocus();
        //        ui->tableView_paraSetting->setCurrentIndex(index);
        return;
    }
    //    m_model->beginReset();
    m_paraInfoList->swap(row, row - 1);
    //    m_model->endReset();
    ui->tableView_paraSetting->setFocus();
    QModelIndex index = m_model->index(row - 1, column);
    ui->tableView_paraSetting->setCurrentIndex(index);
}

/**
 * ParameterSettingForm::downActionTriggered
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void ParameterSettingForm::downActionTriggered()
{

    if(!ui->tableView_paraSetting->currentIndex().isValid())
    {
        return;
    }

    int row = ui->tableView_paraSetting->currentIndex().row();
    int column = ui->tableView_paraSetting->currentIndex().column();
    if(row >= m_paraInfoList->count() - 1)
    {
        ui->tableView_paraSetting->setFocus();
        return;
    }

    //    m_model->beginReset();
    m_paraInfoList->swap(row, row+1);
    //    m_model->endReset();

    ui->tableView_paraSetting->setFocus();
    QModelIndex index = m_model->index(row + 1, column);
    ui->tableView_paraSetting->setCurrentIndex(index);
}

/**
 * ParameterSettingForm::itemChanged
 * \brief   
 * \param   row
 * \param   column
 * \param   str
 * \author  zhangjun
 * \date    2016-10-12
 */
//void ParameterSettingForm::itemChanged(int row, int column, QString str)
//{
//    m_model->setData(m_model->index(row, column), str, Qt::EditRole);
//}

/**
 * ParameterSettingForm::setCellText
 * \brief   
 * \param   str
 * \author  zhangjun
 * \date    2016-10-12
 */
void ParameterSettingForm::setCellText(QString str)
{
    m_model->setData(ui->tableView_paraSetting->currentIndex(), str, Qt::EditRole);
}

/**
 * ParameterSettingForm::stpToNm
 * \brief   
 * \param   fullFileName
 * \return  QString
 * \author  zhangjun
 * \date    2016-10-12
 */
//QString ParameterSettingForm::stpToNm(const QString &fullFileName)
//{
//    return QFileInfo(fullFileName).fileName();
//}

/**
 * ParameterSettingForm::checkValue
 * \brief   
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool ParameterSettingForm::checkValue()
{
    for(int i = 0; i < m_paraInfoList->count(); ++i)
    {
        if(m_model->index(i, XMLFNP).data(Qt::CheckStateRole) == Qt::Unchecked)
            continue;
        if(!QFileInfo(m_model->index(i, XMLFNP).data(Qt::EditRole).toString().trimmed()).exists())
            return false;
        for(int j = INFIFOFNP0; j <= INMEMFNP4; ++j)
        {
            QString fileName = m_model->index(i, j).data(Qt::EditRole).toString().trimmed();
            if(!fileName.isEmpty() && !checkFileExists(fileName))
                return false;
        }
    }
    return true;
}

/**
 * ParameterSettingForm::checkFileExists
 * \brief   
 * \param   fileName
 * \return  bool
 * \author  zhangjun
 * \date    2016-10-12
 */
bool ParameterSettingForm::checkFileExists(const QString fileName) const
{
    if(fileName.startsWith(":"))
    {
        return QFileInfo(m_sourceMap.value(fileName)).exists();
    }
    else
    {
        return QFileInfo(fileName).exists();
    }
}

/**
 * ParameterSettingForm::on_pushButton_down_clicked
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void ParameterSettingForm::on_pushButton_down_clicked()
{
    downActionTriggered();
}

/**
 * ParameterSettingForm::on_pushButton_up_clicked
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void ParameterSettingForm::on_pushButton_up_clicked()
{
    upActionTriggered();
}

/**
 * ParameterSettingForm::on_buttonBox_clicked
 * \brief   
 * \param   button
 * \author  zhangjun
 * \date    2016-10-12
 */
void ParameterSettingForm::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button == ui->buttonBox->button(QDialogButtonBox::Ok))
    {
        if(!checkValue())
        {
            ui->tableView_paraSetting->reset();
            QMessageBox::critical(this, tr(u8"错误"), tr(u8"存在无效参数！"));
            return;
        }
        bool checked = false;
        for(int i = 0, j = 0; i < m_paraInfoList->count(); ++i)
        {
            if(m_paraInfoList->at(i)->selected == Qt::Checked)
            {
                if(!checked)
                    checked = true;

                m_paraInfoList->at(i)->sort = j;
                ++j;
            }
            else
            {
                m_paraInfoList->at(i)->sort = -1;
            }
        }
        if(checked)
        {
            if(m_sourceForm)
                m_sourceForm->hide();
            this->accept();
        }
        else
        {
            QMessageBox::information(this, tr(u8"提示"), tr(u8"请至少勾选一个调试项！"));
            return;
        }
    }
    else
        this->reject();

}
