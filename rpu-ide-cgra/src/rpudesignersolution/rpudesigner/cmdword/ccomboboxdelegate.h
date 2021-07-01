#ifndef CCOMBOBOXDELEGATE_H
#define CCOMBOBOXDELEGATE_H

#include <QItemDelegate>
class QWidget;

class CComboBoxDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    CComboBoxDelegate(QObject *parent = 0);

    //当双击表格里的某一个元素时，会调用这个函数，创造一个Editor
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,const QModelIndex &index) const;
    // 用于把模型的数据传递给代理类的控件
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    // 用于提交修改的数据
    void setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // CCOMBOBOXDELEGATE_H
