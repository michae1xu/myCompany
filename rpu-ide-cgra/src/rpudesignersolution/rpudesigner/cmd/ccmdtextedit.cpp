/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    ccmdtextedit.cpp
 * \brief   implement functions
 * \version 1.0
 * \author  Jun Zhang
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-12
 ********************************************************************
 */
#include <QTextBlock>
#include "common/debug.h"
#include "ccmdtextedit.h"

/**
 * CCmdTextEdit::CCmdTextEdit
 * \brief   constructor of CCmdTextEdit
 * \param   parent
 * \author  zhangjun
 * \date    2016-10-12
 */
CCmdTextEdit::CCmdTextEdit(QWidget *parent)
    :QTextEdit(parent)
{
    //    setPlaceholderText(tr(u8">"));
    //    this->appendText("<a><font color=#5aff5a><strong>"rcs$ "</strong></font></a>");
    //    this->setCursorWidth(3);
//    this->append("<a><font color=#5aff5a>DSN></font> </a>");

    //    QTextFrameFormat format;
    //    format.setBackground(Qt::gray);
    //    this->document()->rootFrame()->setFrameFormat(format);
    m_deep = 0;
    this->setLineWrapMode(QTextEdit::NoWrap);
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
    connect(this, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
}

CCmdTextEdit::~CCmdTextEdit()
{
}

/**
 * CCmdTextEdit::cursorPositionChanged
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void CCmdTextEdit::cursorPositionChanged()
{
    //    QTextBlock block = p->document()->findBlockByNumber(nLineNum);
    //    p->setTextCursor(QTextCursor(block));
    //    QTextCursor txtCursor = this->textCursor();
    //    this->textCursor().positionInBlock(); 光标在当前行的位置
    //    this->textCursor().positionInBlock();

    if(textCursor().block().blockNumber() != document()->lastBlock().blockNumber())
    {
//        DBG << tr(u8"CCmdTextEdit->cursorPositionChanged：不是当前行，设为只读");
        this->setReadOnly(true);
    }
    else
    {
        if(textCursor().positionInBlock() < 5)
        {
//            DBG << tr(u8"CCmdTextEdit->cursorPositionChanged：当前行光标位置小于5，设为只读");
            this->setReadOnly(true);
            QTextCursor textCursor = this->textCursor();
            textCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::MoveAnchor);
            this->setTextCursor(textCursor);
        }
        else
        {
//            DBG << tr(u8"CCmdTextEdit->cursorPositionChanged：当前行光标位置大于5，设为读写");
            this->setReadOnly(false);
        }
    }
    //    QTextBlock block = this->document()->lastBlock();
    //    this->setTextCursor(QTextCursor(block));
    //    this->cur;

    //    if()
    //    setReadOnly(true);
}

/**
 * CCmdTextEdit::selectionChanged
 * \brief   
 * \author  zhangjun
 * \date    2016-10-12
 */
void CCmdTextEdit::selectionChanged()
{
    if(textCursor().hasSelection())
    {
//        DBG << tr(u8"CCmdTextEdit->selectionChanged：有选择，设为只读");
        this->setReadOnly(true);
    }
    else
    {
        cursorPositionChanged();
    }
}

/**
 * CCmdTextEdit::keyPressEvent
 * \brief   
 * \param   event
 * \author  zhangjun
 * \date    2016-10-12
 */
void CCmdTextEdit::keyPressEvent(QKeyEvent *event)
{
    if((event->key() == Qt::Key_Up || event->key() == Qt::Key_Down) && (textCursor().block().blockNumber() == document()->lastBlock().blockNumber() && textCursor().positionInBlock() > 4))
    {
        if(event->key() == Qt::Key_Up)
        {
            if(m_deep < m_historyList.count())
            {
                ++m_deep;
            }
            else
            {
                event->ignore();
                return;
            }
            QTextCursor textCursor = this->textCursor();
            textCursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
            textCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);
            textCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);
            textCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);
            textCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);
            textCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);
            this->setTextCursor(textCursor);
            int length = document()->lastBlock().text().length() - 5;
            for(int i = 0; i < length; ++i)
                this->textCursor().deleteChar();
            this->insertPlainText(m_historyList.at(m_historyList.count() - m_deep));
        }
        else
        {
            if(m_deep > 1)
            {
                --m_deep;
            }
            else
            {
             //清空
                event->ignore();
                return;
            }
            QTextCursor textCursor = this->textCursor();
            textCursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
            textCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);
            textCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);
            textCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);
            textCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);
            textCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);
            this->setTextCursor(textCursor);
            int length = document()->lastBlock().text().length() - 5;
            for(int i = 0; i < length; ++i)
                this->textCursor().deleteChar();
            this->insertPlainText(m_historyList.at(m_historyList.count() - m_deep));
        }
        event->ignore();
        return;
    }
    else
    {
        m_deep = 0;
    }

    if(event->key() == Qt::Key_Backspace && (textCursor().block().blockNumber() != document()->lastBlock().blockNumber() || textCursor().positionInBlock() < 6))
    {
//        DBG << tr(u8"CCmdTextEdit->keyPressEvent：可读，但不可擦除前一个字符");
        event->ignore();
        return;
    }

    if(event->key() == Qt::Key_Return)
    {
        event->ignore();
        QTextCursor textCursor = this->textCursor();
        textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        setTextCursor(textCursor);

        QString cmd = document()->lastBlock().text().mid(5);
        emit sendCmd(cmd);
        if(!cmd.trimmed().isEmpty())
        {
            if(!m_historyList.isEmpty() && m_historyList.last() != cmd)
                m_historyList.append(cmd);
            if(m_historyList.isEmpty())
                m_historyList.append(cmd);
        }

        return;
    }

    if(event->modifiers() == Qt::CTRL && event->key() == Qt::Key_C)
    {
        emit cmdCancel();
        QTextCursor textCursor = this->textCursor();
        textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        this->setTextCursor(textCursor);
        this->insertPlainText("^C");
        this->append("<a><font color=#5aff5a>DSN></font> </a>");
    }
    QTextEdit::keyPressEvent(event);
}

