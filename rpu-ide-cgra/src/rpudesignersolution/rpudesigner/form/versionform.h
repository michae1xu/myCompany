#ifndef VERSIONFORM_H
#define VERSIONFORM_H

#include <QDialog>

namespace Ui {
class VersionForm;
}

class VersionForm : public QDialog
{
    Q_OBJECT

public:
    explicit VersionForm(QWidget *parent = 0);
    ~VersionForm();

private:
    Ui::VersionForm *ui;
};

#endif // VERSIONFORM_H
