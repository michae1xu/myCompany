#ifndef CVIEWINFOSTATUSBAR_H
#define CVIEWINFOSTATUSBAR_H

#include <QWidget>

namespace Ui {
class CViewInfoStatusBar;
}

class CViewInfoStatusBar : public QWidget
{
    Q_OBJECT
public:
    explicit CViewInfoStatusBar(QWidget *parent = nullptr);
    ~CViewInfoStatusBar();

public slots:
    void setMousePos(const QPoint &point);
    void setZoomValue(int value);

signals:
    void zoomIn();
    void zoomOut();
    void zoomTo(int value);
    void showAll();
    void zoomNormal();

private slots:
    void on_pushButton_zoomOut_clicked();
    void on_pushButton_zoonIn_clicked();
    void on_pushButton_zoomNormal_clicked();
    void on_pushButton_showAll_clicked();
    void spinBoxValueChanged(int arg1);
    void horizontalSliderValueChanged(int value);

private:
    Ui::CViewInfoStatusBar *ui;
};

#endif // CVIEWINFOSTATUSBAR_H
