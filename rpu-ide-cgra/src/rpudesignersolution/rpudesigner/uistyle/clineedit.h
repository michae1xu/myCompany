#ifndef CLINEEDIT_H
#define CLINEEDIT_H

#include <QLineEdit>

class CLineEdit : public QLineEdit
{
public:
    CLineEdit(QWidget *parent = NULL);

    void appendCmd(const QString &cmd);

protected:
    void keyReleaseEvent(QKeyEvent *event);

private:
    QStringList m_historyCmd;
    int m_cur;
};

#endif // CLINEEDIT_H
