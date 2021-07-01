#include "resourceselectdialog.h"
#include "ui_resourceselectdialog.h"
#include "RcaGraphScene.h"
#include "fileresolver/cprojectresolver.h"
#include "common/cappenv.hpp"
#include <QListWidgetItem>
#include <QFileIconProvider>
#include <QIcon>

ResourceSelectDialog::ResourceSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResourceSelectDialog)
{
    ui->setupUi(this);
    this->resize(530, 380);
}

ResourceSelectDialog::~ResourceSelectDialog()
{
    delete ui;
}

void ResourceSelectDialog::setScene(RcaGraphScene *curRcaScene)
{
    ui->listWidget->clear();
    m_curRcaScene = curRcaScene;
    QMapIterator<QString, QString> i(m_curRcaScene->getProject()->getProjectParameter()->resourceMap);
    while(i.hasNext())
    {
        i.next();
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
        QString showName = i.key().mid(2);
        item->setSizeHint(QSize(80, 60));
        item->setText(showName);
        item->setData(Qt::UserRole, i.value());
        QIcon icon;
        QString suffix = QFileInfo(i.value()).suffix();
        if(QFileInfo(CAppEnv::getImageDirPath() + QString("/srcico/%0.png").arg(suffix).toLower()).exists())
            icon = QIcon(CAppEnv::getImageDirPath() + QString("/srcico/%0.png").arg(suffix).toLower());
        else
        {
            QFile file(i.value());
            QFileIconProvider fileIcon;
            icon = fileIcon.icon(file);
        }
        item->setIcon(icon);
        item->setToolTip(i.value());
    }
    ui->listWidget->setCurrentItem(ui->listWidget->item(0));
}

void ResourceSelectDialog::on_buttonBox_accepted()
{
    if(ui->listWidget->selectedItems().count() == 1)
        m_selectedResource = m_curRcaScene->getProject()->getProjectParameter()->
                resourceMap.value(":/" + ui->listWidget->selectedItems().at(0)->text());
}

void ResourceSelectDialog::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    if(item)
    {
        m_selectedResource = m_curRcaScene->getProject()->getProjectParameter()->
                resourceMap.value(":/" + item->text());
        accept();
    }
}
