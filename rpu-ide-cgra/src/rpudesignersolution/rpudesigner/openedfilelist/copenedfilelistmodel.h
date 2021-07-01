#ifndef COPENEDFILELISTMODEL_H
#define COPENEDFILELISTMODEL_H

#include <QStringListModel>

class COpenedFileListModel : public QStringListModel
{
    Q_OBJECT
public:
    explicit COpenedFileListModel(QStringList *openedFileList, QObject *parent = nullptr);
    ~COpenedFileListModel();

    QVariant data(const QModelIndex &index, int role) const;
//    bool setData(const QModelIndex &index, const QVariant &value, int role);
    int rowCount(const QModelIndex &parent) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    QStringList *m_openedFileList;

//    QString stpToNm(QString fullFileName) const;
};

#endif // COPENEDFILELISTMODEL_H
