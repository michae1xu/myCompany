#ifndef NEWSRCFORM_H
#define NEWSRCFORM_H

#include <QDialog>

class QAbstractButton;

namespace Ui {
class NewSrcForm;
}

class NewSrcForm : public QDialog
{
    Q_OBJECT

public:
    explicit NewSrcForm(QString path, QWidget *parent = 0);
    ~NewSrcForm();

    QString getFilePath() const;
    QString getType() const {return m_type;}

private slots:
    void on_pushButton_clicked();

    void on_lineEdit_path_textChanged(const QString &arg1);

    void on_buttonBox_clicked(QAbstractButton *button);

    void on_lineEdit_name_textChanged(const QString &arg1);


    void on_lineEdit_path_editingFinished();

private:
    Ui::NewSrcForm *ui;
    QString m_defaultPath;
    QString m_type;

    bool checkFileName();
};

#endif // NEWSRCFORM_H
