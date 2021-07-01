#ifndef CLICENSELINEEDIT_H
#define CLICENSELINEEDIT_H

#include <QLineEdit>

class CLicenseLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    CLicenseLineEdit(QWidget *parent = nullptr);

signals:
    void pasteLicense(QString);

protected:
    void keyPressEvent(QKeyEvent *event);
};

#endif // CLICENSELINEEDIT_H
