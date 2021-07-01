CONFIG += rcpapi-uselib

TEMPLATE += fakelib
greaterThan(QT_MAJOR_VERSION, 5)|\
  if(equals(QT_MAJOR_VERSION, 5):greaterThan(QT_MINOR_VERSION, 4))|\
  if(equals(QT_MAJOR_VERSION, 5):equals(QT_MINOR_VERSION, 4):greaterThan(QT_PATCH_VERSION, 1)) {
    RCPAPI_LIBNAME = $$qt5LibraryTarget(rcpapi)
} else {
    RCPAPI_LIBNAME = $$qtLibraryTarget(rcpapi)
}
TEMPLATE -= fakelib

win32{
    RCPAPI_LIBDIR = $$PWD/../../../../bin
}
linux{
    RCPAPI_LIBDIR = $$PWD/../../../../lib
}
unix:rcpapi-uselib:!rcpapi-buildlib:QMAKE_RPATHDIR += $$RCPAPI_LIBDIR
