#ifndef SESSIONMANAGERFORM_H
#define SESSIONMANAGERFORM_H

#include <QDialog>

class CSessionManager;
class CSession;

namespace Ui {
class SessionManagerForm;
}

class SessionManagerForm : public QDialog
{
    Q_OBJECT

public:
    explicit SessionManagerForm(CSessionManager *manager, QWidget *parent = 0);
    ~SessionManagerForm();

private slots:
    void on_pushButton_new_clicked();

    void on_pushButton_clone_clicked();

    void on_pushButton_rename_clicked();

    void on_pushButton_delete_clicked();

    void on_pushButton_changeTo_clicked();

    void on_pushButton_close_clicked();

    void on_tableWidget_session_cellClicked(int row, int column);

    void on_tableWidget_session_cellDoubleClicked(int, int);

private:
    Ui::SessionManagerForm *ui;

    CSessionManager *m_sessionManager = nullptr;

    void addTableWidgetRow(CSession *session);
    void setCurrentSession(const CSession *session);
    void getAvailableSessionName(QString &sessionName);
    bool isAvailableSessionName(const QString &sessionName) const;
};

#endif // SESSIONMANAGERFORM_H
