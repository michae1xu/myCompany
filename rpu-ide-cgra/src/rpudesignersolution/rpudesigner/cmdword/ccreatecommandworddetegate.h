#ifndef CCREATECOMMANDWORDDETEGATE_H
#define CCREATECOMMANDWORDDETEGATE_H

#include "datafile.h"
#include <QStyledItemDelegate>

//enum
//{
//    XMLFILE,
//    ONLYSIM,
//    RMODE0,
//    RMODE1,
//    GAP,
//    SOURCE,
//    DEST,
//    BURSTNUM,
//    LOOPNUM,
//    REPEATNUM,
//    PROGRESS,
//    STATUS
//};

class CCreateCommandWordDetegate : public QStyledItemDelegate
{
public:
    CCreateCommandWordDetegate(QList<SCMDWordInfo *> *cmdWordInfoList);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    QList<SCMDWordInfo *> *m_cmdWordInfoList;
};

#endif // CCREATECOMMANDWORDDETEGATE_H
