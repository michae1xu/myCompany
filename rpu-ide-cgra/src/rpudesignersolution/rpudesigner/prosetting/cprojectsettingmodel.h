#ifndef CPROJECTSETTINGMODEL_H
#define CPROJECTSETTINGMODEL_H

#ifndef MimeType
#define MimeType "rpudesigner/project.information"
#endif

#include "datafile.h"
#include <QAbstractTableModel>

class CProjectSettingModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    CProjectSettingModel(QList<SXmlParameter> *xmlParameterList,
                         QMap<QString, QString> *resourceMap, QObject *parent = nullptr);
    ~CProjectSettingModel();

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setHeaderData(int, Qt::Orientation, const QVariant &, int){return false;}

    Qt::DropActions supportedDragActions() const
        { return Qt::MoveAction; }
    Qt::DropActions supportedDropActions() const
        { return Qt::MoveAction; }
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData(const QMimeData *mimeData,
            Qt::DropAction action, int, int,
            const QModelIndex &parent);

    QModelIndex moveUp(const QModelIndex &index);
    QModelIndex moveDown(const QModelIndex &index);
    QModelIndex moveItem(int oldRow, int newRow);
    void selectAll();
    void refreshModel();
    void resetData(QList<SXmlParameter> *xmlParameterList, QMap<QString, QString> *resourceMap);


private:
//    SProjectParameter *m_projectParameter;
    QList<SXmlParameter> *m_xmlParameterList;
    QMap<QString, QString> *m_resourceMap;

    QVariant currencyAt(int row, int column) const;
    bool checkFileExists(const QString fileName) const;
    void sortXml();

signals:
    void currentIndex(const QModelIndex &index);

};

#endif // CPROJECTSETTINGMODEL_H
