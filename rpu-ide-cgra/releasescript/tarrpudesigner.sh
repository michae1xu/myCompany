#!/bin/bash
# Program: Package the rpudesignersolution and Ftp the package to server.
# Version: 1.0.0
# Autor: ZhangJun
# History: 2017/8/18 ZhangJun first release.

if [ -z $1 ];then
    echo "Usage: $0 version"
    exit 0
fi
mv ../version.txt version.gbk
iconv -f gbk -t utf8 version.gbk -o ../version.txt
rm ../bin/*_debug
version=$1
#version=`../bin/rpusimforconsole.sh -v | awk '{print $4}'`
datetime=`date +%Y%m%d%H%M%S`
filename="../release/rpudesigner_linux_x86_64_v"$version"_"$datetime".tar.bz2"
tar -jcvf $filename ../../rpudesignersolution --exclude=../../rpudesignersolution/bin/CMDGen.exe --exclude=../../rpudesignersolution/bin/maker.exe --exclude=../../rpudesignersolution/bin/rcs.exe --exclude=../../rpudesignersolution/bin/liboventimer.dll --exclude=../../rpudesignersolution/bin/liboventimerd.dll --exclude=../../rpudesignersolution/bin/rpudesignerd --exclude=../../rpudesignersolution/release --exclude=../../rpudesignersolution/releasescript --exclude=../../rpudesignersolution/src --exclude=../../rpudesignersolution/.svn --exclude=../../rpudesignersolution/log --exclude=../../rpudesignersolution/tmp --exclude=../../rpudesignersolution/var --exclude=*.svn*
rm ../version.txt
mv version.gbk ../version.txt
#ftpToServer.sh $filename
ftpsToServer.py $filename
