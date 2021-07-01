#ifndef RESOURCETYPEFORM_H
#define RESOURCETYPEFORM_H

#include <QDialog>

namespace Ui {
class ResourceTypeForm;
}

class ResourceTypeForm : public QDialog
{
    Q_OBJECT

public:
    explicit ResourceTypeForm(QWidget *parent = 0);
    ~ResourceTypeForm();

    QString getType() const {return m_type;}
    bool getSave() const {return m_save;}

private slots:
    void on_buttonBox_accepted();

private:
    Ui::ResourceTypeForm *ui;

    QString m_type;

    bool m_save = false;
};

#endif // RESOURCETYPEFORM_H
