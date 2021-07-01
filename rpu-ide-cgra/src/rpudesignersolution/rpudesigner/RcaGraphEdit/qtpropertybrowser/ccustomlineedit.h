#ifndef CCUSTOMLINEEDIT_H
#define CCUSTOMLINEEDIT_H

#include <QLineEdit>

class CCustomLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit CCustomLineEdit(QWidget *parent = nullptr);
    ~CCustomLineEdit() = default;

signals:
    void signalTextChanged(QString);

private slots:
    void slotEditingFinished();
};

#endif // CCUSTOMLINEEDIT_H
