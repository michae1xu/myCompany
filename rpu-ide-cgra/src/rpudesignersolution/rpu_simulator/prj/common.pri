CONFIG += rpusim-uselib

TEMPLATE += fakelib
greaterThan(QT_MAJOR_VERSION, 5)|\
  if(equals(QT_MAJOR_VERSION, 5):greaterThan(QT_MINOR_VERSION, 4))|\
  if(equals(QT_MAJOR_VERSION, 5):equals(QT_MINOR_VERSION, 4):greaterThan(QT_PATCH_VERSION, 1)) {
    RPUSIM_LIBNAME = $$qt5LibraryTarget(rpusim)
} else {
    RPUSIM_LIBNAME = $$qtLibraryTarget(rpusim)
}
TEMPLATE -= fakelib

win32{
    RPUSIM_LIBDIR = $$PWD/../lib
}
linux{
    RPUSIM_LIBDIR = $$PWD/../lib
}
unix:rpusim-uselib:!rpusim-buildlib:QMAKE_RPATHDIR += $$RPUSIM_LIBDIR
