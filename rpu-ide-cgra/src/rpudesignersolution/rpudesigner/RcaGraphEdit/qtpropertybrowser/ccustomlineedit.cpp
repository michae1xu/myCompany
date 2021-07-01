#include "ccustomlineedit.h"

CCustomLineEdit::CCustomLineEdit(QWidget *parent) : QLineEdit(parent)
{
    connect(this, SIGNAL(editingFinished()), this, SLOT(slotEditingFinished()));
}

void CCustomLineEdit::slotEditingFinished()
{
    emit signalTextChanged(text());
}
