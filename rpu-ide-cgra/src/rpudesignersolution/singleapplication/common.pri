CONFIG += qtsingleapplication-uselib

TEMPLATE += fakelib
greaterThan(QT_MAJOR_VERSION, 5)|\
  if(equals(QT_MAJOR_VERSION, 5):greaterThan(QT_MINOR_VERSION, 4))|\
  if(equals(QT_MAJOR_VERSION, 5):equals(QT_MINOR_VERSION, 4):greaterThan(QT_PATCH_VERSION, 1)) {
#    QTSINGLEAPPLICATION_LIBNAME = $$qt5LibraryTarget(QtSolutions_SingleApplication-head)
    QTSINGLEAPPLICATION_LIBNAME = $$qt5LibraryTarget(singleapplication)
} else {
#    QTSINGLEAPPLICATION_LIBNAME = $$qtLibraryTarget(QtSolutions_SingleApplication-head)
    QTSINGLEAPPLICATION_LIBNAME = $$qtLibraryTarget(singleapplication)
}
TEMPLATE -= fakelib

win32{
    QTSINGLEAPPLICATION_LIBDIR = $$PWD/../../../bin
}
linux{
    QTSINGLEAPPLICATION_LIBDIR = $$PWD/../../../lib
}
unix:qtsingleapplication-uselib:!qtsingleapplication-buildlib:QMAKE_RPATHDIR += $$QTSINGLEAPPLICATION_LIBDIR
