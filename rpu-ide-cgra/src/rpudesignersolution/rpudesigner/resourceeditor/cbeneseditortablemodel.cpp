#include <QApplication>
#include <QStyleOptionComboBox>
#include "common/debug.h"
#include "cbeneseditortablemodel.h"

CBenesEditorTableModel::CBenesEditorTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    m_valueList = new QList<uint>;
    for(int i = 0; i < 128; ++i)
    {
        m_valueList->append(i);
    }
}

CBenesEditorTableModel::~CBenesEditorTableModel()
{
}

QVariant CBenesEditorTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.column() < 0)
    {
        return QVariant();
    }

    if (role == Qt::SizeHintRole)
    {
        QStyleOptionComboBox option;
        option.currentText = "WWWWW";
        QFontMetrics fontMetrics(data(index, Qt::EditRole).value<QFont>());
        option.fontMetrics = fontMetrics;
        QSize size(fontMetrics.width(option.currentText), fontMetrics.height());
        return qApp->style()->sizeFromContents(QStyle::CT_ComboBox, &option, size);

    }
    else if (role == Qt::TextAlignmentRole)
    {
        return int(Qt::AlignVCenter | Qt::AlignHCenter);
    }
    else if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        return m_valueList->at(index.row() * 16 + index.column());
    }
    else if (role == Qt::ToolTipRole)
    {
        QString str = tr(u8"当前地址：%0<br>交换地址：%2")
                .arg(QString::number(index.row() * 16 + index.column()))
                .arg(data(index, Qt::EditRole).toInt());
        return str;
    }
    else if (role == Qt::ForegroundRole)
    {
        uint value = index.data(Qt::EditRole).toUInt();
        for(int i = 0; i < m_valueList->count(); ++i)
        {
            if(i == index.row() * 16 + index.column())
                continue;
            if(m_valueList->at(i) == value)
            {
                return QBrush(QColor(255, 0, 0));
            }
        }
    }
    else if (role == Qt::FontRole)
    {
        uint value = index.data(Qt::EditRole).toUInt();
        if(value != (uint)index.row() * 16 + index.column())
        {
            QFont font;
            font.setItalic(true);
            return font;
        }

    }
    return QVariant();
}

bool CBenesEditorTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.column() < 0)
    {
        return false;
    }

    if (role == Qt::EditRole)
    {
        m_valueList->replace(16 * index.row() + index.column(), value.toInt());
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

int CBenesEditorTableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 8;
}

int CBenesEditorTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 16;
}

QVariant CBenesEditorTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole)
    {
        if(orientation == Qt::Horizontal)
        {
            return QString("+%0").arg(QString::number(section));
        }
        else
        {
            return QString::number(section * 16);
        }
    }
//    else if(role == Qt::FontRole)
//    {
//        QFont font;
//        font.setBold(true);
//        return font;
//    }
    return QVariant();
}

Qt::ItemFlags CBenesEditorTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    if(index.isValid())
    {
        flags |= Qt::ItemIsEnabled;
        flags |= Qt::ItemIsSelectable;
        flags |= Qt::ItemIsEditable;
    }
    return flags;
}

bool CBenesEditorTableModel::setHeaderData(int, Qt::Orientation, const QVariant, int)
{
    return false;
}

bool CBenesEditorTableModel::checkValue()
{
    QSet<uint> set;
    foreach (uint value, *m_valueList) {
        set << value;
    }
    return set.count() == 128;
}

void CBenesEditorTableModel::setValue(int *out)
{
    beginResetModel();
    m_valueList->clear();
    for(int i = 0; i < 128; ++i)
    {
        m_valueList->append((uint)out[i]);
    }
    endResetModel();
}

void CBenesEditorTableModel::resetValue()
{
    beginResetModel();
    m_valueList->clear();
    for(int i = 0; i < 128; ++i)
    {
        m_valueList->append(i);
    }
    endResetModel();
}
