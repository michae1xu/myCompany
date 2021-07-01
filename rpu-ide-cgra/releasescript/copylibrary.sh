#!/bin/bash
# Program: Copy qt software libraries.
# Version: 1.0.0
# Autor: ZhangJun
# History: 2017/11/06 ZhangJun first release.

echo "Copy libraries..."

source ~/.profile

QTDIR=/opt/Qt/Qt5.12.10/5.12.10/gcc_64
echo QTDIR=$QTDIR

rm -f libs.tmp
ldd ../bin/rpudesigner | grep Qt >libs.tmp
if [ -r "libs.tmp" ];then
    liblist=$(cut -d' ' -f3 ./libs.tmp)
else
    echo "Can't open file libs.tmp,quit now!"
    rm -f libs.tmp
    exit 1 
fi

for lib in $liblist
do
    echo "copy $lib"
    cp -a $lib* ../lib/
done

# copy libQt5XcbQpa, libQt5DBus, platforms
cp -a $QTDIR/lib/libQt5DBus.so* ../lib/
cp -a $QTDIR/lib/libQt5XcbQpa.so* ../lib/
cp -a $QTDIR/plugins/bearer ../bin/
cp -a $QTDIR/plugins/iconengines ../bin/
cp -a $QTDIR/plugins/imageformats ../bin/
cp -a $QTDIR/plugins/platforms ../bin/
cp -a $QTDIR/plugins/printsupport ../bin/

rm -f libs.tmp

echo "Copy libraries done."
