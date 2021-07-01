#ifndef NEWFORM_H
#define NEWFORM_H

#include <QDialog>
#include "datafile.h"

class QAbstractButton;

namespace Ui {
class NewForm;
}

enum ENewFileType{NEWSOLUTION, NEWPROJECT, NEWXML};

class NewForm : public QDialog
{
    Q_OBJECT

public:
    explicit NewForm(QString path, ENewFileType fileType, QWidget *parent = nullptr, ECipherType cipherType = CIPHER_TYPE_NONE);
    ~NewForm();

    QString getFileName() const {return m_fileName;}
    ECipherType getCipherType() const {return m_cipherType;}

private slots:
    void on_pushButton_clicked();

    bool on_lineEdit_path_textChanged(const QString &arg1);

    void on_buttonBox_clicked(QAbstractButton *button);

    void on_lineEdit_name_textChanged(const QString &arg1);

    void on_lineEdit_path_editingFinished();

private:
    Ui::NewForm *ui;
    QString m_defaultPath;
    QString m_suffix;
    QString m_fileName;
    QString m_path;
    QString m_baseName;
    ECipherType m_cipherType;
    ENewFileType m_fileType;

    bool checkFileName();
};

#endif // NEWFORM_H
