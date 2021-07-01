CONFIG += hardwaredebug-uselib

TEMPLATE += fakelib
greaterThan(QT_MAJOR_VERSION, 5)|\
  if(equals(QT_MAJOR_VERSION, 5):greaterThan(QT_MINOR_VERSION, 4))|\
  if(equals(QT_MAJOR_VERSION, 5):equals(QT_MINOR_VERSION, 4):greaterThan(QT_PATCH_VERSION, 1)) {
    HARDWAREDEBUG_LIBNAME = $$qt5LibraryTarget(hardwaredebug)
} else {
    HARDWAREDEBUG_LIBNAME = $$qtLibraryTarget(hardwaredebug)
}
TEMPLATE -= fakelib

win32{
    HARDWAREDEBUG_LIBDIR = $$PWD/../../../../bin
}
linux{
    HARDWAREDEBUG_LIBDIR = $$PWD/../../../../lib
}
unix:hardwaredebug-uselib:!hardwaredebug-buildlib:QMAKE_RPATHDIR += $$HARDWAREDEBUG_LIBDIR
