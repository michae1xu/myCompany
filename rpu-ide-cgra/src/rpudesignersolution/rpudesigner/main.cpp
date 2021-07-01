/********************************************************************
 * Copyright (c) 2016 Tsinghua-wx.inc
 * All rights reserved.
 *
 * \file    main.cpp
 * \brief
 * \version 1.1
 * \author  Zhang Jun
 * \Email   zhangjun@tsinghua-wx.org
 * \date    2016-10-13
 ********************************************************************
 */
#include <QMessageBox>
#include <QSplashScreen>
#include <QHostInfo>
#include <QCommandLineParser>
#include <QCommandLineOption>
//#include <QStyleFactory>
#include "qtsingleapplication.h"
#include "mainwindow.h"
#include "version.h"
#include "form/licenseform.h"
#include "common/cappenv.hpp"
#include "common/debug.h"

static const char *setHighDpiEnvironmentVariable()
{
    const char* envVarName = 0;
#if defined(Q_OS_WIN)
    static const char ENV_VAR_QT_DEVICE_PIXEL_RATIO[] = "QT_DEVICE_PIXEL_RATIO";
    if (!qEnvironmentVariableIsSet(ENV_VAR_QT_DEVICE_PIXEL_RATIO) // legacy in 5.6, but still functional
            && !qEnvironmentVariableIsSet("QT_AUTO_SCREEN_SCALE_FACTOR")
            && !qEnvironmentVariableIsSet("QT_SCALE_FACTOR")
            && !qEnvironmentVariableIsSet("QT_SCREEN_SCALE_FACTORS")) {
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    }
#endif
    return envVarName;
}


//static void setHighDpiEnvironmentVariable()
//{

//    if (Utils::HostOsInfo().isMacHost())
//        return;

//    std::unique_ptr<QSettings> settings(createUserSettings());

//    const bool defaultValue = Utils::HostOsInfo().isWindowsHost();
//    const bool enableHighDpiScaling = settings->value("Core/EnableHighDpiScaling", defaultValue).toBool();

//    static const char ENV_VAR_QT_DEVICE_PIXEL_RATIO[] = "QT_DEVICE_PIXEL_RATIO";
//    if (enableHighDpiScaling
//        && !qEnvironmentVariableIsSet(ENV_VAR_QT_DEVICE_PIXEL_RATIO) // legacy in 5.6, but still functional
//        && !qEnvironmentVariableIsSet("QT_AUTO_SCREEN_SCALE_FACTOR")
//        && !qEnvironmentVariableIsSet("QT_SCALE_FACTOR")
//        && !qEnvironmentVariableIsSet("QT_SCREEN_SCALE_FACTORS")) {
//        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//    }
//}

/*!
 * \brief qMain
 * \param argc
 * \param argv
 * \return
 * \note  main
 */
int main(int argc, char *argv[])
{
//    fstream *log_file = new fstream("D:/TEXT.TXT",ios_base::out);
//    if(log_file->is_open())
//    {
//        for(int i = 0; i < argc; i++)
//        {
//            printf("%s\n", argv[i]);
//            string s = argv[i];
//            if(log_file && log_file->is_open())
//            {
//                (*log_file) << s << "\n";
//            }
//        }
//    }
//    log_file->close();
//    delete log_file;

//    QtSingleApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
//    float f = 1.5;
//    QString s;
//    s.sprintf("%f", f);
//    QByteArray  b = s.toStdString().c_str();
//    qputenv("QT_SCALE_FACTOR", b);
//    const char *highDpiEnvironmentVariable = setHighDpiEnvironmentVariable();

    QtSingleApplication a(QLatin1String(VER_FILEDESCRIPTION_STR) + QLatin1String(" for ") + QLatin1String(VER_PROJECTDESCRIPTION_STR), argc, argv);
//    if (highDpiEnvironmentVariable)
//        qunsetenv(highDpiEnvironmentVariable);
//    QApplication a(argc, argv);

    a.setAttribute(Qt::AA_UseHighDpiPixmaps);
    a.setQuitOnLastWindowClosed(false);

#ifdef RELEASE
    a.setApplicationName(VER_FILEDESCRIPTION_STR);
#else
    a.setApplicationName(QString(VER_FILEDESCRIPTION_STR) + "d");
#endif
    a.setApplicationVersion(VER_PRODUCTVERSION_STR);
    a.setOrganizationName(VER_COMPANYNAME_STR);
    a.setOrganizationDomain(VER_COMPANYDOMAIN_STR);

    QCommandLineParser parser;
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsCompactedShortOptions);
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption client(QStringList() << "client", u8"Attempt to start a new program without check already running first instance.");
    parser.addOption(client);
    QCommandLineOption hidpi(QStringList() << "hidpi", u8"Enable high DPI device,default is enabled.", "enable/disable", "enable");
    parser.addOption(hidpi);
    QCommandLineOption admin(QStringList() << "admin", u8"Run as admin user.");
    parser.addOption(admin);
    parser.process(a);

    if(parser.value(hidpi) == "enable")
    {
        const char *highDpiEnvironmentVariable = setHighDpiEnvironmentVariable();
        if (highDpiEnvironmentVariable)
            qunsetenv(highDpiEnvironmentVariable);
    }

    if (parser.isSet(admin))
    {
        CAppEnv::m_adminUser = true;
    }
    else
    {
        CAppEnv::m_adminUser = false;
    }

#if defined(Q_OS_WIN)
    CAppEnv::m_caseSensitivity = Qt::CaseInsensitive;
#elif defined(Q_OS_UNIX)
    CAppEnv::m_caseSensitivity = Qt::CaseSensitive;
#endif

    CAppEnv::generateDpiFactor();

    if (a.isRunning() && !parser.isSet(client))
    {
        if(parser.positionalArguments().count() > 0)
        {
            foreach (QString fileName, parser.positionalArguments()) {
                a.sendMessage(fileName);
            }
        }
        else
        {
            QMessageBox::information(nullptr, qApp->applicationName(), QObject::tr(u8"RPU Designer 已在运行！"), QMessageBox::Ok);
        }
        return EXIT_SUCCESS;
    }

    QSplashScreen *splash = new QSplashScreen;
    if(qMax(CAppEnv::m_xDpiFactor, CAppEnv:: m_yDpiFactor) > 1.3)
        splash->setPixmap(QPixmap(":images/splash-hidpi.png"));
    else
        splash->setPixmap(QPixmap(":images/splash.png"));
    splash->show();

    Qt::Alignment topRight = Qt::AlignLeft | Qt::AlignBottom;
    splash->showMessage(QObject::tr(u8"正在初始化程序环境……"), topRight, Qt::gray);

    if(!CAppEnv::associateFileTypes())
    {
        qCritical() << QObject::tr(u8"程序环境校验未通过，程序退出");
        QMessageBox::critical(nullptr, QObject::tr(u8"错误"), QObject::tr(u8"程序遇到问题：未能关联解决方案文件，重新安装解决此问题！"));
        exit(EXIT_FAILURE);
    }

    CAppEnv::readSetting();
    try {
        CAppEnv::clcTmpDir();
        CAppEnv::checkDirEnv();
    } catch (CError &error) {
        qCritical() << QObject::tr(u8"程序环境校验未通过，程序退出");
        QMessageBox::critical(nullptr, QObject::tr(u8"错误"), QObject::tr(u8"程序遇到问题：%0，重新安装解决此问题！")
                .arg(QString::fromUtf8(error.what())));
        exit(EXIT_FAILURE);
    }

#ifdef RELEASE
    try {
        CAppEnv::initLog();
    } catch (CError &error) {
        QMessageBox::warning(nullptr, QObject::tr(u8"错误"), QObject::tr(u8"程序遇到问题：%0")
                .arg(QString::fromUtf8(error.what())));
    }
//    if(CAppEnv::m_logOut)
//    {
//        QFile file(CAppEnv::m_sysLog);
//        Q_ASSERT(file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text));
//        textStream.setDevice(&file);
//        qInstallMessageHandler(messageOutput);
//    }
#endif

    qInfo() << QObject::tr(u8"程序启动");
    qInfo() << QObject::tr(u8"正在初始化程序环境");

    MainWindow w;
    CAppEnv::m_mainWgt = &w;
    a.setActivationWindow(&w, true);
    QObject::connect(&a, SIGNAL(messageReceived(const QString&)), &w, SLOT(handleMessage(const QString&)));
    splash->showMessage(QObject::tr(u8"正在连接到服务端，请稍后……"), topRight, Qt::gray);
//    QApplication::setStyle(QStyleFactory::create("Fusion"));
    w.connectToServer();
    w.show();

    if(parser.positionalArguments().count() > 0)
    {
        QStringList proFileNameList;
        QStringList xmlFileNameList;
        foreach (QString fileName, parser.positionalArguments()) {
//            QString fileName = QString::fromLocal8Bit(argv[i]);
//#if defined(Q_OS_WIN)
//            fileName.replace("\\", "/");
//#endif
            if(fileName.endsWith(".rpusln", Qt::CaseInsensitive))
            {
                proFileNameList.append(fileName);
            }
            else if(fileName.endsWith(".xml", Qt::CaseInsensitive))
            {
                xmlFileNameList.append(fileName);
            }
        }
        w.loadRpuSln(proFileNameList);
        w.loadFiles(xmlFileNameList);

    }
    else
    {
        w.restoreSession();
    }

    splash->finish(&w);
    splash->deleteLater();
    w.checkLicense();
    w.activateWindow();
    return a.exec();
}
