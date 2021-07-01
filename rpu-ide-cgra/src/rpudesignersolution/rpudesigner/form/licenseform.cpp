#include <QSettings>
#include <QTimer>
#include <QCryptographicHash>
#include <QRegExpValidator>
#include <QDate>
#include "version.h"
#include "licenseform.h"
#include "ui_licenseform.h"
#include "common/cappenv.hpp"
#include "common/debug.h"

LicenseForm::LicenseForm(QString license, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LicenseForm)
{
    ui->setupUi(this);

    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::red);
    ui->label_error->setPalette(pe);

    QSettings settingsTry(ENVIRONMENT_KEY, "rpudesignerTry");
    bool showProbate = settingsTry.value("showProbate", true).toBool();
    ui->pushButton->setVisible(showProbate);

    if(!CAppEnv::checkLicense(license, false) && !showProbate
            && CAppEnv::checkLicense(license, true))
    {
        QDate probation = settingsTry.value("probation").toDate();
        qint64 num = QDate::currentDate().daysTo(probation);
        if(num < 1)
            ui->label_error->setText(tr(u8"试用已过期"));
        else
            ui->label_error->setText(tr(u8"试用还剩%0天").arg(num));
    }
    else if(!CAppEnv::checkLicense(license, false) && !showProbate
               && !CAppEnv::checkLicense(license, true))
    {
        ui->label_error->setText(tr(u8"试用已过期"));
    }

    if(license.length() == 25)
    {
        ui->lineEdit_1->setText(license.left(5));
        license = license.mid(5);
        ui->lineEdit_2->setText(license.left(5));
        license = license.mid(5);
        ui->lineEdit_3->setText(license.left(5));
        license = license.mid(5);
        ui->lineEdit_4->setText(license.left(5));
        license = license.mid(5);
        ui->lineEdit_5->setText(license.left(5));
    }
	
    QRegExp regExp("[0-9a-zA-Z]{5}");
    QRegExpValidator *validator = new QRegExpValidator(regExp, this);
    ui->lineEdit_1->setValidator(validator);
    ui->lineEdit_2->setValidator(validator);
    ui->lineEdit_3->setValidator(validator);
    ui->lineEdit_4->setValidator(validator);
    ui->lineEdit_5->setValidator(validator);

    QList<CLicenseLineEdit *> list = findChildren<CLicenseLineEdit *>();
    for (int i = 0; i < list.count(); ++i)
    {
        CLicenseLineEdit *lineEdit = list.at(i);
        connect(lineEdit, SIGNAL(pasteLicense(QString)), this, SLOT(pasteLicense(QString)));
    }

    resize(geometry().size() * CAppEnv::m_dpiFactor);
}

LicenseForm::~LicenseForm()
{
    delete ui;
}

void LicenseForm::pasteLicense(QString license)
{
    if(license.length() == 25)
    {
        ui->lineEdit_1->setText(license.left(5));
        license = license.mid(5);
        ui->lineEdit_2->setText(license.left(5));
        license = license.mid(5);
        ui->lineEdit_3->setText(license.left(5));
        license = license.mid(5);
        ui->lineEdit_4->setText(license.left(5));
        license = license.mid(5);
        ui->lineEdit_5->setText(license.left(5));
    }
    else if(license.length() == 29)
    {
        ui->lineEdit_1->setText(license.left(5));
        license = license.mid(6);
        ui->lineEdit_2->setText(license.left(5));
        license = license.mid(6);
        ui->lineEdit_3->setText(license.left(5));
        license = license.mid(6);
        ui->lineEdit_4->setText(license.left(5));
        license = license.mid(6);
        ui->lineEdit_5->setText(license.left(5));
    }
    else
    {
        ui->label_error->setText(tr(u8"格式错误"));
        QTimer::singleShot(1500, ui->label_error, SLOT(clear()));
    }
}

void LicenseForm::on_buttonBox_accepted()
{
    QString license = ui->lineEdit_1->text().toUpper()
            + ui->lineEdit_2->text().toUpper()
            + ui->lineEdit_3->text().toUpper()
            + ui->lineEdit_4->text().toUpper()
            + ui->lineEdit_5->text().toUpper();

    if(CAppEnv::checkLicense(license, false))
    {
        CAppEnv::m_license = license;
        QSettings settings(ENVIRONMENT_KEY, "rpudesigner");
        settings.setValue("license", license);
        QSettings settingsTry(ENVIRONMENT_KEY, "rpudesignerTry");
        settingsTry.setValue("showProbate", false);
        ui->label_error->setText(tr(u8"已激活"));
        QTimer::singleShot(1000, this, SLOT(accept()));
//        accept();
    }
    else
    {
        ui->label_error->setText(tr(u8"验证错误"));
        QTimer::singleShot(1500, ui->label_error, SLOT(clear()));
    }
}

void LicenseForm::on_pushButton_clicked()
{
    QSettings settings(ENVIRONMENT_KEY, "rpudesigner");
    settings.setValue("license", QString());
    CAppEnv::m_license.clear();
    QSettings settingsTry(ENVIRONMENT_KEY, "rpudesignerTry");
    settingsTry.setValue("probation", QDate::currentDate().addDays(7));
    settingsTry.setValue("showProbate", false);
    accept();
}

void LicenseForm::on_lineEdit_1_textChanged(const QString &arg1)
{
    if(arg1.length() == 5)
    {
        ui->lineEdit_2->setFocus();
    }
}

void LicenseForm::on_lineEdit_2_textChanged(const QString &arg1)
{
    if(arg1.length() == 5)
    {
        ui->lineEdit_3->setFocus();
    }
}

void LicenseForm::on_lineEdit_3_textChanged(const QString &arg1)
{
    if(arg1.length() == 5)
    {
        ui->lineEdit_4->setFocus();
    }
}

void LicenseForm::on_lineEdit_4_textChanged(const QString &arg1)
{
    if(arg1.length() == 5)
    {
        ui->lineEdit_5->setFocus();
    }
}

