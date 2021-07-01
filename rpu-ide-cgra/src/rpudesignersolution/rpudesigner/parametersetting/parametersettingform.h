#ifndef PARAMETERSETTINGFORM_H
#define PARAMETERSETTINGFORM_H

#include <QDialog>
#include <guiviewer.h>
#include "datafile.h"

class CParameterSettingModel;
class SourceForm;

namespace Ui {
class ParameterSettingForm;
}

class ParameterSettingForm : public QDialog
{
    Q_OBJECT

public:
    explicit ParameterSettingForm(QList<SParaInfo *> *list, QMap<QString, QString> &sourceMap, QWidget *parent = 0);
    ~ParameterSettingForm();

public slots:
    void showTableViewContextMenu(QPoint point);
    void sourceActionTriggered();
    void exploreActionTriggered();
    void choiceActionTriggered();
    void choiceAllActionTriggered();
    void upActionTriggered();
    void downActionTriggered();
//    void itemChanged(int row, int column, QString str);
    void setCellText(QString str);

private slots:
    void on_pushButton_down_clicked();

    void on_pushButton_up_clicked();

    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::ParameterSettingForm *ui;

    CParameterSettingModel *m_model;
    QMenu *m_contextMenu;
    QAction *m_exploreAction;
    QAction *m_sourceAction;
    QList<SParaInfo *> *m_paraInfoList;
    QMap<QString, QString> &m_sourceMap;
    SourceForm *m_sourceForm;

    bool checkValue();
//    QString stpToNm(const QString &fullFileName);
    bool checkFileExists(const QString fileName) const;
};

#endif // PARAMETERSETTINGFORM_H
