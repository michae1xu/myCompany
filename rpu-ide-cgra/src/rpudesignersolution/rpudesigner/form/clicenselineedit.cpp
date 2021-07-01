#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>
#include "clicenselineedit.h"

CLicenseLineEdit::CLicenseLineEdit(QWidget *parent) :
    QLineEdit(parent)
{

}

void CLicenseLineEdit::keyPressEvent(QKeyEvent *event)
{
    if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_V
            && !qApp->clipboard()->text().trimmed().isEmpty())
    {
        emit pasteLicense(qApp->clipboard()->text().trimmed());
        event->accept();
        return;
    }
    QLineEdit::keyPressEvent(event);
}
