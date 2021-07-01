#include "cdevicestatusbar.h"
#include "ui_cdevicestatusbar.h"
#include "common/cappenv.hpp"
#include "common/debug.h"

CDeviceStatusBar::CDeviceStatusBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDeviceStatusBar)
{
    ui->setupUi(this);
//    setFixedHeight(20);
//    setMinimumHeight(20);

    QImage deviceImage;
    deviceImage.load(CAppEnv::getImageDirPath() + "/disconnect.png");
    deviceImage = deviceImage.scaled(14, 14, Qt::KeepAspectRatio);
    ui->label_deviceConnectStatus->setPixmap(QPixmap::fromImage(deviceImage));
    ui->label_deviceConnectStatus->setToolTip(tr(u8"设备未连接"));
//    ui->label_deviceConnectStatus->setFixedWidth(18);

//    QImage serverImage;
//    serverImage.load(CAppEnv::getImageDirPath() + "/128/trafficlight-red128.png");
//    serverImage = serverImage.scaled(10, 10, Qt::KeepAspectRatio);
//    ui->label_serverConnectStatus->setPixmap(QPixmap::fromImage(serverImage));
//    ui->label_serverConnectStatus->setToolTip(tr(u8"连接未就绪"));
    ui->label_serverConnectStatus->setVisible(false);

    ui->pushButton->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/emoticons128.png"));
    ui->pushButton->setToolTip(tr(u8"连接未就绪"));
    ui->pushButton->setVisible(false);
    connect(ui->pushButton, SIGNAL(clicked()), this, SIGNAL(serverButtionClicked()));
}

CDeviceStatusBar::~CDeviceStatusBar()
{
    delete ui;
}

void CDeviceStatusBar::deviceDisconnected()
{
    QImage image;
    image.load(CAppEnv::getImageDirPath() + "/disconnect.png");
    image = image.scaled(14, 14, Qt::KeepAspectRatio);
    ui->label_deviceConnectStatus->setPixmap(QPixmap::fromImage(image));
    ui->label_deviceConnectStatus->setToolTip(tr(u8"设备未连接"));
}

void CDeviceStatusBar::deviceConnected(const QString &deviceInfo, const QString &deviceVersion)
{
    QImage image;
    image.load(CAppEnv::getImageDirPath() + "/connected.png");
    image = image.scaled(14, 14, Qt::KeepAspectRatio);
    ui->label_deviceConnectStatus->setPixmap(QPixmap::fromImage(image));
    ui->label_deviceConnectStatus->setToolTip(tr(u8"设备已连接<br/>"
                                                 "设备信息：%0<br/>"
                                                 "固件版本：%1").arg(deviceInfo).arg(deviceVersion));
}

void CDeviceStatusBar::serverConnectStatusChanged(bool status)
{
//    QImage image;
//    if(status)
//    {
//        qInfo() << tr(u8"C/S线程消息：当前连接状态：连接已就绪");
//        ui->label_serverConnectStatus->setToolTip(tr(u8"连接已就绪"));
//        image.load(CAppEnv::getImageDirPath() + "/128/trafficlight-green128.png");
//    }
//    else
//    {
//        qInfo() << tr(u8"C/S线程消息：当前连接状态：连接未就绪");
//        ui->label_serverConnectStatus->setToolTip(tr(u8"连接未就绪"));
//        image.load(CAppEnv::getImageDirPath() + "/128/trafficlight-red128.png");
//    }
//    image = image.scaled(10, 10, Qt::KeepAspectRatio);
//    ui->label_serverConnectStatus->setPixmap(QPixmap::fromImage(image));

    if(status)
    {
        qInfo() << tr(u8"C/S线程消息：当前连接状态：连接已就绪");
        ui->pushButton->setToolTip(tr(u8"连接已就绪"));
    }
    else
    {
        qInfo() << tr(u8"C/S线程消息：当前连接状态：连接未就绪");
        ui->pushButton->setToolTip(tr(u8"连接未就绪"));
    }
    ui->pushButton->setEnabled(status);
}
