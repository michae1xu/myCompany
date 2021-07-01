#-------------------------------------------------
#
# Project created by QtCreator 2017-03-22T17:34:03
#
#-------------------------------------------------

include(common.pri)

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
CONFIG += qt dll hardwaredebug-buildlib c++11 debug_and_release build_all warn_on
#CONFIG += qt dll hardwaredebug-buildlib c++11 warn_on
mac:CONFIG += absolute_library_soname
win32|mac:!wince*:!win32-msvc:!macx-xcode:CONFIG += debug_and_release build_all
TARGET = $$HARDWAREDEBUG_LIBNAME
DESTDIR = $$HARDWAREDEBUG_LIBDIR
win32 {
    DLLDESTDIR = $$[QT_INSTALL_BINS]
    QMAKE_DISTCLEAN += $$[QT_INSTALL_BINS]\\$${HARDWAREDEBUG_LIBNAME}.dll
}
linux {
    DLLDESTDIR = $$[QT_INSTALL_BINS]
    QMAKE_DISTCLEAN += $$[QT_INSTALL_BINS]\\$${HARDWAREDEBUG_LIBNAME}.so
}
target.path = $$DESTDIR


DEFINES += HARDWAREDEBUG_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += chardwaredebug.cpp

HEADERS += chardwaredebug.h\
        hardwaredebug_global.h


unix {
    INSTALLS += target
}

INCLUDEPATH += $$PWD/rcpsdk/include
DEPENDPATH += $$PWD/rcpsdk/include
#linux {
##    LIBS += $$PWD/../../thirdparty/rcpsdk/lib/linux/rcpcrypto.a
##    LIBS += $$PWD/../../../lib/rcpapi.so
#    LIBS += -L$$PWD/../../../../lib/ -l$$qtLibraryTarget(rcpapi)
#    LIBS += -L$$PWD/../../../../lib/ -l$$qtLibraryTarget(dsc)
#}
#win32 {
#    LIBS += -L$$PWD/../../../../bin/ -l$$qtLibraryTarget(rcpapi)
#}

DEFINES += NODEVICE
DEFINES += UNIXLIKE

