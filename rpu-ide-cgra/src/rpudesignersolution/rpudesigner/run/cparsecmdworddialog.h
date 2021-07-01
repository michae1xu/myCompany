#ifndef CPARSECMDWORDDIALOG_H
#define CPARSECMDWORDDIALOG_H

#include <QDialog>

class QLineEdit;

namespace Ui {
class CParseCmdWordDialog;
}

class CParseCmdWordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CParseCmdWordDialog(QString defaultPath, QWidget *parent = 0);
    ~CParseCmdWordDialog();

    QString getFileName() const;
    bool openSln();

private slots:
    void on_pushButton_slnPath_clicked();

    void on_lineEdit_slnPath_textChanged(const QString &arg1);

    void on_lineEdit_slnPath_editingFinished();

    void on_lineEdit_slnName_textChanged(const QString &arg1);

    void on_pushButton_cfgFileName_clicked();

    void on_pushButton_sboxFileName_clicked();

    void on_lineEdit_cfgFileName_textChanged(const QString &arg1);

    void on_lineEdit_cfgFileName_editingFinished();

    void on_lineEdit_sboxFileName_textChanged(const QString &arg1);

    void on_lineEdit_sboxFileName_editingFinished();

    void on_pushButton_clicked();

private:
    Ui::CParseCmdWordDialog *ui;
    QString m_defaultPath;
    QString m_suffix;
    QString m_fileName;
    QString m_path;
    QString m_baseName;
    bool checkFileName();

    void lineEditTextChanged(QLineEdit *lineEdit, const QString &str, bool checkDir = false);
    void lineEditEditingFinished(QLineEdit *lineEdit);
};

#endif // CPARSECMDWORDDIALOG_H
