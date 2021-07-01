#ifndef RESOURCESELECTDIALOG_H
#define RESOURCESELECTDIALOG_H

#include <QDialog>

class RcaGraphScene;
class QListWidgetItem;

namespace Ui {
class ResourceSelectDialog;
}

class ResourceSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResourceSelectDialog(QWidget *parent = 0);
    ~ResourceSelectDialog();

    void setScene(RcaGraphScene* curRcaScene);
    QString m_selectedResource;


private slots:
    void on_buttonBox_accepted();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::ResourceSelectDialog *ui;
    RcaGraphScene* m_curRcaScene;

};

#endif // RESOURCESELECTDIALOG_H
