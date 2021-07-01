#ifndef CTOOLBUTTONDELEGATE_H
#define CTOOLBUTTONDELEGATE_H

#include <QItemDelegate>
class QWidget;
class QToolButton;
class QMenu;

class CToolButtonDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    CToolButtonDelegate(QObject *parent = 0);
    ~CToolButtonDelegate();
    void setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:
    void itemChanged(int, int, QString);

private slots:
    void srcTriggered();

private:
    QMap<QModelIndex, QMenu*> m_menuMap;
};

#endif // CTOOLBUTTONDELEGATE_H
