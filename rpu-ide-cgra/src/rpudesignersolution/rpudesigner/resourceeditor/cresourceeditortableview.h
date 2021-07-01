#ifndef CRESOURCEEDITORTABLEVIEW_H
#define CRESOURCEEDITORTABLEVIEW_H

#include <QTableView>

class CResourceEditorTableView : public QTableView
{
public:
    explicit CResourceEditorTableView(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *keyEvent);
};

#endif // CRESOURCEEDITORTABLEVIEW_H
