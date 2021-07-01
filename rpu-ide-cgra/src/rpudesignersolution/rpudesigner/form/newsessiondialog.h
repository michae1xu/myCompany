#ifndef NEWSESSIONDIALOG_H
#define NEWSESSIONDIALOG_H

#include <QDialog>

namespace Ui {
class NewSessionDialog;
}

class NewSessionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewSessionDialog(QWidget *parent = 0);
    ~NewSessionDialog();

    void setDialogTitle(const QString &text);
    void setDefaultSessionName(const QString &getSessionName);
    bool getOpenFlag() const;
    QString getSessionName() const;

private slots:
    void on_pushButton_create_clicked();

    void on_pushButton_createAndOpen_clicked();

private:
    Ui::NewSessionDialog *ui;

    QString m_sessionName;
    bool m_openFlag = false;
};

#endif // NEWSESSIONDIALOG_H
