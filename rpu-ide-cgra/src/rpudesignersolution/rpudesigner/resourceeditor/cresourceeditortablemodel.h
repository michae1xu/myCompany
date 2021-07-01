#ifndef CRESOURCEEDITORTABLEMODEL_H
#define CRESOURCEEDITORTABLEMODEL_H

#include <QAbstractTableModel>

class CBenesEditorTableModel;

class CResourceEditorTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit CResourceEditorTableModel(QWidget *parent = nullptr);
    ~CResourceEditorTableModel();

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setHeaderData(int, Qt::Orientation, const QVariant &, int = Qt::EditRole) override;
    bool insertRows(int row, int count, const QModelIndex &parent) override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;

    QString getFileName() const {return m_fileName;}
    bool getIsModified() const {return m_isModified;}
    void setIsModified(bool isModified);
    bool open(const QString &projectName, const QString &fileName, bool checkMd5Flag, int column, int addressInterval, QString type = "none");
    bool reload();
    bool save();
    bool okToContinue();
    inline void setProjectName(const QString &projectName){m_projectName = projectName;}
    bool rename(const QString &oldName, const QString &newName);
    QString getProjectName() const {return m_projectName;}
    void resetData(const QString &projectName, QMap<unsigned int, unsigned int> &resourceFileContentMap, QString type, QString fileName, int count = 4, int addressInterval = 1, bool sboxShowInOneLine = false);
    void resetData(const QString &projectName, QList<unsigned int> &resourceFileContentList, QString type, QString fileName, int count = 4, int addressInterval = 1);
    void setColumn(int columnCount);
    void setAddressInterval(int interval);
    void setShowInLine(bool flag);
    QString getType() const {return m_type;}
    CBenesEditorTableModel* getBenesEditorTableModel(){return m_benesEditorTableModel;}
    void generateBenes();
    void loadBenesConfig();

    //hardware debug
    void getSrcForHardware(uint *&data, uint &len) const;

signals:
    void resourceSaved(QString);
    void editorClosed();

public slots:
    void setDirty();

private:
    QMap<unsigned int, unsigned int> *m_resourceFileContentMap;
    QList<unsigned int> *m_resourceFileContentList;
    bool m_sboxShowInOneLine;
    int m_columnCount;
    int m_addressInterval;
    int m_totalCount;
    QString m_type;
    QString m_fileName;
    bool m_isModified;
    QWidget *m_parent;
    QString m_projectName;

    CBenesEditorTableModel *m_benesEditorTableModel;

    QVariant currencyAtMap(int row, int column) const;
    QVariant currencyAtList(int row, int column) const;
    QByteArray makeResourceMd5(const QMap<unsigned int, unsigned int> *map);
    QByteArray makeResourceMd5(const QList<unsigned int> *list);
    QString hexIntToString(unsigned int value) const;
};

#endif // CRESOURCEEDITORTABLEMODEL_H
