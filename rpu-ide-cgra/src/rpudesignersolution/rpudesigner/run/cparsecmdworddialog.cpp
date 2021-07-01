#include <QFileDialog>
#include <QCompleter>
#include <QRegExpValidator>
#include "cparsecmdworddialog.h"
#include "ui_cparsecmdworddialog.h"
#include "capppathenv.hpp"
#include "rpucmdgen.h"
#include "datafile.h"
#include "fileresolver/cprojectresolver.h"

CParseCmdWordDialog::CParseCmdWordDialog(QString defaultPath, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CParseCmdWordDialog),
    m_defaultPath(defaultPath),
    m_suffix(".rpusln")
{
    ui->setupUi(this);

    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::red);
    ui->label_error->setPalette(pe);
    ui->lineEdit_slnPath->setText(m_defaultPath);
    ui->lineEdit_slnPath->setCursorPosition(0);

    QRegExp regExp("[^\\\\/:*?\"<>|]+(,[^\\\\/:*?\"<>|]+)*");
    ui->lineEdit_slnName->setValidator(new QRegExpValidator(regExp, this));

    QCompleter *completer = new QCompleter(this);
#ifndef Q_WS_X11
    completer->setCaseSensitivity(Qt::CaseInsensitive);
#endif
#ifdef USE_CUSTOM_DIR_MODEL
    completer->setModel(new CDirModel(completer));
#else
    completer->setModel(new QDirModel(completer));
#endif
    ui->lineEdit_slnPath->setCompleter(completer);
    ui->lineEdit_cfgFileName->setCompleter(completer);
    ui->lineEdit_sboxFileName->setCompleter(completer);
    ui->lineEdit_slnName->setFocus();
}

CParseCmdWordDialog::~CParseCmdWordDialog()
{
    delete ui;
}

void CParseCmdWordDialog::on_pushButton_slnPath_clicked()
{
    QString str = QFileDialog::getExistingDirectory(this, tr(u8"打开"), m_defaultPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(str.isEmpty())
        return;

    ui->lineEdit_slnPath->setText(str);
}

void CParseCmdWordDialog::on_lineEdit_slnPath_textChanged(const QString &arg1)
{
    lineEditTextChanged(ui->lineEdit_slnPath, arg1, true);
}

void CParseCmdWordDialog::on_lineEdit_slnPath_editingFinished()
{
    lineEditEditingFinished(ui->lineEdit_slnPath);
}


void CParseCmdWordDialog::on_pushButton_cfgFileName_clicked()
{
    QString str = QFileDialog::getOpenFileName(this, tr(u8"打开"), CAppPathEnv::getCmdwordOutDirPath());
    if(str.isEmpty())
        return;

    ui->lineEdit_cfgFileName->setText(str);
}


void CParseCmdWordDialog::on_lineEdit_cfgFileName_textChanged(const QString &arg1)
{
    lineEditTextChanged(ui->lineEdit_cfgFileName, arg1);
}


void CParseCmdWordDialog::on_lineEdit_cfgFileName_editingFinished()
{
    lineEditEditingFinished(ui->lineEdit_cfgFileName);
}

void CParseCmdWordDialog::on_pushButton_sboxFileName_clicked()
{
    QString str = QFileDialog::getOpenFileName(this, tr(u8"打开"), CAppPathEnv::getCmdwordOutDirPath());
    if(str.isEmpty())
        return;

    ui->lineEdit_sboxFileName->setText(str);
}


void CParseCmdWordDialog::on_lineEdit_sboxFileName_textChanged(const QString &arg1)
{
    lineEditTextChanged(ui->lineEdit_sboxFileName, arg1);
}

void CParseCmdWordDialog::on_lineEdit_sboxFileName_editingFinished()
{
    lineEditEditingFinished(ui->lineEdit_sboxFileName);
}

void CParseCmdWordDialog::on_lineEdit_slnName_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    checkFileName();
}

bool CParseCmdWordDialog::checkFileName()
{
    m_baseName = ui->lineEdit_slnName->text().trimmed();
    m_path = ui->lineEdit_slnPath->text().trimmed();
    if(m_baseName.isEmpty())
    {
        ui->label_error->setText(tr(u8"名称不能为空"));
        return false;
    }
    if(m_baseName.endsWith(m_suffix, Qt::CaseInsensitive))
    {
        m_suffix = m_baseName.right(m_suffix.length());
        m_baseName.remove(m_baseName.length() - m_suffix.length(), m_suffix.length());
    }

    if(QDir(m_path + "/" + m_baseName).exists())
    {
        ui->label_error->setText(tr(u8"文件已存在"));
        return false;
    }
    m_fileName = m_path + "/" + m_baseName + "/" + m_baseName + m_suffix;

    ui->label_error->setText("");
    return true;
}

void CParseCmdWordDialog::lineEditTextChanged(QLineEdit *lineEdit, const QString &str, bool checkDir)
{
    if(str.trimmed().isEmpty())
    {
        ui->label_error->setText(tr(u8"不能为空"));
        return;
    }
    else if(checkDir && QDir(str).exists())
    {
        QPalette palette;
        palette.setColor(QPalette::Text, Qt::black);
        lineEdit->setPalette(palette);
        ui->label_error->setText(tr(u8""));
        return;
    }
    else if(QFileInfo(str).exists() && QFileInfo(str).isFile())
    {
        QPalette palette;
        palette.setColor(QPalette::Text, Qt::black);
        lineEdit->setPalette(palette);
        ui->label_error->setText(tr(u8""));
        return;
    }
    else
    {
        QPalette palette;
        palette.setColor(QPalette::Text, Qt::red);
        lineEdit->setPalette(palette);
        ui->label_error->setText(tr(u8"文件不存在"));
        return;
    }
}

void CParseCmdWordDialog::lineEditEditingFinished(QLineEdit *lineEdit)
{
    QString text = lineEdit->text().trimmed();
#if defined(Q_OS_WIN)
    text.replace("\\", "/");
    while(text.endsWith("/"))
    {
        text.chop(1);
    }
#endif
    lineEdit->setText(text);
    lineEdit->setCursorPosition(0);
}

void CParseCmdWordDialog::on_pushButton_clicked()
{
    if(!checkFileName())
        return;

    if(!QFileInfo(ui->lineEdit_cfgFileName->text().trimmed()).exists())
    {
        ui->label_error->setText(tr(u8"配置码文件不存在"));
        return;
    }
    if(!QFileInfo(ui->lineEdit_sboxFileName->text().trimmed()).exists())
    {
        ui->label_error->setText(tr(u8"TU数据文件不存在"));
        return;
    }

    QDir dir(m_path);
    if(!dir.mkpath(m_baseName) || !dir.cd(m_baseName))
    {
        ui->label_error->setText(tr(u8"解析失败"));
        return;
    }
    if(!dir.mkpath(m_baseName))
    {
        ui->label_error->setText(tr(u8"解析失败"));
        dir.removeRecursively();
        return;
    }

    QString slnPath = m_path + "/" + m_baseName;
    QString slnFileName = m_fileName;
    QString proPath = m_path + "/" + m_baseName + "/" + m_baseName;
    QString proFileName = proPath + "/" + m_baseName + ".rpupro";

    QString ret;
    QList<SProCmdAttr> proCmdAttrList = RpuCmdGen::getInstance("")->
            revertProFromCmd(ui->lineEdit_cfgFileName->text().trimmed(),
                             ui->lineEdit_sboxFileName->text().trimmed(),
                             proPath, &ret);//使用Qt的写时复制技术，避免系统开销
    if(RpuCmdGen::getInstance("")->getIsRevertCmdFailed())
    {
        ret.chop(11);
        ui->label_error->setText(tr(u8"解析失败"));
        QMessageBox::critical(this, tr(u8"错误"), tr(u8"逆向解析配置码遇到错误：“%1”！").arg(ret.mid(45).trimmed()));
        qInfo() << tr(u8"逆向解析配置码遇到错误：“%1”！").arg(ret.mid(45).trimmed());
        RpuCmdGen::releaseInstance();
        dir.setPath(slnPath);
        dir.removeRecursively();
        return;
    }
    RpuCmdGen::releaseInstance();

    //写入rpusln
    QFile file(slnFileName);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << CAppPathEnv::absToRel(slnFileName, proFileName) << endl;
    }
    file.close();

    CProjectResolver projectResolver(proFileName, m_baseName, CIPHERTYPENONE);

    QSet<QString> srcFileList;
    SProCmdAttr proCmdAttr = proCmdAttrList.at(0);
    for (int i = 0; i < proCmdAttr.xmlCmdAttrList.count(); ++i)
    {
        SXmlCmdAttr xmlCmdAttr = proCmdAttr.xmlCmdAttrList.at(i);
        SXmlParameter xmlParameter;
        xmlParameter.sort = i;
        xmlParameter.xmlPath = xmlCmdAttr.xmlFileName;
        xmlParameter.imdPath = xmlCmdAttr.imdFileName;
        if(!xmlParameter.imdPath.isEmpty())
            srcFileList.insert(xmlParameter.imdPath);
        xmlParameter.source = xmlCmdAttr.srcType;
        xmlParameter.burstNum = xmlCmdAttr.burstNum;
        xmlParameter.loopNum = xmlCmdAttr.loopNum;
        xmlParameter.inMemPath0 = xmlCmdAttr.memFileName;
        if(!xmlParameter.inMemPath0.isEmpty())
            srcFileList.insert(xmlParameter.inMemPath0);
        xmlParameter.repeatNum = 0;
        xmlParameter.dest = 0;
        projectResolver.appendXmlElement(xmlParameter.xmlPath, &xmlParameter);
    }
    foreach (QString str, srcFileList) {
        projectResolver.appendSrcElement(str);
    }
    this->accept();
}

QString CParseCmdWordDialog::getFileName() const
{
    return m_fileName;
}

bool CParseCmdWordDialog::openSln()
{
    return ui->checkBox->isChecked();
}
