#ifndef CMAKER_H
#define CMAKER_H

#include <QDialog>

namespace Ui {
class CMaker;
}

class CMaker : public QDialog
{
    Q_OBJECT

public:
    explicit CMaker(QWidget *parent = 0);
    ~CMaker();

private slots:
    void on_buttonBox_accepted();

    void on_lineEdit_mcu1_textChanged(const QString &arg1);

    void on_lineEdit_mcu2_textChanged(const QString &arg1);

    void on_lineEdit_mcu3_textChanged(const QString &arg1);

    void on_pushButton_mcu1_clicked();

    void on_pushButton_mcu2_clicked();

    void on_pushButton_mcu3_clicked();

private:
    Ui::CMaker *ui;

    QPalette m_redPlt;
    QPalette m_blackPlt;

    bool checkValue();
    QString decToHex(int value);
};

#endif // CMAKER_H
