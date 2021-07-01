#ifndef CPARAMETERSETTINGMODEL_H
#define CPARAMETERSETTINGMODEL_H

#include "datafile.h"
#include <QAbstractTableModel>
#include <QMap>

enum
{
    XMLFNP,
    PROJECT,
    CYCLE,
    INTERVAL,
    INFIFOFNP0,
    INFIFOFNP1,
    INFIFOFNP2,
    INFIFOFNP3,
    INMEMFNP0,
    INMEMFNP1,
    INMEMFNP2,
    INMEMFNP3,
    INMEMFNP4,
};

class CParameterSettingModel : public QAbstractTableModel
{
public:
    CParameterSettingModel(QList<SParaInfo *> *list, QMap<QString, QString> sourceMap);
    ~CParameterSettingModel();

//    QString stpToNm(const QString &fullFileName) const;

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
//    void beginReset();
//    void endReset();


private:
    QList<SParaInfo *> *m_paraInfoList;
    QMap<QString, QString> m_sourceMap;

    QVariant currencyAt(int row, int column) const;
    bool checkFileExists(const QString fileName) const;


};

#endif // CPARAMETERSETTINGMODEL_H
