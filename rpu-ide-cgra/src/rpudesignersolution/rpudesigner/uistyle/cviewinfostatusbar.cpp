#include "cviewinfostatusbar.h"
#include "ui_cviewinfostatusbar.h"
#include "common/cappenv.hpp"
#include "common/debug.h"

CViewInfoStatusBar::CViewInfoStatusBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CViewInfoStatusBar)
{
    ui->setupUi(this);
    setFixedHeight(20);
    ui->horizontalSlider_zoom->setFixedSize(150, 12);

    QImage image;
    image.load(CAppEnv::getImageDirPath() + "/128/rulers128_1.png");
    image = image.scaled(14, 14, Qt::KeepAspectRatio);
    ui->label_pic->setPixmap(QPixmap::fromImage(image));

    ui->spinBox_zoom->setFixedHeight(18);
    ui->spinBox_zoom->setButtonSymbols(QAbstractSpinBox::NoButtons);
//    ui->spinBox_zoom->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

    ui->pushButton_showAll->setIcon(QIcon(CAppEnv::getImageDirPath() + "/showall.png"));
    ui->pushButton_zoomNormal->setIcon(QIcon(CAppEnv::getImageDirPath() + "/nozoom.png"));
    ui->pushButton_zoomOut->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/Zoom-out128.png"));
    ui->pushButton_zoonIn->setIcon(QIcon(CAppEnv::getImageDirPath() + "/128/zoom-in128.png"));
    QSize size(14, 14);
    ui->pushButton_showAll->setIconSize(size);
    ui->pushButton_zoomNormal->setIconSize(size);
    ui->pushButton_zoomOut->setIconSize(size);
    ui->pushButton_zoonIn->setIconSize(size);

    connect(ui->horizontalSlider_zoom, SIGNAL(valueChanged(int)), SLOT(horizontalSliderValueChanged(int)));
    connect(ui->spinBox_zoom, SIGNAL(valueChanged(int)), this, SLOT(spinBoxValueChanged(int)));
    this->setEnabled(false);
}

CViewInfoStatusBar::~CViewInfoStatusBar()
{
    delete ui;
}

void CViewInfoStatusBar::setZoomValue(int value)
{
    disconnect(ui->horizontalSlider_zoom, SIGNAL(valueChanged(int)), this, SLOT(horizontalSliderValueChanged(int)));
    disconnect(ui->spinBox_zoom, SIGNAL(valueChanged(int)), this, SLOT(spinBoxValueChanged(int)));
    ui->spinBox_zoom->setValue(value);
    int percentValue = 50;
    if(value < 100) {
        percentValue = qRound((double)(value - 10) / 90 * 50);
    } else if(value > 100) {
        percentValue = qRound((double)(value - 100) / 400 * 50 + 50);
    }
    ui->horizontalSlider_zoom->setValue(percentValue);
    connect(ui->horizontalSlider_zoom, SIGNAL(valueChanged(int)), this, SLOT(horizontalSliderValueChanged(int)));
    connect(ui->spinBox_zoom, SIGNAL(valueChanged(int)), this, SLOT(spinBoxValueChanged(int)));
}

void CViewInfoStatusBar::setMousePos(const QPoint &point)
{
    ui->label_x->setText(QString::number(point.x()));
    ui->label_y->setText(QString::number(point.y()));
}

void CViewInfoStatusBar::on_pushButton_zoomOut_clicked()
{
    emit zoomOut();
}

void CViewInfoStatusBar::on_pushButton_zoonIn_clicked()
{
    emit zoomIn();
}

void CViewInfoStatusBar::on_pushButton_zoomNormal_clicked()
{
    emit zoomNormal();
}

void CViewInfoStatusBar::on_pushButton_showAll_clicked()
{
    emit showAll();
}

void CViewInfoStatusBar::horizontalSliderValueChanged(int value)
{
    int zoomValue = 100;
    if(value < 50) {
        zoomValue = (double)value / 50 * 90 + 10;
    } else if(value > 50) {
        zoomValue = (double)(value - 50) / 50 * 400 + 100 ;
    }
    emit zoomTo(zoomValue);
}

void CViewInfoStatusBar::spinBoxValueChanged(int arg1)
{
    emit zoomTo(arg1);
}
