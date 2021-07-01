#include <QApplication>
#include <QAction>
#include <QFileDialog>
#include <QMimeData>
#include <QCloseEvent>
#include "Qsci/qscilexerlua.h"
#include "Qsci/qscilexercpp.h"
#include "Qsci/qsciapis.h"
#include "cfileeditor.h"
#include "common/cappenv.hpp"
#include "common/debug.h"

CFileEditor::CFileEditor(QWidget *parent) :
    QsciScintilla(parent),
    m_action(new QAction(this))
{
    setAcceptDrops(true);
    setWindowTitle("[*]");
    setAttribute(Qt::WA_DeleteOnClose);
    setFrameShape(QFrame::Panel);

    this->setMarginType(0, QsciScintilla::NumberMargin);//设置编号为0的页边显示行号。
    this->setMarginLineNumbers(0, true);//对该页边启用行号
    this->setMarginWidth(0, 50);//设置页边宽度
    this->setMarginsBackgroundColor("#F0F0F0"); //显示行号背景颜色
//    this->setCaretLineVisible(true);   //显示选中行号
//    this->setCaretLineBackgroundColor(Qt::lightGray);
    this->setBraceMatching(QsciScintilla::SloppyBraceMatch); //括号匹配
    QsciLexerCPP *textLexer = new QsciLexerCPP(this);//创建一个词法分析器
    QsciAPIs *apis = new QsciAPIs(textLexer);
    if(apis->load(CAppEnv::getEtcDirPath() + "/api.txt"))
        apis->prepare();
    this->setLexer(textLexer);//给QsciScintilla设置词法分析器
    this->setAutoCompletionSource(QsciScintilla::AcsAll);   //设置源
    this->setAutoCompletionCaseSensitivity(true);   //设置自动补全大小写敏感
    this->setAutoCompletionThreshold(1);    //设置每输入一个字符就会出现自动补全的提示
    this->setFont(QFont("Courier New"));
    this->SendScintilla(QsciScintilla::SCI_SETCODEPAGE, QsciScintilla::SC_CP_UTF8);//设置编码为UTF-8
    this->setAutoIndent(true);

    m_action->setCheckable(true);
    connect(m_action, SIGNAL(triggered()), this, SLOT(show()));
    connect(m_action, SIGNAL(triggered()), this, SLOT(setFocus()));
    connect(m_action, SIGNAL(triggered()), this, SIGNAL(activeWindowChanged()));
}

CFileEditor::~CFileEditor()
{
    DBG << "~CFileEditor()";
}


void CFileEditor::newFile()
{
    static int documentNumber = 1;

    m_curFile = CAppEnv::getTmpDirPath() + "/" + tr(u8"untitled%1.xml").arg(documentNumber);
    setWindowTitle(CAppEnv::stpToNm(m_curFile) + "[*]");
    setWindowModified(false);
    m_action->setText(CAppEnv::stpToNm(m_curFile));
    m_isUntitled = true;
    ++documentNumber;
}

bool CFileEditor::open(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    if(!read(&file))
    {
        return false;
    }
    setCurrentFile(fileName);
    return true;
}


bool CFileEditor::save()
{
    if (m_isUntitled) {
        return saveAs();
    } else {
        return saveFile(m_curFile);
    }
}

bool CFileEditor::saveAs()
{
//    QString str = CAppEnv::getXmlOutDirPath() + "/" + CAppEnv::stpToNm(m_curFile);
    QString fileName = QFileDialog::getSaveFileName(this, tr(u8"另存为"), ""/*str*/, tr(u8"XML 文件 (*.xml)"));
    if (fileName.isEmpty())
    {
        return true;
    }

    return saveFile(fileName);
}

bool CFileEditor::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    if (!write(&file)) {
        return false;
    }
    setCurrentFile(fileName);
    return true;
}

bool CFileEditor::okToContinue()
{
    if (isWindowModified()) {
        int ret = QMessageBox::warning(parentWidget(), qApp->applicationName(),
                                       tr(u8"是否保存对 “%0” 的修改？")
                                       .arg(CAppEnv::stpToNm(m_curFile)),
                                       QMessageBox::Save | QMessageBox::Discard |
                                       QMessageBox::Cancel);
        if (ret == QMessageBox::Save) {
            return save();
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}


void CFileEditor::setCurrentFile(const QString &fileName)
{
    m_curFile = fileName;
    m_isUntitled = false;
    m_action->setText(CAppEnv::stpToNm(m_curFile));
    setWindowModified(false);
    setWindowTitle(CAppEnv::stpToNm(m_curFile) + "[*]");
}

bool CFileEditor::getIsUntitled() const
{
    return m_isUntitled;
}

QString CFileEditor::getStatus()
{
    return "";
}

void CFileEditor::closeEvent(QCloseEvent *event)
{
    if (okToContinue()) {
        emit fileEditorClosed(m_curFile);
        event->accept();
    } else {
        event->ignore();
    }
}

void CFileEditor::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if(mimeData->hasUrls())
    {
        QStringList xmlList, unSupportList;
        QList<QUrl> urlList = mimeData->urls();
        for(int i = 0; i < urlList.count(); ++i)
        {
            QString fileName = urlList.at(i).toLocalFile();
            if(fileName.endsWith(".S", Qt::CaseInsensitive))
            {
                xmlList.append(fileName);
            }
            else
            {
                unSupportList.append(fileName);
            }
        }
        emit dropFile(xmlList, unSupportList);
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}
