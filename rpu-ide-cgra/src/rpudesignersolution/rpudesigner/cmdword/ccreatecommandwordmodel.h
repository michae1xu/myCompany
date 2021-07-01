#ifndef CCREATECOMMANDWORDMODEL_H
#define CCREATECOMMANDWORDMODEL_H

#include <QAbstractTableModel>
#include "datafile.h"

enum
{
    XMLFILE,
    ONLYSIM,
    RMODE0,
    RMODE1,
    GAP,
    SOURCE,
    DEST,
    BURSTNUM,
    LOOPNUM,
    REPEATNUM,
    PROGRESS,
    STATUS
};

class CCreateCommandWordModel : public QAbstractTableModel
{
public:
    CCreateCommandWordModel(QList<SCMDWordInfo *> *cmdWordInfoList);

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    QList<SCMDWordInfo *> *m_cmdWordInfoList;

//    QString stpToNm(const QString &fullFileName) const;

    QVariant currencyAt(int row, int column) const;
};

#endif // CCREATECOMMANDWORDMODEL_H
