#include "ccustomspinbox.h"

CCustomSpinBox::CCustomSpinBox(QWidget *parent) : QSpinBox(parent)
{
    connect(this, SIGNAL(editingFinished()), this, SLOT(slotEditingFinished()));
}

void CCustomSpinBox::slotEditingFinished()
{
    emit signalValueChanged(value());
}

