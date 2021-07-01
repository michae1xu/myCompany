#-------------------------------------------------
#
# Project created by QtCreator 2017-12-12T15:03:17
#
#-------------------------------------------------

include(common.pri)

QT       -= core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
CONFIG += qt dll rcpapi-buildlib c++11 debug_and_release build_all warn_on
#CONFIG += qt dll rcpapi-buildlib c++11 warn_on
mac:CONFIG += absolute_library_soname
win32|mac:!wince*:!win32-msvc:!macx-xcode:CONFIG += debug_and_release build_all
TARGET = $$RCPAPI_LIBNAME
DESTDIR = $$RCPAPI_LIBDIR
win32 {
    DLLDESTDIR = $$[QT_INSTALL_BINS]
    QMAKE_DISTCLEAN += $$[QT_INSTALL_BINS]\\$${RCPAPI_LIBNAME}.dll
}
target.path = $$DESTDIR


DEFINES += RCPAPI_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    api_comm.c \
    api_debug.c \
    common.c \
    rcpdevice.c

HEADERS += \
    api_comm.h \
    api_crypto.h \
    api_debug.h \
    api_device.h \
    common.h \
    rcp.h \
    token.h \
    token_helper.h \
    types.h \
    version.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    common.pri
