#include <QApplication>
#include <QColor>
#include <QMap>
#include <QStyleOptionComboBox>
#include <QFile>
#include <QMessageBox>
#include <QCryptographicHash>
#include "common/cappenv.hpp"
#include "common/debug.h"
#include "cbeneseditortablemodel.h"
#include "cresourceeditortablemodel.h"
#include "cbenesformatconvert.h"
#include "benes_new.h"
#include "form/resourcetypeform.h"

#define MEMORY_LEN  64*128   //bit
#define IMD_LEN     64*128   //bit
#define INFIFO_LEN  128*128  //bit
#define OUTFIFO_LEN 64*128   //bit
#define SBOX_LEN    1024*128 //bit
#define RCH_LEN     16*32    //bit
#define BYTE        8
#define WORD        32

CResourceEditorTableModel::CResourceEditorTableModel(QWidget *parent) :
    QAbstractTableModel(parent),
    m_sboxShowInOneLine(true),
    m_columnCount(0),
    m_addressInterval(0),
    m_totalCount(0),
    m_isModified(false),
    m_parent(parent)
{
    m_benesEditorTableModel = new CBenesEditorTableModel(this);

    m_resourceFileContentMap = new QMap<unsigned int, unsigned int>;
    m_resourceFileContentList = new QList<unsigned int>;
    connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(setDirty()));
}

CResourceEditorTableModel::~CResourceEditorTableModel()
{
    if(!okToContinue())
        return;
    emit editorClosed();
    delete m_resourceFileContentList;
    delete m_resourceFileContentMap;
}

void CResourceEditorTableModel::resetData(const QString &projectName, QMap<unsigned int, unsigned int> &resourceFileContentMap, QString type, QString fileName, int count, int addressInterval, bool sboxShowInOneLine)
{
    beginResetModel();
    m_resourceFileContentMap->clear();
    *m_resourceFileContentMap = resourceFileContentMap;
    m_sboxShowInOneLine = sboxShowInOneLine;
    m_addressInterval = addressInterval;
    m_columnCount = count;
    m_type = type;
    m_fileName = fileName;
    m_projectName = projectName;
    if(m_type == "memory")
    {
        m_totalCount = MEMORY_LEN / WORD;
    }
    else if(m_type == "imd")
    {
        m_totalCount = IMD_LEN / WORD;
    }
    if(m_type == "infifo")
    {
        m_totalCount = INFIFO_LEN / WORD;
    }
    else if(m_type == "sbox")
    {
        m_totalCount = SBOX_LEN / WORD;
    }
    else if(m_type == "benes")
    {
        m_totalCount = 26;
    }

    if(m_resourceFileContentMap->count() < 1) {
        for(auto i = 0; i < m_totalCount; ++i)
        {
            m_resourceFileContentMap->insert(i*4, 0);
        }
    }

    m_isModified = false;
    endResetModel();
}

void CResourceEditorTableModel::resetData(const QString &projectName, QList<unsigned int> &resourceFileContentList, QString type, QString fileName, int count, int addressInterval)
{
    beginResetModel();
    m_resourceFileContentList->clear();
    m_resourceFileContentList->append(resourceFileContentList);
    m_columnCount = count;
    m_addressInterval = addressInterval;
    m_type = type;
    m_fileName = fileName;
    m_projectName = projectName;
    m_totalCount = m_resourceFileContentList->count();
    m_isModified = false;
    endResetModel();
}

void CResourceEditorTableModel::setColumn(int columnCount)
{
    beginResetModel();
    m_columnCount = columnCount;
    endResetModel();
}

void CResourceEditorTableModel::setAddressInterval(int interval)
{
    beginResetModel();
    m_addressInterval = interval;
    endResetModel();
}

void CResourceEditorTableModel::setShowInLine(bool flag)
{
    beginResetModel();
    m_sboxShowInOneLine = flag;
    endResetModel();
}

void CResourceEditorTableModel::generateBenes()
{
    if(m_type != "benes")
        return;

    if(!m_benesEditorTableModel->checkValue())
    {
        QMessageBox::warning(m_parent, qApp->applicationName(), tr(u8"配置存在错误，请核对！"));
        return;
    }

    CBenesFormatConvert convert;
    convert.setValue(m_benesEditorTableModel->getValueList());
    convert.convert128To26(0, 128);

    int **config = convert.getConfig();
    uint *cf = new uint[26];
    for (int i = 0; i < 13; i++)
    {
        cf[i * 2] = 0;
        cf[i * 2 + 1] = 0;
        for (int j = 0; j < 64; j++)
        {
            uint bit = 0;
            if (config[12 - i][63 - j] == 1) bit = 1;
            if (j < 32)
                cf[i * 2] = (cf[i * 2] << 1) | bit;
            else
                cf[i * 2 + 1] = (cf[i * 2 + 1] << 1) | bit;
        }
    }

    beginResetModel();
    m_resourceFileContentMap->clear();
    for(int i = 25; i >= 0; --i)
    {
        m_resourceFileContentMap->insert((unsigned int)(25 - i) * 4, cf[i]);
    }
    endResetModel();
    QModelIndex parentIndex = createIndex(0, 0).parent();
    emit dataChanged(parentIndex, parentIndex);
    delete cf;
}

void CResourceEditorTableModel::loadBenesConfig()
{
    if(m_type != "benes")
        return;

    int *in = new int[128];
    for(int i = 0; i < 128; ++i)
    {
        in[i] = i;
    }
    int *out = new int[128];

    QHash<QChar, QString> hexIndex;
    hexIndex.insert(QChar('0'), "0000");
    hexIndex.insert(QChar('1'), "0001");
    hexIndex.insert(QChar('2'), "0010");
    hexIndex.insert(QChar('3'), "0011");
    hexIndex.insert(QChar('4'), "0100");
    hexIndex.insert(QChar('5'), "0101");
    hexIndex.insert(QChar('6'), "0110");
    hexIndex.insert(QChar('7'), "0111");
    hexIndex.insert(QChar('8'), "1000");
    hexIndex.insert(QChar('9'), "1001");
    hexIndex.insert(QChar('A'), "1010");
    hexIndex.insert(QChar('B'), "1011");
    hexIndex.insert(QChar('C'), "1100");
    hexIndex.insert(QChar('D'), "1101");
    hexIndex.insert(QChar('E'), "1110");
    hexIndex.insert(QChar('F'), "1111");

    vector<bool> cfg;
    QMapIterator<unsigned int, unsigned int> itor(*m_resourceFileContentMap);
    while (itor.hasNext()) {
        itor.next();
        unsigned int value = itor.value();
        QString str = QString::number(value, 16).toUpper();
        int length = 8 - str.length();
        for(int i = 0; i < length; ++i)
        {
            str.insert(0, "0");
        }

        for(int i = str.count() - 1; i >= 0; --i)
        {
            QString bin = hexIndex.value(str.at(i));
            for(int j = bin.count() - 1; j >= 0; --j)
            {
                cfg.push_back(bin.at(j) == QChar('1'));
            }
        }
    }

//    vector<bool>::iterator it;
//    int num = 0;
//    for(it = cfg.begin(); it != cfg.end(); it++)
//    {
//        qInfo() << num++ << ":" << *it;
//    }

    benes_new benes;
    benes.benes128x128(in, cfg, out);

    m_benesEditorTableModel->setValue(out);

    delete[] in;
    delete[] out;
}


void CResourceEditorTableModel::setDirty()
{
    m_isModified = true;
}

void CResourceEditorTableModel::setIsModified(bool isModified)
{
    m_isModified = isModified;
}

QVariant CResourceEditorTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.column() < 0)
    {
        return QVariant();
    }

    if (role == Qt::SizeHintRole)
    {
        QStyleOptionComboBox option;
        option.currentText = "00000000000";
        QFontMetrics fontMetrics(data(index, Qt::EditRole).value<QFont>());
        option.fontMetrics = fontMetrics;
        QSize size(fontMetrics.width(option.currentText), fontMetrics.height());
        return qApp->style()->sizeFromContents(QStyle::CT_ComboBox, &option, size);

    }
    if (role == Qt::TextAlignmentRole)
    {
        return int(Qt::AlignVCenter | Qt::AlignHCenter);
    }
    else if (role == Qt::DisplayRole)
    {
        QString text;
        if(m_type == "fifo")
        {
            text = currencyAtList(index.row(), index.column()).toString();
            if(!text.isEmpty())
            {
                text = "0x" + currencyAtList(index.row(), index.column()).toString();
            }
        }
        else if(m_type == "none")
        {
            Q_ASSERT(false);
        }
        else if(m_type == "sbox" && m_sboxShowInOneLine)
        {
            text = "0x" + currencyAtMap(index.row(), index.column()).toString();
        }
        else if((m_type == "sbox" && !m_sboxShowInOneLine) || m_type == "memory" || m_type == "imd" || m_type == "benes")
        {
            text = "0x" + currencyAtMap(index.row(), index.column()).toString();
        }
        QVariant current = text;
        return current;
    }
    else if (role == Qt::EditRole)
    {
        QString text;
        if(m_type == "fifo")
        {
            text = currencyAtList(index.row(), index.column()).toString();
        }
        else if(m_type == "none")
        {
        }
        else if(m_type == "sbox" && m_sboxShowInOneLine)
        {
            text = currencyAtMap(index.row(), index.column()).toString();
        }
        else if((m_type == "sbox" && !m_sboxShowInOneLine) || m_type == "memory" || m_type == "imd" || m_type == "benes")
        {
            text = currencyAtMap(index.row(), index.column()).toString();
        }
        QVariant current = text;
        return current;
    }
    return QVariant();
}

bool CResourceEditorTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.column() < 0)
    {
        return false;
    }

    int row = index.row();
    int column = index.column();
    if (role == Qt::EditRole)
    {
        if(m_type == "fifo")
        {
            bool ok;
            int idx = m_columnCount * row + column;
            unsigned int vle = value.toString().toUInt(&ok, 16);
            if(!ok)
            {
                vle = 0;
            }
            m_resourceFileContentList->replace(idx, vle);
            emit dataChanged(index, index);
            return true;
        }
        else if(m_type == "sbox" || m_type == "imd" || m_type == "memory" || m_type == "benes")
        {
            bool ok;
            unsigned int key;
            if(m_type == "sbox" && m_sboxShowInOneLine)
            {
                key = column * 4096 + row * 4;
            }
//            else if(m_type == "imd")
//            {
//                key = 4 * (m_columnCount * row + column);
//            }
            else
            {
                key = 4 * (m_columnCount * row + column);
            }
            unsigned int vle = value.toString().toUInt(&ok, 16);
            if(!ok)
            {
                vle = 0;
            }
            m_resourceFileContentMap->remove(key);
            m_resourceFileContentMap->insert(key, vle);
            emit dataChanged(index, index);
            return true;
        }
    }
    return false;
}

int CResourceEditorTableModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid() || m_columnCount == 0)
    {
        return 0;
    }
    else
    {
        int count = m_totalCount / m_columnCount;
        if(m_totalCount % m_columnCount != 0)
        {
            count++;
        }
        return count;
    }
}

int CResourceEditorTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_columnCount;
}

QVariant CResourceEditorTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if(orientation == Qt::Horizontal)
    {
        if(m_type == "fifo")
        {
            return section;
        }
        else if(m_type == "none")
        {
            Q_ASSERT(false);
        }
        else if(m_type == "sbox" || m_type == "memory" || m_type == "imd" || m_type == "benes")
        {
            int num;
            if(m_type == "sbox" && m_sboxShowInOneLine)
            {
                num = section * 4096;
            }
            else
            {
                num = section * 4;
            }
            int interval = m_addressInterval;
            while(interval)
            {
                num /= 4;
                --interval;
            }
            QString str = "+" + QString::number(num, 16);
            return str;
        }
    }
    else
    {
        if(m_type == "fifo")
        {
            return section;
        }
        else if(m_type == "none")
        {
            Q_ASSERT(false);
        }
        else if(m_type == "sbox" || m_type == "imd" || m_type == "memory" || m_type == "benes")
        {
            int num;
            if(m_type == "sbox" && m_sboxShowInOneLine)
            {
                num = section * 4;
            }
//            else if(m_type == "imd")
//            {
//                num = section * m_columnCount * 4 + 1024;
//            }
            else{
                num = section * m_columnCount * 4;
            }
            int interval = m_addressInterval;
            while(interval)
            {
                num /= 4;
                --interval;
            }
            QString str = QString::number(num, 16);
            int length = 8 - str.length();
            for(int i = 0; i < length; ++i)
            {
                str.insert(0, "0");
            }
            str = "0x" + str;
            return str;
        }
//        else if(m_type == "sbox")
//        {
//            int num = section * m_columnCount * 4;
//            int ffcCount = num / 4093;
//            int ffc = num % 4093;
//            QString str = QString::number(ffc, 16);
//            str.insert(0, QString::number(ffcCount, 16));
//            int length = 8 - str.length();
//            for(int i = 0; i < length; ++i)
//            {
//                str.insert(0, "0");
//            }
//            str = "0x" + str;
//            return str;
//        }
    }
    return QVariant();
}

Qt::ItemFlags CResourceEditorTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    if(index.isValid())
    {
        flags |= Qt::ItemIsEnabled;
        flags |= Qt::ItemIsSelectable;
        if(data(index, Qt::DisplayRole).toString().isEmpty())
        {
            return flags;
        }
        if(m_columnCount * index.row() + index.column() < m_totalCount)
            flags |= Qt::ItemIsEditable;
    }
    return flags;
}

bool CResourceEditorTableModel::setHeaderData(int , Qt::Orientation , const QVariant &, int )
{
    return false;
}

bool CResourceEditorTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if(m_type != "fifo")
        return false;
    beginInsertRows(parent, row, row + count - 1);
    int index = row * m_columnCount;
    for(int i = 0; i < m_columnCount; ++i)
    {
        m_resourceFileContentList->insert(index, (unsigned)0);
    }
    m_totalCount = m_resourceFileContentList->count();
    emit dataChanged(parent, parent);
    endInsertRows();
    return true;
}

bool CResourceEditorTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if(m_type != "fifo")
        return false;
    beginRemoveRows(parent, row, row + count - 1);
    int index = row * m_columnCount;
    for(int i = 0; i < m_columnCount * count; ++i)
    {
        m_resourceFileContentList->removeAt(index);
    }
    m_totalCount = m_resourceFileContentList->count();
    emit dataChanged(parent, parent);
    endRemoveRows();
    return true;
}


bool CResourceEditorTableModel::okToContinue()
{
    if(m_isModified)
    {
        int &&ret = QMessageBox::warning(m_parent, qApp->applicationName(),
                                       tr(u8"是否保存对资源 “%0” 的修改？")
                                       .arg(CAppEnv::stpToNm(m_fileName)),
                                       QMessageBox::Save | QMessageBox::Discard |
                                       QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
        {
            return save();
        }
        else if (ret == QMessageBox::Cancel)
        {
            return false;
        }
//        else if (ret == QMessageBox::Discard)
//        {
//            m_isModified = false;
//            return open(m_projectName, m_fileName, false, m_columnCount);
//        }
    }
    return true;
}

bool CResourceEditorTableModel::rename(const QString &oldName, const QString &newName)
{
    if(m_fileName == oldName)
    {
        m_fileName = newName;
        return true;
    }
    return false;
}


bool CResourceEditorTableModel::open(const QString &projectName, const QString &fileName, bool checkMd5Flag, int column, int addressInterval, QString type)
{
    if(!okToContinue())
        return false;

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << tr(u8"打开文件“%0”遇到错误：\n%1.")
                       .arg(file.fileName())
                       .arg(file.errorString());
        QMessageBox::warning(m_parent, qApp->applicationName(),
                             tr(u8"打开文件“%0”遇到错误：\n%1.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }

    QTextStream in(&file);
    QString md5InFile;
    if(type != "none")
    {
        checkMd5Flag = false;
    }
    while(!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if(line.startsWith("# Type:"))
        {
            type = line.split(":").at(1);
            continue;
        }
        else if(line.startsWith("# Md5:"))
        {
            md5InFile = line.split(":").at(1);
            continue;
        }
    }
    in.seek(0);
    bool saveFlag = (type != "none");
    if(!saveFlag)
    {
        ResourceTypeForm form(m_parent);
        if(QDialog::Rejected == form.exec())
        {
            file.close();
            return false;
        }
        type = form.getType();
        saveFlag = saveFlag || form.getSave();
    }
    qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
    QMap<unsigned int, unsigned int> resourceFileContentMap;
    QList<unsigned int> resourceFileContentList;

    if(type == "memory" || type == "imd" || type == "sbox" || type == "benes")
    {
        while(!in.atEnd())
        {
            QString line = in.readLine().trimmed();
            if(line.startsWith("#"))
            {
                continue;
            }
            QStringList cellList = line.split(" ");
            if(cellList.count() < 1)
            {
                continue;
            }
            bool ok;
            unsigned int baseMemIndex = QString(cellList.at(0)).toUInt(&ok, 0);
            if(!ok)
            {
                continue;
            }
            for(int i = 1; i < cellList.count(); ++i)
            {
                unsigned int memIndex = baseMemIndex + (i - 1) * 4;
                unsigned int cell =  QString(cellList.at(i)).toUInt(&ok, 16);
                if(!ok)
                {
                    continue;
                }
                resourceFileContentMap.insert(memIndex, cell);
            }
        }
    }
    else if(type == "fifo")
    {
        while(!in.atEnd())
        {
            QString line = in.readLine().trimmed();
            if(line.startsWith("#"))
            {
                continue;
            }
            QStringList cellList = line.split(" ");
            for(int i = 1; i < cellList.count(); ++i)
            {
                bool ok;
                unsigned int cell =  QString(cellList.at(i)).toUInt(&ok, 16);
                if(!ok)
                {
                    continue;
                }
                resourceFileContentList.append(cell);
            }
        }
    }
    else
    {
        qApp->restoreOverrideCursor();
        QMessageBox::critical(m_parent, qApp->applicationName(),
                             tr(u8"打开文件“%0”遇到错误：\n%1.")
                             .arg(file.fileName())
                             .arg(tr(u8"文件类型无法识别")));
        return false;
    }

    if(checkMd5Flag)
    {
        QString md5;
        if(type == "fifo")
            md5 = makeResourceMd5(&resourceFileContentList).toHex();
        else
            md5 = makeResourceMd5(&resourceFileContentMap).toHex();
        if(md5 != md5InFile)
        {
            qInfo() << tr(u8"打开资源文件：在试图打开文件时遇到错误，文件已被损坏：%0").arg(fileName);
            qApp->restoreOverrideCursor();
            QMessageBox::warning(m_parent, qApp->applicationName(),
                                 tr(u8"在试图打开文件时遇到错误，文件“%0”已被损坏！").arg(fileName));
            file.close();
            return false;
        }
    }
    file.close();


    if(type == "fifo")
    {
        resetData(projectName, resourceFileContentList, type, fileName);
    }
    else if(type == "sbox")
    {
        resetData(projectName, resourceFileContentMap, type, fileName, column, addressInterval, true);
    }
    else if(type == "memory" || type == "imd" || type == "benes")
    {
        resetData(projectName, resourceFileContentMap, type, fileName, column, addressInterval);
    }

    if(saveFlag)
    {
        save();
    }
    qApp->restoreOverrideCursor();
    return true;
}

bool CResourceEditorTableModel::reload()
{
    m_isModified = false;
    return open(m_projectName, m_fileName, false, m_columnCount, m_addressInterval);
}

bool CResourceEditorTableModel::save()
{
    QFile file(m_fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QWidget *wgt = qobject_cast<QWidget *>(m_parent);
        QMessageBox::warning(wgt, qApp->applicationName(),
                             tr(u8"打开文件“%0”遇到错误：\n%1.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out << "# This is a " << qApp->applicationName() << " project resource file." << endl;
    out << tr(u8"# Type:%0").arg(m_type) << endl;
    QString md5;
    if(m_type == "fifo")
        md5 = makeResourceMd5(m_resourceFileContentList).toHex();
    else
        md5 = makeResourceMd5(m_resourceFileContentMap).toHex();
    out << tr(u8"# Md5:%0").arg(md5) << endl;


    if(m_type == "benes" || m_type == "sbox" || m_type == "imd")
    {
        QMapIterator<unsigned int, unsigned int> itor(*m_resourceFileContentMap);
        while (itor.hasNext())
        {
            itor.next();
            out << hexIntToString(itor.key()) << " " << hexIntToString(itor.value()) << endl;
        }
    }
    else if(m_type == "memory")
    {
        int count = 0;
        QMapIterator<unsigned int, unsigned int> itor(*m_resourceFileContentMap);
        while (itor.hasNext())
        {
            itor.next();
            if(count == 0)
            {
                out << hexIntToString(itor.key()) << " " << hexIntToString(itor.value());
                ++count;
                continue;
            }
            if(count % 4 == 0)
            {
                out << endl << hexIntToString(itor.key()) << " " << hexIntToString(itor.value());
            }
            else
            {
                out << " " << hexIntToString(itor.value());
            }
            ++count;
        }
    }
    else if(m_type == "fifo")
    {
        for(int i = 0; i < m_resourceFileContentList->count(); ++i)
        {
            if(i == 0)
            {
                out << "0x00000000 " << hexIntToString(m_resourceFileContentList->at(i));
                continue;
            }
            if(i % 4 == 0)
            {
                out << endl << "0x00000000 " << hexIntToString(m_resourceFileContentList->at(i));
            }
            else
            {
                out << " " << hexIntToString(m_resourceFileContentList->at(i));
            }

        }
    }
    out.flush();
    file.close();
    m_isModified = false;
    emit resourceSaved(m_fileName);
    return true;
}




QString CResourceEditorTableModel::hexIntToString(unsigned int value) const
{
    QString str = QString::number(value, 16).toUpper();
    int length = 8 - str.length();
    for(int i = 0; i < length; ++i)
    {
        str.insert(0, "0");
    }
    str = "0x" + str;
    return str;
}

QByteArray CResourceEditorTableModel::makeResourceMd5(const QMap<unsigned int, unsigned int> *map)
{
    if(!map)
        return QByteArray();

    QString content;
    QMapIterator<unsigned int, unsigned int> itor(*map);
    while (itor.hasNext()) {
        itor.next();
        if(itor.value() != 0)
        {
            content.append(QString::number(itor.key()));
            content.append(QString::number(itor.value()));
        }
    }
    QByteArray bty;
    bty.append(content);
    QByteArray md5Code = QCryptographicHash::hash(bty, QCryptographicHash::Md5);
    return md5Code;
}

QByteArray CResourceEditorTableModel::makeResourceMd5(const QList<unsigned int> *list)
{
    if(!list || list->count() < 1)
        return QByteArray();

    QString content;
    foreach (unsigned int str, *list)
    {
        if(str != 0)
        {
            content.append(QString::number(str));
        }
    }
    QByteArray bty;
    bty.append(content);
    QByteArray md5Code = QCryptographicHash::hash(bty, QCryptographicHash::Md5);
    return md5Code;
}


QVariant CResourceEditorTableModel::currencyAtMap(int row, int column) const
{
    if(!m_resourceFileContentMap || m_resourceFileContentMap->count() < 1)
        return QVariant();
    unsigned int key;
    if(m_sboxShowInOneLine)
    {
        key = column * 4096 + row * 4;
    }
//    else if(m_type == "memory32")
//    {
//        key = 4 * (m_columnCount * row + column) + 1024;
//    }
    else
    {
        key = 4 * (m_columnCount * row + column);
    }
    if(!m_resourceFileContentMap->contains(key))
    {
        m_resourceFileContentMap->insert(key, 0);
    }
    unsigned int value = m_resourceFileContentMap->value(key);
    QString str = QString::number(value, 16).toUpper();
    int length = 8 - str.length();
    for(int i = 0; i < length; ++i)
    {
        str.insert(0, "0");
    }

    uint index;
//    if(m_type == "memory32")
//        index = (key - 1024) / 4;
//    else
        index = key / 4;

    if(!m_sboxShowInOneLine && index >= (unsigned int)m_totalCount)
    {
        return "########";
    }
    return str;
//    QMapIterator<unsigned int, unsigned int> itor(*m_resourceFileContentMap);
//    while (itor.hasNext()) {
//        itor.next();
//    }

}

QVariant CResourceEditorTableModel::currencyAtList(int row, int column) const
{
    if(m_resourceFileContentList->count() < 1)
        return QVariant();
    int index = m_columnCount * row + column;
    if(index >= m_resourceFileContentList->count())
    {
        return QString();
    }
    unsigned int value = m_resourceFileContentList->at(index);
    QString str = QString::number(value, 16).toUpper();
    int length = 8 - str.length();
    for(int i = 0; i < length; ++i)
    {
        str.insert(0, "0");
    }
    return str;
}

void CResourceEditorTableModel::getSrcForHardware(uint *&data, uint &len) const
{
    if(m_type == "memory")
    {
        len = (uint)m_resourceFileContentMap->count();
        if(len < 1)
            return;
        data = new uint[len];
        for(uint i = 0; i < len; ++i)
        {
//            Q_ASSERT(m_resourceFileContentMap->contains(i*4));
            uint index = i*4;
//            if(m_type == "memory32")
//                index += 1024;
            if(m_resourceFileContentMap->contains(index))
                data[i] = m_resourceFileContentMap->value(index);
            else
                data[i] = 0;
        }
    }
    else if(m_type == "fifo")
    {
        len = (uint)m_resourceFileContentList->count();
        if(len < 1)
            return;
        data = new uint[len];
        for(uint i = 0; i < len; ++i)
        {
            data[i] = m_resourceFileContentList->at(i);
        }
    }
}
