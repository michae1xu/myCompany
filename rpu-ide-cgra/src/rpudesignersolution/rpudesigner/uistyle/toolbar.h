#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QToolBar>
#include "datafile.h"

class QComboBox;
class SpinBox;

class ToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit ToolBar(QWidget *parent = nullptr);
    ~ToolBar();

    SpinBox *getSpinBox() const;
    QComboBox *getComboBox() const;
    QStringList getComboBoxStrList() const;
//    void setParaInfo(const QStringList &strList);

signals:
//    void toolBarClicked();

private:
    //悬浮窗拖动
//    bool m_moveFlag;
//    bool m_moved;
    QPoint m_mainPos;
    QPoint m_mousePos;
    SpinBox *m_spinBox = nullptr;
    QComboBox *m_comboBox = nullptr;
    QStringList m_comboBoxStrList;
//    QList<SParaInfo *> *m_paraInfoList;

//protected:
//    void mousePressEvent(QMouseEvent *mouseEvent);
//    void mouseReleaseEvent(QMouseEvent *mouseEvent);
//    void mouseMoveEvent(QMouseEvent *mouseEvent);

};

#endif // TOOLBAR_H
