#ifndef CCUSTOMSPINBOX_H
#define CCUSTOMSPINBOX_H

#include <QSpinBox>

class CCustomSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit CCustomSpinBox(QWidget *parent = nullptr);

signals:
    void signalValueChanged(int);

private slots:
    void slotEditingFinished();
};

#endif // CCUSTOMSPINBOX_H
