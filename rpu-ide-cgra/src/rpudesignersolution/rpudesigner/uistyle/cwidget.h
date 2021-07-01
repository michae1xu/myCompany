#ifndef CWIDGET_H
#define CWIDGET_H

#include <QWidget>

class CWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CWidget(QWidget *parent = NULL);

private:
    bool m_moveFlag;
    QPoint m_mainPos;
    QPoint m_mousePos;

    void mousePressEvent(QMouseEvent *mouseEvent);
    void mouseReleaseEvent(QMouseEvent *mouseEvent);
    void mouseMoveEvent(QMouseEvent *mouseEvent);
};

#endif // CWIDGET_H
