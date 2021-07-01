#include <QSpinBox>
#include "cbeneseditorspinboxdelegate.h"

CBenesEditorSpinBoxDelegate::CBenesEditorSpinBoxDelegate(QObject *parent) :
    QItemDelegate(parent)
{

}

QWidget *CBenesEditorSpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.isValid())
    {
        QSpinBox *spinBox = new QSpinBox(parent);
        spinBox->setFrame(false);
        spinBox->setMinimum(0);
        spinBox->setMaximum(127);
        return spinBox;
    }
    return  QItemDelegate::createEditor(parent, option, index);
}

void CBenesEditorSpinBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QItemDelegate::setEditorData(editor, index);
}

void CBenesEditorSpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QItemDelegate::setModelData(editor, model, index);
}

void CBenesEditorSpinBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QItemDelegate::updateEditorGeometry(editor, option, index);
}
