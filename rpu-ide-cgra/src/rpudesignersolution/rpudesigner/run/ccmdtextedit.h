#ifndef CCMDTEXTEDIT_H
#define CCMDTEXTEDIT_H

#include <QTextEdit>

class CCmdTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit CCmdTextEdit(QWidget *parent=0);
    ~CCmdTextEdit();

    QStringList m_historyList;
    int m_deep;

private slots:
    void cursorPositionChanged();
    void selectionChanged();

private:
    void keyPressEvent(QKeyEvent *event);

signals:
    void sendCmd(QString);
    void cmdCancel();
};

#endif // CCMDTEXTEDIT_H
