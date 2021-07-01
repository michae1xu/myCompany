#-------------------------------------------------
#
# Project created by QtCreator 2016-06-07T10:26:17
#
#-------------------------------------------------

include(rpudesigner.pri)

#version check qt
!minQtVersion(5, 7, 0) {
    message("Cannot build Qt Creator with Qt version $${QT_VERSION}.")
    error("Use at least Qt 5.7.0.")
}

QT += core gui xml network concurrent
win32:QT += winextras
qtHaveModule(charts):QT += charts
qtHaveModule(printsupport):QT += printsupport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG  += qt c++11 debug_and_release build_all warn_on
DESTDIR = $$PWD/../../../bin
TARGET = rpudesigner
build_pass:CONFIG(debug, debug|release) {
    unix: TARGET = $$join(TARGET,,,_debug)
    else: TARGET = $$join(TARGET,,,d)
}

TEMPLATE = app
SOURCES += main.cpp \
    fileeditor/cfileeditor.cpp \
    fileresolver/cxmlresolver.cpp \
    mainwindow.cpp \
    common/cappenv.cpp \
    common/cfilename.cpp \
    cmd/ccmdtextedit.cpp \
    connectserver/ctcpclient.cpp \
    fileresolver/cprojectresolver.cpp \
    form/deletexmlform.cpp \
    form/helpform.cpp \
    form/newform.cpp \
    form/newsrcform.cpp \
    form/resourcetypeform.cpp \
    form/settingform.cpp \
    form/versionform.cpp \
    form/waittingform.cpp \
    form/updateprogressform.cpp \
    form/licenseform.cpp \
    form/clicenselineedit.cpp \
    firmware/cchipdebug.cpp \
    firmware/cmaker.cpp \
    firmware/cpemodel.cpp \
    firmware/pe/cpe.cpp \
    firmware/cfirmwareupdatedialog.cpp \
    openedfilelist/clistview.cpp \
    openedfilelist/copenedfilelistmodel.cpp \
    prosetting/cprojectsettingcomboboxdelegate.cpp \
    prosetting/cprojectsettingspinboxdelegate.cpp \
    prosetting/cprojectsettingmodel.cpp \
    prosetting/cresourcelistwidget.cpp \
    prosetting/cprojectsettingwidget.cpp \
    prosetting/cprojectsettingtableview.cpp \
    prosetting/cfilesystemtreeview.cpp \
    projecttree/cprojecttreeitem.cpp \
    projecttree/cprojectfoldernodetreeitem.cpp \
    projecttree/cprojecttreeview.cpp \
    projecttree/csolutionnodetreeitem.cpp \
    projecttree/cprojecttreeitemmodel.cpp \
    resourceeditor/cresourceeditortablemodel.cpp \
    uistyle/cdockwidget.cpp \
    uistyle/clineedit.cpp \
    uistyle/ctitlebar.cpp \
    uistyle/spinbox.cpp \
    uistyle/toolbar.cpp \
    uistyle/cviewinfostatusbar.cpp \
    uistyle/cprogressstatusbar.cpp \
    resourceeditor/cresourceeditortableview.cpp \
    resourceeditor/cbeneseditortablemodel.cpp \
    resourceeditor/cbenesformatconvert.cpp \
    resourceeditor/cbeneseditorspinboxdelegate.cpp \
    resourceeditor/benes_new.cpp \
    uistyle/cdevicestatusbar.cpp \
    uistyle/cwidget.cpp \
    session/csessionmanager.cpp \
    session/csession.cpp \
    form/sessionmanagerform.cpp \
    form/newsessiondialog.cpp \
    session/cdebugsession.cpp \
    form/performancestatisticsform.cpp \
    common/debug.cpp \
    connectserver/cclientdialog.cpp \
    firmware/cotpdialog.cpp \
    uistyle/cwidgettitlebar.cpp \
    uistyle/cnoclosewidget.cpp \
    form/DMSNavigation.cpp


HEADERS  += mainwindow.h \
    common/cappenv.hpp \
    common/cfilename.h \
    connectserver/ctcpclient.h \
    cmd/ccmdtextedit.h \
    datafile.h \
    fileeditor/cfileeditor.h \
    fileresolver/cprojectresolver.h \
    fileresolver/cxmlresolver.h \
    form/deletexmlform.h \
    form/helpform.h \
    form/newform.h \
    form/newsrcform.h \
    form/resourcetypeform.h \
    form/settingform.h \
    form/versionform.h \
    form/waittingform.h \
    form/updateprogressform.h \
    form/licenseform.h \
    form/clicenselineedit.h \
    form/sessionmanagerform.h \
    form/newsessiondialog.h \
    firmware/cchipdebug.h \
    firmware/cmaker.h \
    firmware/cpemodel.h \
    firmware/pe/cpe.h \
    firmware/cfirmwareupdatedialog.h \
    openedfilelist/copenedfilelistmodel.h \
    openedfilelist/clistview.h \
    prosetting/cprojectsettingmodel.h \
    prosetting/cprojectsettingspinboxdelegate.h \
    prosetting/cprojectsettingcomboboxdelegate.h \
    prosetting/cresourcelistwidget.h \
    prosetting/cprojectsettingtableview.h \
    prosetting/cfilesystemtreeview.h \
    prosetting/cprojectsettingwidget.h \
    projecttree/cprojecttreeitem.h \
    projecttree/cprojectfoldernodetreeitem.h \
    projecttree/cprojecttreeview.h \
    projecttree/csolutionnodetreeitem.h \
    projecttree/cprojecttreeitemmodel.h \
    resourceeditor/cresourceeditortableview.h \
    resourceeditor/cresourceeditortablemodel.h \
    resourceeditor/cbeneseditortablemodel.h \
    resourceeditor/cbenesformatconvert.h \
    resourceeditor/cbeneseditorspinboxdelegate.h \
    resourceeditor/benes_new.h \
    uistyle/cdockwidget.h \
    uistyle/clineedit.h \
    uistyle/ctitlebar.h \
    uistyle/spinbox.h \
    uistyle/toolbar.h \
    uistyle/cviewinfostatusbar.h \
    uistyle/cprogressstatusbar.h \
    uistyle/cdevicestatusbar.h \
    uistyle/cwidget.h \
    version.h \
    session/csessionmanager.h \
    session/csession.h \
    session/cdebugsession.h \
    form/performancestatisticsform.h \
    rpuwaveinterface.h \
    common/debug.h \
    connectserver/cclientdialog.h \
    firmware/cotpdialog.h \
    uistyle/cwidgettitlebar.h \
    uistyle/cnoclosewidget.h \
    form/DMSNavigation.h

FORMS    += mainwindow.ui \
    form/deletexmlform.ui \
    form/helpform.ui \
    form/newform.ui \
    form/newsrcform.ui \
    form/resourcetypeform.ui \
    form/settingform.ui \
    form/versionform.ui \
    form/waittingform.ui \
    form/updateprogressform.ui \
    form/licenseform.ui \
    form/sessionmanagerform.ui \
    form/newsessiondialog.ui \
    firmware/cfirmwareupdatedialog.ui \
    firmware/cmaker.ui \
    uistyle/ctitlebar.ui \
    uistyle/cviewinfostatusbar.ui \
    uistyle/cprogressstatusbar.ui \
    uistyle/cdevicestatusbar.ui \
    form/performancestatisticsform.ui \
    connectserver/cclientdialog.ui \
    firmware/cotpdialog.ui

win32{
    LIBS += -L$$PWD/../../../bin/ -l$$qtLibraryTarget(singleapplication)
    LIBS += -L$$PWD/../../../bin/ -l$$qtLibraryTarget(hardwaredebug)
    LIBS += -L$$PWD/../../../bin/ -l$$qtLibraryTarget(rpusim)
}
linux{
    LIBS += -L$$PWD/../../../lib/ -l$$qtLibraryTarget(singleapplication)
    LIBS += -L$$PWD/../../../lib/ -l$$qtLibraryTarget(hardwaredebug)
    LIBS += -L$$PWD/../../../lib/ -l$$qtLibraryTarget(rpusim)
}

include($$PWD/cfgdebuglog/cfgdebuglog.pri)
include($$PWD/RcaGraphEdit/RcaGraphEdit.pri)
include($$PWD/code/code.pri)
include($$PWD/basepe/basepe.pri)

INCLUDEPATH += $$PWD/../singleapplication/src
DEPENDPATH += $$PWD/../singleapplication/src
INCLUDEPATH += $$PWD/../hardwaredebugsolution/hardwaredebug
DEPENDPATH += $$PWD/../hardwaredebugsolution/hardwaredebug
INCLUDEPATH += $$PWD/../rpu_simulator/src
DEPENDPATH += $$PWD/../rpu_simulator/src
INCLUDEPATH += $$PWD/RcaGraphEdit
DEPENDPATH += $$PWD/RcaGraphEdit

INCLUDEPATH += $$PWD/../rpu_simulator/prj/thirdparty/systemc-2.3.3/src/sysc/utils
DEPENDPATH  += $$PWD/../rpu_simulator/prj/thirdparty/systemc-2.3.3/src/sysc/utils
INCLUDEPATH += $$PWD/../rpu_simulator/prj/thirdparty/systemc-2.3.3/src/sysc/kernel
DEPENDPATH  += $$PWD/../rpu_simulator/prj/thirdparty/systemc-2.3.3/src/sysc/kernel
INCLUDEPATH += $$PWD/../rpu_simulator/prj/thirdparty/systemc-2.3.3/src/sysc/communication
DEPENDPATH  += $$PWD/../rpu_simulator/prj/thirdparty/systemc-2.3.3/src/sysc/communication
INCLUDEPATH += $$PWD/../rpu_simulator/prj/thirdparty/systemc-2.3.3/src/sysc/datatypes/int
DEPENDPATH  += $$PWD/../rpu_simulator/prj/thirdparty/systemc-2.3.3/src/sysc/datatypes/int
INCLUDEPATH += $$PWD/../rpu_simulator/prj/thirdparty/systemc-2.3.3/src
DEPENDPATH  += $$PWD/../rpu_simulator/prj/thirdparty/systemc-2.3.3/src

#DEFINES += ZJ_NO_DEBUG_OUTPUT
DEFINES += LK_NO_DEBUG_OUTPUT
DEFINES += USE_CUSTOM_DIR_MODEL
DEFINES += GUI
DEFINES += USE_DLL_MODE
DEFINES += EVALUATION_VERSION

CONFIG(release, debug|release) {
    DEFINES -= CUSTOM_ANIMATION
    DEFINES += RELEASE
    DEFINES += QT_NO_DEBUG_OUTPUT
    DEFINES -= ZJ_NO_DEBUG_OUTPUT
    DEFINES -= LK_NO_DEBUG_OUTPUT
}

RESOURCES += \
    images.qrc


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../bin/ -lqscintilla2_qt5
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../bin/ -lqscintilla2_qt5d
else:unix:!macx: LIBS += -L$$PWD/../../../lib/ -lqscintilla2_qt5

INCLUDEPATH += $$PWD/thirdparty/QScintilla_gpl-2.11.1/Qt4Qt5
DEPENDPATH += $$PWD/thirdparty/QScintilla_gpl-2.11.1/Qt4Qt5

win32{
    RC_FILE += ICO.rc
#    QMAKE_LFLAGS += /MANIFESTUAC:"level='requireAdministrator'uiAccess='false'"
}

#message(inplace INCLUDEPATH: $$INCLUDEPATH)


