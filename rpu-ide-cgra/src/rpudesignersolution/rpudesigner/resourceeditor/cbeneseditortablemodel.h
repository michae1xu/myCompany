#ifndef CBENESEDITORTABLEMODEL_H
#define CBENESEDITORTABLEMODEL_H

#include <QAbstractTableModel>

class CBenesEditorTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit CBenesEditorTableModel(QObject *parent = nullptr);
    ~CBenesEditorTableModel();

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setHeaderData(int, Qt::Orientation, const QVariant, int);

    QList<uint> *getValueList() const {return m_valueList;}
    bool checkValue();
    void setValue(int *out);
    void resetValue();

private:
    QList<uint> *m_valueList;
};

#endif // CBENESEDITORTABLEMODEL_H
