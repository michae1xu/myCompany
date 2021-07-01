#ifndef CFIRMWAREUPDATEDIALOG_H
#define CFIRMWAREUPDATEDIALOG_H

#include <QDialog>
#include <QTimer>

class QAbstractButton;

namespace Ui {
class CFirmwareUpdateDialog;
}

class CFirmwareUpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CFirmwareUpdateDialog(QWidget *parent = 0);
    ~CFirmwareUpdateDialog();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

    void on_lineEdit_textChanged(const QString &arg1);

    void on_lineEdit_editingFinished();

    void updateProgressPercent(int percent);

    void on_pushButton_explorer_clicked();

    void on_pushButton_refresh_clicked();

private:
    Ui::CFirmwareUpdateDialog *ui;
//    QTimer m_timer;
    bool m_isUpdating;

    bool checkFileName(const QString &str);

    void closeEvent(QCloseEvent *event);
};

#endif // CFIRMWAREUPDATEDIALOG_H
