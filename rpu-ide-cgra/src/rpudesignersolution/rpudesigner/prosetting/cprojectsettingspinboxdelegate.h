#ifndef CPROJECTSETTINGSPINBOXDELEGATE_H
#define CPROJECTSETTINGSPINBOXDELEGATE_H

#include <QItemDelegate>

class QWidget;

class CProjectSettingSpinBoxDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit CProjectSettingSpinBoxDelegate(QObject *parent = nullptr);

    //当双击表格里的某一个元素时，会调用这个函数，创造一个Editor
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,const QModelIndex &index) const;
    // 用于把模型的数据传递给代理类的控件
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    // 用于提交修改的数据
    void setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // CPROJECTSETTINGSPINBOXDELEGATE_H
