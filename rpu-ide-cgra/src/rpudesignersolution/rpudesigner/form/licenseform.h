#ifndef LICENSEFORM_H
#define LICENSEFORM_H

#include <QDialog>

namespace Ui {
class LicenseForm;
}

class LicenseForm : public QDialog
{
    Q_OBJECT

public:
    explicit LicenseForm(QString license, QWidget *parent = 0);
    ~LicenseForm();

private slots:
    void pasteLicense(QString license);

    void on_buttonBox_accepted();

    void on_pushButton_clicked();

    void on_lineEdit_1_textChanged(const QString &arg1);

    void on_lineEdit_2_textChanged(const QString &arg1);

    void on_lineEdit_3_textChanged(const QString &arg1);

    void on_lineEdit_4_textChanged(const QString &arg1);

private:
    Ui::LicenseForm *ui;
};

#endif // LICENSEFORM_H
