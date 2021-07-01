#ifndef CBENESEDITORSPINBOXDELEGATE_H
#define CBENESEDITORSPINBOXDELEGATE_H

#include <QItemDelegate>

class CBenesEditorSpinBoxDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit CBenesEditorSpinBoxDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const;
};


#endif // CBENESEDITORSPINBOXDELEGATE_H
