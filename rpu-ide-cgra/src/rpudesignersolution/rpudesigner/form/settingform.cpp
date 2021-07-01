/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    settingform.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include <QSettings>
#include <QDesktopServices>
#include <QUrl>
#include <QDirModel>
#include <QCompleter>
#include <QScrollBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include <QDomDocument>
#include "settingform.h"
#include "ui_settingform.h"
#include "common/cappenv.hpp"
#include "common/debug.h"
//#include "oventimer.h"
//#include "liboventimer.h"

/**
 * SettingForm::SettingForm
 * \brief   constructor of SettingForm
 * \param   setting
 * \param   parent
 * \author  zhangjun
 * \date    2016-10-12
 */
SettingForm::SettingForm(SSetting &setting, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingForm),
    m_setting(setting)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);

    getSimSetting();

    for(int i = 0; i < ui->listWidget->count(); ++i)
    {
        ui->listWidget->item(i)->setSizeHint(QSize(150 * CAppEnv::m_dpiFactor, 40 * CAppEnv::m_dpiFactor));
    }
    ui->listWidget->item(0)->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/chart128.png"));
//    ui->listWidget->item(1)->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Globe128_1.png"));
    ui->listWidget->item(1)->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Product-documentation128.png"));
    ui->listWidget->item(2)->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/product128.png"));
    ui->listWidget->item(3)->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Compasses128.png"));
    ui->listWidget->item(4)->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Tools1-128.png"));
    ui->listWidget->item(5)->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Open-file128.png"));
    ui->listWidget->item(6)->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/start128.png"));
    ui->listWidget->item(7)->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/backup-restore128.png"));
    ui->listWidget->item(8)->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Inventory-maintenance128.png"));
    ui->listWidget->item(9)->setIcon(QIcon(CAppEnv::getImageDirPath() + "/cmd.png"));
    ui->listWidget->item(10)->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/wifi128.png"));

    ui->spinBox_maxCycle->setRange(10, std::numeric_limits<int>::max());

    ui->groupBox_11->setEnabled(false);
    ui->checkBox_logArchive->setVisible(false);
    ui->checkBox_cycleOut->setVisible(false);
    ui->pushButton_cycleOut->setVisible(false);
    ui->label_11->setVisible(false);
    ui->pushButton_2->setVisible(false);
    ui->label_12->setVisible(false);

    ui->listWidget->setCurrentRow(0);

    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::red);
    ui->label_info->setPalette(pe);
#if defined(Q_OS_WIN)
    ui->checkBox_autoRun->setChecked(m_setting.autoRun);
#else
    ui->checkBox_autoRun->setVisible(false);
    ui->checkBox_autoRun->setEnabled(false);
    ui->checkBox_autoRun->setChecked(false);
#endif
//    ui->horizontalSlider_priority->setValue(m_setting.priority);
    ui->checkBox_logOut->setChecked(m_setting.logOut);
    on_checkBox_logOut_clicked(m_setting.logOut);
    ui->spinBox_logSize->setValue(m_setting.logSize);
    ui->checkBox_logArchive->setChecked(m_setting.logArchive);
    ui->spinBox_maxSubWin->setValue(m_setting.maxSubWin);
    ui->checkBox_showTab->setChecked(m_setting.showTab);
    on_checkBox_showTab_clicked(m_setting.showTab);
    switch (m_setting.tabType) {
    case 0:
        ui->radioButton_rounded->setChecked(true);
        break;
    case 1:
        ui->radioButton_triangular->setChecked(true);
        break;
    default:
        ui->radioButton_rounded->setChecked(true);
        break;
    }

    ui->checkBox_cycleOut->setChecked(m_setting.cycleOut);
//    ui->checkBox_autoSave->setChecked(m_setting.autoSave);
//    ui->spinBox_autoSave->setValue(m_setting.saveInterval);
//    on_checkBox_autoSave_clicked();
    ui->checkBox_sysTray->setChecked(m_setting.sysTray);
    ui->checkBox_autoSave->setChecked(m_setting.autoSave);
    ui->spinBox_autoSave->setValue(m_setting.saveInterval);
    ui->horizontalSlider_autoSave->setValue(m_setting.saveInterval);
    ui->spinBox_autoSave->setEnabled(m_setting.autoSave);
    ui->horizontalSlider_autoSave->setEnabled(m_setting.autoSave);
//    if(m_setting.autoSave)
//        m_ovenTimer->set(m_setting.saveInterval * 60);
//    else
//        m_ovenTimer->set(0);
    ui->checkBox_tabMovable->setChecked(m_setting.tabMovable);
    ui->checkBox_tabClosable->setChecked(m_setting.tabClosable);
    ui->comboBox_tabPosition->setCurrentIndex(m_setting.tabPosition);
    ui->checkBox_xmlMd5Check->setChecked(!m_setting.xmlMd5Check);
    ui->comboBox_iconSize->setCurrentIndex(m_setting.iconSize);
    ui->checkBox_srcMd5Check->setChecked(!m_setting.srcMd5Check);
    ui->spinBox_maxCycle->setValue(m_setting.maxCycle);
    ui->spinBox_recentFiles->setValue(m_setting.recentFileCount);
    ui->spinBox_recentProjects->setValue(m_setting.recentProCount);
    ui->checkBox_logDelete->setChecked(m_setting.logDelete);
    ui->checkBox_logDelete->setChecked(m_setting.logDelete);
    ui->spinBox_logDeleteDate->setValue(m_setting.logDeleteDate);
//    switch (m_setting.sortType) {
//    case 0:
//        ui->radioButton_noSort->setChecked(true);
//        break;
//    case 1:
//        ui->radioButton_sortByAlgName->setChecked(true);
//        break;
//    case 2:
//        ui->radioButton_sortByXmlName->setChecked(true);
//        break;
//    default:
//        ui->radioButton_noSort->setChecked(true);
//        break;
//    }

    ui->checkBox_showGrid->setChecked(m_setting.showGrid);
    ui->spinBox_intervalX->setValue(m_setting.gridPoint.x());
    ui->spinBox_intervalY->setValue(m_setting.gridPoint.y());
    ui->spinBox_intervalX->setEnabled(m_setting.showGrid);
    ui->spinBox_intervalY->setEnabled(m_setting.showGrid);
    ui->checkBox_cmdGenDir->setChecked(m_setting.openCmdGenDir);
    ui->checkBox_customCmdGenDir->setChecked(m_setting.customCmdGenDirFlag);
    ui->lineEdit_customCmdGenDir->setEnabled(m_setting.customCmdGenDirFlag);
    ui->pushButton_customCmdGenDir->setEnabled(m_setting.customCmdGenDirFlag);
    ui->lineEdit_customCmdGenDir->setText(m_setting.customCmdGenDir);
    ui->lineEdit_customCmdGenDir->setCursorPosition(0);
    ui->lineEdit_defaultProPath->setText(m_setting.defaultProPath);
    ui->lineEdit_defaultProPath->setCursorPosition(0);
    ui->lineEdit_cmdGenCommand->setText(m_setting.cmdGenOutSideCommand);
    ui->checkBox_saveWave->setChecked(m_setting.saveWave);
    ui->lineEdit_pythonLocate->setText(m_setting.pythonLocate);

    ui->fontComboBox->setCurrentFont(m_setting.cmdFont);
    ui->doubleSpinBox->setValue(m_setting.cmdFontSize);

    ui->comboBox_networkProxy->setCurrentIndex(m_setting.networkPorxy);
    ui->lineEdit_networkProxyAddress->setText(m_setting.networkPorxyAddress);
    ui->spinBox_networkProxyPort->setValue(m_setting.networkPorxyPort);
    ui->lineEdit_networkProxyUserName->setText(m_setting.networkPorxyUserName);
    ui->lineEdit_networkProxyPassword->setText(m_setting.networkPorxyPassword);
    on_comboBox_networkProxy_currentIndexChanged(m_setting.networkPorxy);

    QCompleter *completer = new QCompleter(this);
#ifndef Q_WS_X11
    completer->setCaseSensitivity(Qt::CaseInsensitive);
#endif
#ifdef USE_CUSTOM_DIR_MODEL
    completer->setModel(new CDirModel(completer));
#else
    completer->setModel(new QDirModel(completer));
#endif
    ui->lineEdit_defaultProPath->setCompleter(completer);
    ui->lineEdit_customCmdGenDir->setCompleter(completer);
    ui->lineEdit_pythonLocate->setCompleter(completer);

    QRegExp regExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    ui->lineEdit_networkProxyAddress->setValidator(new QRegExpValidator(regExp, this));

    connect(ui->scrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(sliderValueChanged(int)));

    resize(geometry().size() * CAppEnv::m_dpiFactor);
}

/**
 * SettingForm::~SettingForm
 * \brief   destructor of SettingForm
 * \author  zhangjun
 * \date    2016-10-12
 */
SettingForm::~SettingForm()
{
    delete m_document;
    m_document = nullptr;
    delete ui;
}

void SettingForm::timeOut()
{
    ui->label_info->clear();
}

void SettingForm::sliderValueChanged(int value)
{
    QList<QGroupBox *> list = ui->scrollArea->findChildren<QGroupBox *>();
    for (int i = 0; i < list.count(); ++i)
    {
        QGroupBox *groupBox = list.at(i);
        if(value >= groupBox->pos().y() - 15)
        {
            ui->listWidget->setCurrentRow(i);
        }
    }
}

/**
 * SettingForm::on_buttonBox_accepted
 * \brief
 * \author  zhangjun
 * \date    2016-10-12
 */
void SettingForm::on_buttonBox_accepted()
{
    if(!checkSetting())
    {
        ui->label_info->setText(tr(u8"存在无效设置"));
        QTimer::singleShot(5000, this, SLOT(timeOut()));
        return;
    }

//    m_setting.priority = ui->horizontalSlider_priority->value();
    m_setting.logOut = ui->checkBox_logOut->isChecked();
    if(m_setting.logSize != ui->spinBox_logSize->value())
    {
        m_setting.logSize = ui->spinBox_logSize->value();
        m_restatFlag = true;
    }
    if(m_setting.logArchive != ui->checkBox_logArchive->isChecked())
    {
        m_setting.logArchive = ui->checkBox_logArchive->isChecked();
        m_restatFlag = true;
    }
    if(m_setting.logDelete != ui->checkBox_logDelete->isChecked())
    {
        m_setting.logDelete = ui->checkBox_logDelete->isChecked();
        m_restatFlag = true;
    }
    if(m_setting.logDeleteDate != ui->spinBox_logDeleteDate->value())
    {
        m_setting.logDeleteDate = ui->spinBox_logDeleteDate->value();
        m_restatFlag = true;
    }

#if defined(Q_OS_WIN)
    if(m_setting.autoRun != ui->checkBox_autoRun->isChecked())
    {
        QSettings *settings = new QSettings(REG_RUN, QSettings::NativeFormat);
        if(ui->checkBox_autoRun->isChecked())
        {
            QString appFilePath = CAppEnv::getAppFilePath();
            settings->setValue(CAppEnv::getAppName(), "\"" + appFilePath.replace("/", "\\") + "\"");
        }
        else
        {
            settings->remove(CAppEnv::getAppName());
        }
        delete settings;
        m_setting.autoRun = ui->checkBox_autoRun->isChecked();
    }
#endif

    m_setting.maxSubWin = ui->spinBox_maxSubWin->value();
    m_setting.showTab = ui->checkBox_showTab->isChecked();
    if(ui->radioButton_rounded->isChecked())
        m_setting.tabType = 0;
    else
        m_setting.tabType = 1;

    m_setting.cycleOut = ui->checkBox_cycleOut->isChecked();

    m_setting.autoSave = ui->checkBox_autoSave->isChecked();
    m_setting.saveInterval = ui->spinBox_autoSave->value();
//    if(m_ovenTimer->value() == 0)
//    {
//        m_setting.autoSave = false;
//        m_setting.saveInterval = 0;
//    }
//    else
//    {
//        m_setting.autoSave = true;
//        int base = m_ovenTimer->value() / 60;
//        if (m_ovenTimer->value() % 60 >= 30) {
//            m_setting.saveInterval = base + 1;
//        } else {
//            m_setting.saveInterval = base;
//        }
//    }

    m_setting.sysTray = ui->checkBox_sysTray->isChecked();

    m_setting.tabMovable = ui->checkBox_tabMovable->isChecked();
    m_setting.tabClosable = ui->checkBox_tabClosable->isChecked();
    m_setting.tabPosition = ui->comboBox_tabPosition->currentIndex();

    m_setting.xmlMd5Check = !ui->checkBox_xmlMd5Check->isChecked();

    m_setting.iconSize = ui->comboBox_iconSize->currentIndex();

    m_setting.srcMd5Check = !ui->checkBox_srcMd5Check->isChecked();

    m_setting.maxCycle = ui->spinBox_maxCycle->value();

    if(m_setting.recentFileCount != ui->spinBox_recentFiles->value())
    {
        m_setting.recentFileCount = ui->spinBox_recentFiles->value();
        m_restatFlag = true;
    }
    if( m_setting.recentProCount != ui->spinBox_recentProjects->value())
    {
        m_setting.recentProCount = ui->spinBox_recentProjects->value();
        m_restatFlag = true;
    }

//    if(ui->radioButton_noSort->isChecked())
//        m_setting.sortType = 0;
//    else if(ui->radioButton_sortByAlgName->isChecked())
//        m_setting.sortType = 1;
//    else if(ui->radioButton_sortByXmlName->isChecked())
//        m_setting.sortType = 2;
//    else
//        m_setting.sortType = 0;

    m_setting.showGrid = ui->checkBox_showGrid->isChecked();
    m_setting.gridPoint.setX(ui->spinBox_intervalX->value());
    m_setting.gridPoint.setY(ui->spinBox_intervalY->value());
    m_setting.openCmdGenDir = ui->checkBox_cmdGenDir->isChecked();
    m_setting.customCmdGenDirFlag = ui->checkBox_customCmdGenDir->isChecked();
    m_setting.customCmdGenDir = ui->lineEdit_customCmdGenDir->text().trimmed();
    m_setting.defaultProPath = ui->lineEdit_defaultProPath->text().trimmed();
    m_setting.cmdGenOutSideCommand = ui->lineEdit_cmdGenCommand->text().trimmed();
    m_setting.saveWave = ui->checkBox_saveWave->isChecked();
    m_setting.pythonLocate = ui->lineEdit_pythonLocate->text().trimmed();

//    m_setting.cmdFont.setPointSizeF((qreal)ui->doubleSpinBox->value());
    m_setting.cmdFont = ui->fontComboBox->currentFont();
    m_setting.cmdFontSize = ui->doubleSpinBox->value();

    m_setting.networkPorxy = ui->comboBox_networkProxy->currentIndex();
    m_setting.networkPorxyAddress = ui->lineEdit_networkProxyAddress->text().trimmed();
    m_setting.networkPorxyPort = ui->spinBox_networkProxyPort->value();
    m_setting.networkPorxyUserName = ui->lineEdit_networkProxyUserName->text().trimmed();
    m_setting.networkPorxyPassword = ui->lineEdit_networkProxyPassword->text().trimmed();

    if(m_restatFlag)
        QMessageBox::information(this, tr(u8"首选项"), tr(u8"部分功能需重启后生效!"));

    setSimSetting();

    this->accept();
}

bool SettingForm::checkSetting() const
{
    if(ui->checkBox_customCmdGenDir->isChecked() &&
            (ui->lineEdit_customCmdGenDir->text().trimmed().isEmpty() || !QFileInfo(ui->lineEdit_customCmdGenDir->text().trimmed()).exists()))
    {
        return false;
    }
    if(!QFileInfo(ui->lineEdit_defaultProPath->text().trimmed()).exists()
            && !ui->lineEdit_defaultProPath->text().trimmed().isEmpty())
    {
        return false;
    }
    if(ui->comboBox_networkProxy->currentIndex()
            && ui->lineEdit_networkProxyAddress->text().trimmed().isEmpty())
    {
        return false;
    }
    if(!ui->lineEdit_pythonLocate->text().trimmed().isEmpty()
            && !QFileInfo(ui->lineEdit_pythonLocate->text().trimmed()).isExecutable())
    {
        return false;
    }

    return true;
}

void SettingForm::getSimSetting()
{
    QFile file(CAppEnv::getCustomEtcSimulatorConfigFilePath());
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << file.errorString();
    }
    m_document = new QDomDocument();
    m_document->setContent(&file);
    file.close();

    auto m_root = m_document->documentElement();
    if(m_root.isNull() || m_root.tagName() != "all")
    {
        QDomElement element = m_document->createElement("all");
        m_document->appendChild(element);
        m_root = element;
    }

    QDomNode node = m_document->childNodes().at(0);
    if(node.toProcessingInstruction().isNull())
    {
        QDomProcessingInstruction instruction;
        instruction = m_document->createProcessingInstruction("xml", "version=\'1.0\' encoding=\'UTF-8\'");
        m_document->insertBefore(instruction, m_root);
    }
    else
    {
        /*QDomProcessingInstruction instruction = */node.toProcessingInstruction();
    }

    auto tb = m_root.firstChildElement("tb");
    if(tb.isNull())
    {
        QDomElement element = m_document->createElement("tb");
        element.setAttribute("top_clock", 500);
        element.setAttribute("simu_time", 1000);
        element.setAttribute("print_period", 5000);
        element.setAttribute("rcu_num", 256);
        tb = element;
        m_root.appendChild(element);
    }
    int topClock = tb.attribute("top_clock").toInt();
    int simTime = tb.attribute("simu_time").toInt();
    int printPeriod = tb.attribute("print_period").toInt();
    int rcuNum = tb.attribute("rcu_num").toInt();
    ui->spinBox_sim_clock->setValue(topClock);
    ui->spinBox_sim_time->setValue(simTime);
    ui->spinBox_sim_printPeriod->setValue(printPeriod);
    ui->spinBox_sim_reconfigMaxCount->setValue(rcuNum);
}

void SettingForm::setSimSetting()
{
    auto m_root = m_document->documentElement();
    if(m_root.isNull() || m_root.tagName() != "all")
    {
        QDomElement element = m_document->createElement("all");
        m_document->appendChild(element);
        m_root = element;
    }

    QDomNode node = m_document->childNodes().at(0);
    if(node.toProcessingInstruction().isNull())
    {
        QDomProcessingInstruction instruction;
        instruction = m_document->createProcessingInstruction("xml", "version=\'1.0\' encoding=\'UTF-8\'");
        m_document->insertBefore(instruction, m_root);
    }
    else
    {
        /*QDomProcessingInstruction instruction = */node.toProcessingInstruction();
    }

    auto tb = m_root.firstChildElement("tb");
    if(tb.isNull())
    {
        QDomElement element = m_document->createElement("tb");
        m_root.appendChild(element);
        tb = element;
    }
    tb.setAttribute("top_clock", ui->spinBox_sim_clock->value());
    tb.setAttribute("simu_time", ui->spinBox_sim_time->value());
    tb.setAttribute("print_period", ui->spinBox_sim_printPeriod->value());
    tb.setAttribute("rcu_num", ui->spinBox_sim_reconfigMaxCount->value());

    QFile file(CAppEnv::getCustomEtcSimulatorConfigFilePath());
    if(!file.open(QFile::WriteOnly | QFile::Text)) {
        qDebug() << file.errorString();
    }
    QTextStream out(&file);
    m_document->save(out, 4);
}

/*!
 * SettingForm::on_listWidget_clicked
 * \brief
 * \param  index
 * \author zhangjun
 * \date   2016-10-17
 */
void SettingForm::on_listWidget_clicked(const QModelIndex &index)
{    
    QList<QGroupBox *> list = ui->scrollArea->findChildren<QGroupBox *>();
    QScrollBar *scrollBar = ui->scrollArea->verticalScrollBar();
    disconnect(ui->scrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)),
               this, SLOT(sliderValueChanged(int)));
    scrollBar->setSliderPosition(list.at(index.row())->pos().y() - 15);
    connect(ui->scrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)),
               this, SLOT(sliderValueChanged(int)));
}


void SettingForm::on_checkBox_logOut_clicked(bool checked)
{
    ui->label_logOut->setEnabled(checked);
    ui->spinBox_logSize->setEnabled(checked);
    ui->horizontalSlider_logSize->setEnabled(checked);
    ui->pushButton_lookLog->setEnabled(checked);
    ui->checkBox_logArchive->setEnabled(checked);
    ui->checkBox_logDelete->setEnabled(checked);
    ui->spinBox_logDeleteDate->setEnabled(checked);
    ui->label_13->setEnabled(checked);
}

void SettingForm::on_pushButton_lookLog_clicked()
{
    QDesktopServices::openUrl(QUrl("file:///" + CAppEnv::getLogDirPath()));
}

void SettingForm::on_checkBox_showTab_clicked(bool checked)
{
    ui->label_tabStyle->setEnabled(checked);
    ui->radioButton_rounded->setEnabled(checked);
    ui->radioButton_triangular->setEnabled(checked);
    ui->label_tabPosition->setEnabled(checked);
    ui->comboBox_tabPosition->setEnabled(checked);
    ui->checkBox_tabMovable->setEnabled(checked);
    ui->checkBox_tabClosable->setEnabled(checked);
}

void SettingForm::on_pushButton_cycleOut_clicked()
{
    QDesktopServices::openUrl(QUrl("file:///" + CAppEnv::getCycleOutDirPath()));
}

//void SettingForm::on_checkBox_autoSave_clicked()
//{
//    ui->label_autoSave->setEnabled(ui->checkBox_autoSave->isChecked());
//    ui->spinBox_autoSave->setEnabled(ui->checkBox_autoSave->isChecked());
//    ui->horizontalSlider_autoSave->setEnabled(ui->checkBox_autoSave->isChecked());
//}

void SettingForm::on_checkBox_logDelete_clicked(bool checked)
{
    ui->label_13->setEnabled(checked);
    ui->spinBox_logDeleteDate->setEnabled(checked);
}

void SettingForm::on_checkBox_showGrid_clicked(bool checked)
{
    ui->spinBox_intervalX->setEnabled(checked);
    ui->spinBox_intervalY->setEnabled(checked);
}

void SettingForm::on_comboBox_networkProxy_currentIndexChanged(int index)
{
    if(index)
    {
        ui->label_networkProxyAddress->setEnabled(true);
        ui->label_networkProxyPort->setEnabled(true);
        ui->label_networkProxyUserName->setEnabled(true);
        ui->label_networkProxyPassword->setEnabled(true);
        ui->lineEdit_networkProxyAddress->setEnabled(true);
        ui->spinBox_networkProxyPort->setEnabled(true);
        ui->lineEdit_networkProxyPassword->setEnabled(true);
        ui->lineEdit_networkProxyUserName->setEnabled(true);
    }
    else
    {
        ui->label_networkProxyAddress->setEnabled(false);
        ui->label_networkProxyPort->setEnabled(false);
        ui->label_networkProxyUserName->setEnabled(false);
        ui->label_networkProxyPassword->setEnabled(false);
        ui->lineEdit_networkProxyAddress->setEnabled(false);
        ui->spinBox_networkProxyPort->setEnabled(false);
        ui->lineEdit_networkProxyPassword->setEnabled(false);
        ui->lineEdit_networkProxyUserName->setEnabled(false);
    }
}

void SettingForm::on_pushButton_defaultProPath_clicked()
{
    QString str = QFileDialog::getExistingDirectory(this, tr(u8"选择解决方案默认路径"), m_setting.defaultProPath);
    if(!str.isEmpty())
        ui->lineEdit_defaultProPath->setText(str);
}

void SettingForm::on_lineEdit_defaultProPath_textChanged(const QString &arg1)
{
    if(QDir(arg1.trimmed()).exists() || arg1.trimmed().isEmpty())
    {
        QPalette palette;
        palette.setColor(QPalette::Text, Qt::black);
        ui->lineEdit_defaultProPath->setPalette(palette);
    }
    else
    {
        QPalette palette;
        palette.setColor(QPalette::Text, Qt::red);
        ui->lineEdit_defaultProPath->setPalette(palette);
    }
}

void SettingForm::on_lineEdit_defaultProPath_editingFinished()
{
    QString text = ui->lineEdit_defaultProPath->text().trimmed();
#if defined(Q_OS_WIN)
    text.replace("\\", "/");
    while(text.endsWith("/"))
    {
        text.chop(1);
    }
    if(text.endsWith(":"))
        text += "/";
#endif
    ui->lineEdit_defaultProPath->setText(text);
    ui->lineEdit_defaultProPath->setCursorPosition(0);
}

void SettingForm::on_pushButton_customCmdGenDir_clicked()
{
    QString str = QFileDialog::getExistingDirectory(this, tr(u8"选择文件夹"), m_setting.customCmdGenDir);
    if(!str.isEmpty())
        ui->lineEdit_customCmdGenDir->setText(str);
}

void SettingForm::on_lineEdit_customCmdGenDir_textChanged(const QString &arg1)
{
    if(QDir(arg1.trimmed()).exists() || arg1.trimmed().isEmpty())
    {
        QPalette palette;
        palette.setColor(QPalette::Text, Qt::black);
        ui->lineEdit_customCmdGenDir->setPalette(palette);
    }
    else
    {
        QPalette palette;
        palette.setColor(QPalette::Text, Qt::red);
        ui->lineEdit_customCmdGenDir->setPalette(palette);
    }
}

void SettingForm::on_lineEdit_customCmdGenDir_editingFinished()
{
    QString text = ui->lineEdit_customCmdGenDir->text().trimmed();
#if defined(Q_OS_WIN)
    text.replace("\\", "/");
    while(text.endsWith("/"))
    {
        text.chop(1);
    }
#endif
    ui->lineEdit_customCmdGenDir->setText(text);
    ui->lineEdit_customCmdGenDir->setCursorPosition(0);
}

void SettingForm::on_pushButton_pythonLocate_clicked()
{
#if defined(Q_OS_WIN)
    QString str = QFileDialog::getOpenFileName(this, tr(u8"python路径"), "", tr(u8"python (python*.exe)"));
#else
    QString str = QFileDialog::getOpenFileName(this, tr(u8"python路径"), "", tr(u8"python (python*)"));
#endif
    if(!str.isEmpty())
        ui->lineEdit_pythonLocate->setText(str);
}

void SettingForm::on_lineEdit_pythonLocate_textChanged(const QString &arg1)
{
    if(QFileInfo(arg1.trimmed()).isExecutable() || arg1.trimmed().isEmpty())
    {
        QPalette palette;
        palette.setColor(QPalette::Text, Qt::black);
        ui->lineEdit_pythonLocate->setPalette(palette);
    }
    else
    {
        QPalette palette;
        palette.setColor(QPalette::Text, Qt::red);
        ui->lineEdit_pythonLocate->setPalette(palette);
    }
}

void SettingForm::on_lineEdit_pythonLocate_editingFinished()
{
    QString text = ui->lineEdit_pythonLocate->text().trimmed();
#if defined(Q_OS_WIN)
    text.replace("\\", "/");
    while(text.endsWith("/"))
    {
        text.chop(1);
    }
#endif
    ui->lineEdit_pythonLocate->setText(text);
    ui->lineEdit_pythonLocate->setCursorPosition(0);
}
