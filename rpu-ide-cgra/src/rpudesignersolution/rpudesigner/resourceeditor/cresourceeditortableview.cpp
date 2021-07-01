#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>
#include <QMessageBox>
#include "cresourceeditortableview.h"

CResourceEditorTableView::CResourceEditorTableView(QWidget *parent) :
    QTableView(parent)
{

}

void CResourceEditorTableView::keyPressEvent(QKeyEvent *keyEvent)
{
    if(selectionModel() && !selectionModel()->selectedIndexes().isEmpty())
    {
        if(keyEvent->modifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_C)
        {
            QString str;
            QModelIndexList indexList = selectionModel()->selectedIndexes();
            int topLeftRow = indexList.at(0).row();
            int topLeftcolumn = indexList.at(0).column();
            int currentRow = topLeftRow;
            for(int i = 0; i < indexList.count(); ++i)
            {
                QModelIndex index = indexList.at(i);
                if(currentRow != index.row())
                {
                    str += "\n";
                    currentRow = index.row();
                }
                if(topLeftcolumn != index.column())
                {
                    str += "\t";
                }
                str += index.data(Qt::DisplayRole).toString();

            }
            QApplication::clipboard()->setText(str);
            return;
        }
        else if(keyEvent->modifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_V)
        {
            QString str = QApplication::clipboard()->text();
            QStringList rows = str.split('\n');
            for(int i = 0; i < rows.count(); ++i)
            {
                if(rows.at(i).trimmed().isEmpty())
                {
                    rows.removeAt(i);
                }
            }
            int numRows = rows.count();
            int numColumns = rows.first().count('\t') + 1;
            int curRow = selectionModel()->selection().first().top();
            int curColumn = selectionModel()->selection().first().left();
            int rowCount = model()->rowCount();
            int columnCount = model()->columnCount();

            if (rowCount - curRow < numRows || columnCount - curColumn < numColumns)
            {
                QMessageBox::information(this, qApp->applicationName(),
                                         tr(u8"The information cannot be pasted because the copy "
                                            "and paste areas aren't the same size."));
                return;
            }
            for (int i = 0; i < numRows; ++i)
            {
                QStringList columns = rows[i].split('\t');
                if(columns.count() != numColumns)
                {
                    continue;
                }
                for (int j = 0; j < numColumns; ++j)
                {
                    QString value = columns.at(j);
                    if(value.trimmed().isEmpty())
                    {
                        continue;
                    }
                    model()->setData(model()->index(curRow + i, curColumn + j), value, Qt::EditRole);
                }
            }
            reset();
            return;
        }
        else if(keyEvent->modifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_X)
        {
            QString str;
            QModelIndexList indexList = selectionModel()->selectedIndexes();
            int topLeftRow = indexList.at(0).row();
            int topLeftcolumn = indexList.at(0).column();
            int currentRow = topLeftRow;
            for(int i = 0; i < indexList.count(); ++i)
            {
                QModelIndex index = indexList.at(i);
                if(currentRow != index.row())
                {
                    str += "\n";
                    currentRow = index.row();
                }
                if(topLeftcolumn != index.column())
                {
                    str += "\t";
                }
                str += index.data(Qt::DisplayRole).toString();
                model()->setData(index, 0, Qt::EditRole);

            }
            QApplication::clipboard()->setText(str);
            return;
        }
        else if(keyEvent->key() == Qt::Key_Delete)
        {
            QModelIndexList indexList = selectionModel()->selectedIndexes();
            foreach (QModelIndex index, indexList)
            {
                model()->setData(index, 0, Qt::EditRole);
            }
            return;
        }
    }
    QTableView::keyPressEvent(keyEvent);
}
