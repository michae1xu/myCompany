#include <QApplication>
#include <QIcon>
#include <QFileInfo>
#include <QBrush>
#include <QMap>
#include <QMimeData>
#include <QStyleOptionComboBox>
#include "cprojectsettingmodel.h"
#include "common/cappenv.hpp"
#include "common/debug.h"

CProjectSettingModel::CProjectSettingModel(QList<SXmlParameter> *xmlParameterList, QMap<QString, QString> *resourceMap, QObject *parent) :
    QAbstractTableModel(parent),
    m_xmlParameterList(xmlParameterList),
    m_resourceMap(resourceMap)
{
}

CProjectSettingModel::~CProjectSettingModel()
{
}

void CProjectSettingModel::resetData(QList<SXmlParameter> *xmlParameterList, QMap<QString, QString> *resourceMap)
{
    beginResetModel();
    m_xmlParameterList = xmlParameterList;
    m_resourceMap = resourceMap;
    endResetModel();
}

QVariant CProjectSettingModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() ||
            index.row() < 0 || index.row() >= m_xmlParameterList->count() ||
            index.column() < 0 || index.column() > projectSettingLFSRGroup)
    {
        return QVariant();
    }

    if (role == Qt::SizeHintRole)
    {
        QStyleOptionComboBox option;
        if(index.column() > ProjectSettingGap && index.column() < ProjectSettingBurstNum)
        {
            option.currentText = "MemoryPort10000";
            QFontMetrics fontMetrics(data(index, Qt::EditRole).value<QFont>());
            option.fontMetrics = fontMetrics;
            QSize size(fontMetrics.width(option.currentText), fontMetrics.height());
            return qApp->style()->sizeFromContents(QStyle::CT_ComboBox, &option, size);
        }
        if((index.column() > ProjectSettingDest && index.column() <= projectSettingLFSRGroup)
                || index.column() == ProjectSettingGap)
        {
            option.currentText = "000";
            QFontMetrics fontMetrics(data(index, Qt::EditRole).value<QFont>());
            option.fontMetrics = fontMetrics;
            QSize size(fontMetrics.width(option.currentText), fontMetrics.height());
            return qApp->style()->sizeFromContents(QStyle::CT_ComboBox, &option, size);
        }
    }
    else if (role == Qt::TextAlignmentRole)
    {
        if((index.column() >= ProjectSettingXmlName && index.column() < ProjectSettingOnlyRsm)
                || index.column() == ProjectSettingSource || index.column() == ProjectSettingDest)
        {
            return static_cast<int>(Qt::AlignVCenter | Qt::AlignLeft);
        }
        return static_cast<int>(Qt::AlignVCenter | Qt::AlignHCenter);
    }
    else if (role == Qt::ForegroundRole)
    {
        QString fileName = index.data(Qt::EditRole).toString();
        if((index.column() >= ProjectSettingXmlName && index.column() < ProjectSettingOnlyRsm) && !checkFileExists(fileName))
        {
            return QBrush(QColor(255, 0, 0));
        }
        if((index.column() > ProjectSettingXmlName && index.column() < ProjectSettingInMemoryPath0)
                && QFileInfo(fileName).suffix().compare("fifo", Qt::CaseInsensitive) != 0)
        {
            return QBrush(QColor(255, 150, 0));
        }
        if((index.column() > ProjectSettingInFifoPath3 && index.column() < ProjectSettingOnlyRsm)
//                && QFileInfo(fileName).suffix().compare("memory", Qt::CaseInsensitive) != 0
                && QFileInfo(fileName).suffix().compare("memory", Qt::CaseInsensitive) != 0)
        {
            return QBrush(QColor(255, 150, 0));
        }
    }
    else if(role == Qt::ToolTipRole)
    {
        QString fileName = index.data(Qt::EditRole).toString();
        if((index.column() >= ProjectSettingXmlName && index.column() < ProjectSettingOnlyRsm) && !fileName.isEmpty() && !checkFileExists(fileName))
        {
            return tr(u8"错误：文件不存在");
        }
        if((index.column() > ProjectSettingXmlName && index.column() < ProjectSettingInMemoryPath0)
                && !fileName.isEmpty()
                && QFileInfo(fileName).suffix().compare("fifo", Qt::CaseInsensitive) != 0)
        {
            return tr(u8"告警：文件类型错误");
        }
        if((index.column() > ProjectSettingInFifoPath3 && index.column() < ProjectSettingOnlyRsm)
                && !fileName.isEmpty()
//                && QFileInfo(fileName).suffix().compare("memory", Qt::CaseInsensitive) != 0
                && QFileInfo(fileName).suffix().compare("memory", Qt::CaseInsensitive) != 0)
        {
            return tr(u8"告警：文件类型错误");
        }
        if(index.column() >= ProjectSettingXmlName && index.column() < ProjectSettingOnlyRsm)
        {
            return fileName;
        }
    }
    else if(role == Qt::DecorationRole)
    {
        QVariant v;
        QString fileName = index.data(Qt::EditRole).toString();
        if((index.column() >= ProjectSettingXmlName && index.column() < ProjectSettingOnlyRsm) && !fileName.isEmpty() && !checkFileExists(fileName))
        {
            QIcon icon(CAppEnv::getImageDirPath() + "/error.png");
            v = icon;
            return v;
        }
        if((index.column() > ProjectSettingXmlName && index.column() < ProjectSettingInMemoryPath0)
                && !fileName.isEmpty()
                && QFileInfo(fileName).suffix().compare("fifo", Qt::CaseInsensitive) != 0)
        {
            QIcon icon(CAppEnv::getImageDirPath() + "/128/warning128.png");
            v = icon;
            return v;
        }
        if((index.column() > ProjectSettingInFifoPath3 && index.column() < ProjectSettingOnlyRsm)
                && !fileName.isEmpty()
//                && QFileInfo(fileName).suffix().compare("memory", Qt::CaseInsensitive) != 0
                && QFileInfo(fileName).suffix().compare("memory", Qt::CaseInsensitive) != 0)
        {
            QIcon icon(CAppEnv::getImageDirPath() + "/128/warning128.png");
            v = icon;
            return v;
        }
    }
    else if (role == Qt::DisplayRole)
    {
        QVariant current = currencyAt(index.row(), index.column());
        if (index.column() >= ProjectSettingXmlName && index.column() < ProjectSettingOnlyRsm)
        {
            QString str = current.toString();
            str = CAppEnv::stpToNm(str);
            current = str;
        }
        return current;
    }
    else if (role == Qt::EditRole)
    {
        QVariant current = currencyAt(index.row(), index.column());
        return current;
    }
    else if (role == Qt::CheckStateRole)
    {
        if (index.column() == ProjectSettingXmlName)
        {
            return m_xmlParameterList->at(index.row()).selected == Qt::Checked ? Qt::Checked : Qt::Unchecked;
        }
    }
//    else if(role == Qt::BackgroundColorRole )
//    {
//        if(index.row() % 2 == 1)
//            return QColor(240, 240, 240);
//    }
    return QVariant();
}

bool CProjectSettingModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() ||
            index.row() < 0 || index.row() >= m_xmlParameterList->count() ||
            index.column() < 0 || index.column() > projectSettingLFSRGroup)
    {
        return false;
    }

    if (role == Qt::EditRole)
    {
        SXmlParameter xmlParameter = m_xmlParameterList->at(index.row());
        switch (index.column()) {
        case ProjectSettingInFifoPath0:
            xmlParameter.inFifoPath0 = value.toString().trimmed();
            break;
        case ProjectSettingInFifoPath1:
            xmlParameter.inFifoPath1 = value.toString().trimmed();
            break;
        case ProjectSettingInFifoPath2:
            xmlParameter.inFifoPath2 = value.toString().trimmed();
            break;
        case ProjectSettingInFifoPath3:
            xmlParameter.inFifoPath3 = value.toString().trimmed();
            break;
        case ProjectSettingInMemoryPath0:
            xmlParameter.inMemPath0 = value.toString().trimmed();
            break;
        case ProjectSettingInMemoryPath1:
            xmlParameter.inMemPath1 = value.toString().trimmed();
            break;
        case ProjectSettingInMemoryPath2:
            xmlParameter.inMemPath2 = value.toString().trimmed();
            break;
        case ProjectSettingInMemoryPath3:
            xmlParameter.inMemPath3 = value.toString().trimmed();
            break;
        case ProjectSettingInMemoryPath4:
            xmlParameter.inMemPath4 = value.toString().trimmed();
            break;
        case ProjectSettingOnlyRsm:
            xmlParameter.onlyRsm = value.toInt();
            break;
        case ProjectSettingRMode0:
            xmlParameter.rMode0 = value.toInt();
            break;
        case ProjectSettingRMode1:
            xmlParameter.rMode1 = value.toInt();
            break;
        case ProjectSettingGap:
            xmlParameter.gap = value.toInt();
            break;
        case ProjectSettingSource:
            xmlParameter.source = value.toString().trimmed();
            break;
        case ProjectSettingDest:
            xmlParameter.dest = value.toString().trimmed();
            break;
        case ProjectSettingBurstNum:
            xmlParameter.burstNum = value.toInt();
            break;
        case ProjectSettingLoopNum:
            xmlParameter.loopNum = value.toInt();
            break;
        case ProjectSettingRepeatNum:
            xmlParameter.repeatNum = value.toInt();
            break;
        case projectSettingLFSRGroup:
            xmlParameter.lfsrGroup = value.toString();
            break;
        default:
            break;
        }

        m_xmlParameterList->replace(index.row(), xmlParameter);
        emit dataChanged(index, index);
        return true;
    }

    if (index.isValid() && role == Qt::CheckStateRole)
    {
        SXmlParameter xmlParameter = m_xmlParameterList->at(index.row());
        if(value == Qt::Checked)
            xmlParameter.selected = Qt::Checked;
        else
            xmlParameter.selected = Qt::Unchecked;

        m_xmlParameterList->replace(index.row(), xmlParameter);
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

int CProjectSettingModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_xmlParameterList->count();
}

int CProjectSettingModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : projectSettingLFSRGroup + 1;
}

QVariant CProjectSettingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if(orientation == Qt::Horizontal)
    {
        switch (section) {
        case ProjectSettingXmlName:
            return tr(u8"XML文件");
            break;
        case ProjectSettingInFifoPath0:
            return tr(u8"IN FIFO 0");
            break;
        case ProjectSettingInFifoPath1:
            return tr(u8"IN FIFO 1");
            break;
        case ProjectSettingInFifoPath2:
            return tr(u8"IN FIFO 2");
            break;
        case ProjectSettingInFifoPath3:
            return tr(u8"IN FIFO 3");
            break;
        case ProjectSettingInMemoryPath0:
            return tr(u8"IN MEM 0");
            break;
        case ProjectSettingInMemoryPath1:
            return tr(u8"IN MEM 1");
            break;
        case ProjectSettingInMemoryPath2:
            return tr(u8"IN MEM 2");
            break;
        case ProjectSettingInMemoryPath3:
            return tr(u8"IN MEM 3");
            break;
        case ProjectSettingInMemoryPath4:
            return tr(u8"IN MEM 4");
            break;
        case ProjectSettingOnlyRsm:
            return tr(u8"OnlySim");
            break;
        case ProjectSettingRMode0:
            return tr(u8"RMode0");
            break;
        case ProjectSettingRMode1:
            return tr(u8"RMode1");
            break;
        case ProjectSettingGap:
            return tr(u8"Gap");
            break;
        case ProjectSettingSource:
            return tr(u8"Source");
            break;
        case ProjectSettingDest:
            return tr(u8"Dest");
            break;
        case ProjectSettingBurstNum:
            return tr(u8"Burst");
            break;
        case ProjectSettingLoopNum:
            return tr(u8"Loop");
            break;
        case ProjectSettingRepeatNum:
            return tr(u8"Repeat");
            break;
        case projectSettingLFSRGroup:
            return tr(u8"LFSR Group");
            break;
        default:
            break;
        }
    }
    else
    {
        return section + 1;
    }
    return QVariant();

}

Qt::ItemFlags CProjectSettingModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);

    if(index.isValid())
    {
        flags |= Qt::ItemIsEnabled;
        flags |= Qt::ItemIsSelectable;
        if(index.column() == ProjectSettingXmlName)
        {
            flags |= Qt::ItemIsUserCheckable|
                     Qt::ItemIsDragEnabled|
                     Qt::ItemIsDropEnabled;
            return flags;
        }
        flags |= Qt::ItemIsEditable;
    }
    return flags;
}

QByteArray intToByte(int i)
{
    QByteArray abyte0;
    abyte0.resize(4);
    abyte0[0] = (uchar)  (0x000000ff & i);
    abyte0[1] = (uchar) ((0x0000ff00 & i) >> 8);
    abyte0[2] = (uchar) ((0x00ff0000 & i) >> 16);
    abyte0[3] = (uchar) ((0xff000000 & i) >> 24);
    return abyte0;
}

int bytesToInt(QByteArray bytes)
{
    int addr = bytes[0] & 0x000000FF;
    addr |= ((bytes[1] << 8) & 0x0000FF00);
    addr |= ((bytes[2] << 16) & 0x00FF0000);
    addr |= ((bytes[3] << 24) & 0xFF000000);
    return addr;
}

QStringList CProjectSettingModel::mimeTypes() const
{
    return QStringList() << MimeType;
}

QMimeData *CProjectSettingModel::mimeData(const QModelIndexList &indexes) const
{
    if (indexes.count() != 1)
        return 0;
    QModelIndex index = indexes.at(0);
    if(index.isValid() && index.column() == 0)
    {
        QMimeData *mimeData = new QMimeData;
        QByteArray projectData;
        projectData.append(intToByte(index.row()));
        mimeData->setData(MimeType, projectData);
        return mimeData;
    }
    return 0;
}

bool CProjectSettingModel::dropMimeData(const QMimeData *mimeData, Qt::DropAction action, int , int , const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
        return true;
    if (action != Qt::MoveAction || !mimeData || !mimeData->hasFormat(MimeType))
        return false;

    if (parent.isValid())
    {
        QByteArray projectData = mimeData->data(MimeType);
        int sourceRow = bytesToInt(projectData);

//        beginRemoveRows(QModelIndex(), sourceRow, sourceRow);
        SXmlParameter xmlParameter = m_xmlParameterList->takeAt(sourceRow);        
//        endRemoveRows();
//        beginInsertRows(QModelIndex(), parent.row(), parent.row());
        m_xmlParameterList->insert(parent.row(), xmlParameter);
        sortXml();
//        endInsertRows();

        emit currentIndex(parent);
        if(sourceRow < parent.row())
            emit dataChanged(index(sourceRow, ProjectSettingXmlName), index(parent.row(), projectSettingLFSRGroup));
        else
            emit dataChanged(index(parent.row(), ProjectSettingXmlName), index(sourceRow, projectSettingLFSRGroup));
        return true;
    }
    return false;
}

QModelIndex CProjectSettingModel::moveUp(const QModelIndex &index)
{
    if (!index.isValid() || index.row() <= 0)
        return index;
    return moveItem(index.row(), index.row() - 1);
}

QModelIndex CProjectSettingModel::moveDown(const QModelIndex &index)
{
    if (!index.isValid() || index.row() >= m_xmlParameterList->count() - 1)
        return index;
    return moveItem(index.row(), index.row() + 1);
}

QModelIndex CProjectSettingModel::moveItem(int oldRow, int newRow)
{
    Q_ASSERT(0 <= oldRow && oldRow < m_xmlParameterList->count() &&
             0 <= newRow && newRow < m_xmlParameterList->count());
    m_xmlParameterList->swap(oldRow, newRow);
    sortXml();
    if(oldRow < newRow)
    {
        QModelIndex oldIndex = createIndex(oldRow, ProjectSettingXmlName);
        QModelIndex newIndex = createIndex(newRow, projectSettingLFSRGroup);
        emit dataChanged(oldIndex, newIndex);
        return createIndex(newRow, ProjectSettingXmlName);
    }
    else
    {
        QModelIndex oldIndex = createIndex(oldRow, projectSettingLFSRGroup);
        QModelIndex newIndex = createIndex(newRow, ProjectSettingXmlName);
        emit dataChanged(newIndex, oldIndex);
        return newIndex;
    }
}

void CProjectSettingModel::selectAll()
{
    beginResetModel();
    QModelIndex indexBegin = createIndex(0, ProjectSettingXmlName);
    QModelIndex indexEnd = createIndex(m_xmlParameterList->count() - 1, ProjectSettingXmlName);
    for(int i = 0; i < m_xmlParameterList->count(); ++i)
    {
        SXmlParameter xmlParameter = m_xmlParameterList->at(i);
        xmlParameter.selected = Qt::Checked;
        m_xmlParameterList->replace(i, xmlParameter);
    }
    emit dataChanged(indexBegin, indexEnd);
    endResetModel();
}

void CProjectSettingModel::refreshModel()
{
    beginResetModel();
    endResetModel();
}



QVariant CProjectSettingModel::currencyAt(int row, int column) const
{
    QVariant v;
    SXmlParameter xmlParameter = m_xmlParameterList->at(row);
    switch (column) {
    case ProjectSettingXmlName:
        v = xmlParameter.xmlPath;
        break;
    case ProjectSettingInFifoPath0:
        v = xmlParameter.inFifoPath0;
        break;
    case ProjectSettingInFifoPath1:
        v = xmlParameter.inFifoPath1;
        break;
    case ProjectSettingInFifoPath2:
        v = xmlParameter.inFifoPath2;
        break;
    case ProjectSettingInFifoPath3:
        v = xmlParameter.inFifoPath3;
        break;
    case ProjectSettingInMemoryPath0:
        v = xmlParameter.inMemPath0;
        break;
    case ProjectSettingInMemoryPath1:
        v = xmlParameter.inMemPath1;
        break;
    case ProjectSettingInMemoryPath2:
        v = xmlParameter.inMemPath2;
        break;
    case ProjectSettingInMemoryPath3:
        v = xmlParameter.inMemPath3;
        break;
    case ProjectSettingInMemoryPath4:
        v = xmlParameter.inMemPath4;
        break;
    case ProjectSettingOnlyRsm:
        v = xmlParameter.onlyRsm;
        break;
    case ProjectSettingRMode0:
        v = xmlParameter.rMode0;
        break;
    case ProjectSettingRMode1:
        v = xmlParameter.rMode1;
        break;
    case ProjectSettingGap:
        v = xmlParameter.gap;
        break;
    case ProjectSettingSource:
        v = xmlParameter.source;
        break;
    case ProjectSettingDest:
        v = xmlParameter.dest;
        break;
    case ProjectSettingBurstNum:
        v = xmlParameter.burstNum;
        break;
    case ProjectSettingLoopNum:
        v = xmlParameter.loopNum;
        break;
    case ProjectSettingRepeatNum:
        v = xmlParameter.repeatNum;
        break;
    case projectSettingLFSRGroup:
        v = xmlParameter.lfsrGroup;
        break;
    default:
        break;
    }
    return v;
}

bool CProjectSettingModel::checkFileExists(const QString fileName) const
{
    if(fileName.startsWith(":"))
    {
        return QFileInfo(m_resourceMap->value(fileName)).exists();
    }
    else
    {
        return QFileInfo(fileName).exists();
    }
}

bool sortASC(SXmlParameter xmlParameter1, SXmlParameter xmlParameter2)
{
    if(xmlParameter1.sort == -1)
        return false;
    if(xmlParameter2.sort == -1)
        return true;
    if(xmlParameter1.sort < xmlParameter2.sort)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void CProjectSettingModel::sortXml()
{
    for(int i = 0; i < m_xmlParameterList->count(); ++i)
    {
        SXmlParameter xmlParameter = m_xmlParameterList->at(i);
        xmlParameter.sort = i;
        m_xmlParameterList->replace(i, xmlParameter);
    }
    qStableSort(m_xmlParameterList->begin(), m_xmlParameterList->end(), sortASC);
}
