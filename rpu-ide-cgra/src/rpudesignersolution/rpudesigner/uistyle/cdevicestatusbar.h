#ifndef CDEVICESTATUSBAR_H
#define CDEVICESTATUSBAR_H

#include <QWidget>

namespace Ui {
class CDeviceStatusBar;
}

class CDeviceStatusBar : public QWidget
{
    Q_OBJECT

public:
    explicit CDeviceStatusBar(QWidget *parent = nullptr);
    ~CDeviceStatusBar();

signals:
    void serverButtionClicked();

public slots:
    void deviceDisconnected();
    void deviceConnected(const QString &deviceInfo, const QString &deviceVersion);
    void serverConnectStatusChanged(bool status);

private:
    Ui::CDeviceStatusBar *ui;
};

#endif // CDEVICESTATUSBAR_H
