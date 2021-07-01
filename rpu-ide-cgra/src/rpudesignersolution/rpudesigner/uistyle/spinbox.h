#ifndef SPINBOX_H
#define SPINBOX_H

#include <QSpinBox>

class SpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit SpinBox(QWidget *parent = nullptr);
    ~SpinBox();

    QAction *getAction();

public slots:
    void updateValue(int value);

private:
    QAction *m_jumpAction;

};

#endif // SPINBOX_H
