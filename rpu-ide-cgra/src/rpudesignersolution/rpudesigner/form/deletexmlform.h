#ifndef DELETEXMLFORM_H
#define DELETEXMLFORM_H

#include <QDialog>

namespace Ui {
class DeleteXmlForm;
}

class DeleteXmlForm : public QDialog
{
    Q_OBJECT

public:
    explicit DeleteXmlForm(const QString &fileName, QWidget *parent = 0);
    ~DeleteXmlForm();

    bool getChecked() const;

private:
    Ui::DeleteXmlForm *ui;
};

#endif // DELETEXMLFORM_H
