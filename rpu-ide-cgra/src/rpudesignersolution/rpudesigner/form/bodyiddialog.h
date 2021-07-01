#ifndef BODYIDDIALOG_H
#define BODYIDDIALOG_H

#include <QDialog>
#include "fileresolver/xmlresolver.h"
#include "guiviewer/modulebody.h"
namespace Ui {
class bodyIdDialog;
}

class bodyIdDialog : public QDialog
{
    Q_OBJECT

public:
    explicit bodyIdDialog(ModuleBody* body);
    ModuleBody *body;
    ~bodyIdDialog();
signals:
    void changeID(ModuleBody* body, int bcu, int rcu, int id);
private slots:
    void on_button_accepted();

private:
    Ui::bodyIdDialog *ui;
};

#endif // BODYIDDIALOG_H
