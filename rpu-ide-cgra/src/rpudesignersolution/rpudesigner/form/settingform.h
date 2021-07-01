#ifndef SETTINGFORM_H
#define SETTINGFORM_H

#include <QDialog>
#include "datafile.h"

class QDomDocument;

namespace Ui {
class SettingForm;
}

class SettingForm : public QDialog
{
    Q_OBJECT

public:
    explicit SettingForm(SSetting &setting, QWidget *parent = 0);
    ~SettingForm();

private slots:
    void timeOut();
    void sliderValueChanged(int value);

    void on_buttonBox_accepted();

    void on_listWidget_clicked(const QModelIndex &index);

    void on_checkBox_logOut_clicked(bool checked);

    void on_pushButton_lookLog_clicked();

    void on_checkBox_showTab_clicked(bool checked);

    void on_pushButton_defaultProPath_clicked();

    void on_lineEdit_defaultProPath_textChanged(const QString &arg1);

    void on_pushButton_cycleOut_clicked();

//    void on_checkBox_autoSave_clicked();

    void on_checkBox_logDelete_clicked(bool checked);

    void on_checkBox_showGrid_clicked(bool checked);

    void on_comboBox_networkProxy_currentIndexChanged(int index);

    void on_lineEdit_defaultProPath_editingFinished();

    void on_pushButton_customCmdGenDir_clicked();

    void on_lineEdit_customCmdGenDir_textChanged(const QString &arg1);

    void on_lineEdit_customCmdGenDir_editingFinished();

    void on_pushButton_pythonLocate_clicked();

    void on_lineEdit_pythonLocate_textChanged(const QString &arg1);

    void on_lineEdit_pythonLocate_editingFinished();

private:
    Ui::SettingForm *ui;

    SSetting &m_setting;

    QDomDocument *m_document = nullptr;

    bool m_restatFlag = false;
    bool checkSetting() const;

    void getSimSetting();
    void setSimSetting();
};

#endif // SETTINGFORM_H
