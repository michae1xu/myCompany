#include "MainWindow.h"
#include "newBloodDialog.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QAction *newBlood = ui->menubar->addAction("新成员");
    connect(newBlood,SIGNAL(triggered()),this,SLOT(on_action_NewBlood_triggered()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_NewBlood_triggered()
{
    newBloodDialog dlg(this);
    dlg.exec();
}

