#ifndef SOURCEFORM_H
#define SOURCEFORM_H

#include <QDialog>
#include <QMap>

namespace Ui {
class SourceForm;
}

class SourceForm : public QDialog
{
    Q_OBJECT

public:
    explicit SourceForm(QMap<QString, QString> &sourceMap, QWidget *parent = 0);
    ~SourceForm();

signals:
    void choice(QString);

private slots:
    void on_pushButton_choice_clicked();

private:
    Ui::SourceForm *ui;
    QStringList m_listViewList;
    QMap<QString, QString> m_sourceMap;
};

#endif // SOURCEFROM_H
