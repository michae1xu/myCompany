#ifndef COTPDIALOG_H
#define COTPDIALOG_H

#include <QDialog>
#include <QTimer>

class QAbstractButton;

namespace Ui {
class COTPDialog;
}

class COTPDialog : public QDialog
{
    Q_OBJECT

public:
    explicit COTPDialog(QWidget *parent = 0);
    ~COTPDialog();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

    void on_lineEdit_pubKey_textChanged(const QString &arg1);
    void on_lineEdit_encryptKey_textChanged(const QString &arg1);

    void on_lineEdit_pubKey_editingFinished();
    void on_lineEdit_encryptKey_editingFinished();

    void on_pushButton_pubKeyExplorer_clicked();
    void on_pushButton_encryptKeyExplorer_clicked();

    void on_pushButton_refresh_clicked();

    void updateProgressPercent(int percent);


private:
    Ui::COTPDialog *ui;
//    QTimer m_timer;
    bool m_isUpdating;

    bool checkFileName(const QString &str);

    void closeEvent(QCloseEvent *event);
};

#endif // COTPDIALOG_H
