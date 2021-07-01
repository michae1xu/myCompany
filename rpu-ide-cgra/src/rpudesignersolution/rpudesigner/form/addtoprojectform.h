#ifndef ADDTOPROJECTFORM_H
#define ADDTOPROJECTFORM_H

#include <QDialog>

namespace Ui {
class AddToProjectForm;
}

class AddToProjectForm : public QDialog
{
    Q_OBJECT

public:
    explicit AddToProjectForm(QWidget *parent = 0);
    ~AddToProjectForm();

private:
    Ui::AddToProjectForm *ui;
};

#endif // ADDTOPROJECTFORM_H
