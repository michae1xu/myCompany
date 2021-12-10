#ifndef NEWBLOODDIALOG_H
#define NEWBLOODDIALOG_H

#include <QDialog>

namespace Ui {
class newBloodDialog;
}

class newBloodDialog : public QDialog
{
    Q_OBJECT

public:
    explicit newBloodDialog(QWidget *parent = nullptr);
    ~newBloodDialog();

private:
    Ui::newBloodDialog *ui;

private slots:
    void on_buttonBoxOk_clicked();
};

#endif // NEWBLOODDIALOG_H
