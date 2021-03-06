TEMPLATE=lib
CONFIG += qt dll qtsingleapplication-buildlib c++11 debug_and_release build_all warn_on
mac:CONFIG += absolute_library_soname
win32|mac:!wince*:!win32-msvc:!macx-xcode:CONFIG += debug_and_release build_all
include(./src/qtsingleapplication.pri)
TARGET = $$QTSINGLEAPPLICATION_LIBNAME
DESTDIR = $$QTSINGLEAPPLICATION_LIBDIR
win32 {
    DLLDESTDIR = $$[QT_INSTALL_BINS]
    QMAKE_DISTCLEAN += $$[QT_INSTALL_BINS]\\$${QTSINGLEAPPLICATION_LIBNAME}.dll
}
linux {
    DLLDESTDIR = $$[QT_INSTALL_BINS]
    QMAKE_DISTCLEAN += $$[QT_INSTALL_BINS]\\$${QTSINGLEAPPLICATION_LIBNAME}.so
}
target.path = $$DESTDIR
INSTALLS += target
