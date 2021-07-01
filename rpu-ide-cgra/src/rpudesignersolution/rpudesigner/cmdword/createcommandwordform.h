#ifndef CREATECOMMANDWORDFORM_H
#define CREATECOMMANDWORDFORM_H

#include <QDialog>
#include "datafile.h"

class CMergeThread;

class CCreateCommandWordModel;

namespace Ui {
class CreateCommandWordForm;
}

class CreateCommandWordForm : public QDialog
{
    Q_OBJECT

public:
    explicit CreateCommandWordForm(QList<SCMDWordInfo *> *cmdWordInfoList, int priority, QWidget *parent = 0);
    ~CreateCommandWordForm();

private slots:
    void on_pushButton_start_clicked();
    void mergeFinish(int exitCode);

private:
    Ui::CreateCommandWordForm *ui;

    QList<SCMDWordInfo *> *m_cmdWordInfoList;
    CCreateCommandWordModel *m_model;
    int m_priority;
    CMergeThread *m_mergeThread;
};

#endif // CREATECOMMANDWORDFORM_H
