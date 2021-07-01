#!/bin/bash
# Program: Create rpudesigner installer.
# Version: 1.0.0
# Autor: ZhangJun
# History: 2017/11/06 ZhangJun first release.

if [ -z $1 ];then
	echo -n  "please input the rpu designer version: "
	read  version
	if [ -z $version ];then
		echo "Usage: $0 version"
		exit 0
	fi
else
	version=$1
fi

./copylibrary.sh

echo "Package rpudesigner..."

rm -rf ./ifw/packages/com.tsinghua.main/data
rm -rf ./ifw/packages/com.tsinghua.rpudesigner/data
#rm -rf ./ifw/packages/com.tsinghua.rpusimulator/data
#rm -rf ./ifw/packages/com.tsinghua.firmwareupdate/data
mkdir -p ./ifw/packages/com.tsinghua.main/data/bin
mkdir -p ./ifw/packages/com.tsinghua.rpudesigner/data/bin
mkdir -p ./ifw/packages/com.tsinghua.rpudesigner/data/lib
#mkdir ./ifw/packages/com.tsinghua.rpusimulator/data
#mkdir ./ifw/packages/com.tsinghua.firmwareupdate/data

# com.tsinghua.main核心组件
cp -ra ../bin/bearer       ./ifw/packages/com.tsinghua.main/data/bin/
cp -ra ../bin/iconengines  ./ifw/packages/com.tsinghua.main/data/bin/
cp -ra ../bin/imageformats ./ifw/packages/com.tsinghua.main/data/bin/
cp -ra ../bin/platforms    ./ifw/packages/com.tsinghua.main/data/bin/
cp -ra ../bin/printsupport ./ifw/packages/com.tsinghua.main/data/bin/
cp -ra ../bin/LICENSE      ./ifw/packages/com.tsinghua.main/data/bin/
cp -ra ../lib              ./ifw/packages/com.tsinghua.main/data/
cd ./ifw/packages/com.tsinghua.main/data/lib
rm -rf librcpapi.so* libsingleapplication.so* libsystemc* librpusim* libqscintilla* 
cd ../../../../../

# com.tsinghua.rpudesigner
cp -ra ../bin/compress       ./ifw/packages/com.tsinghua.rpudesigner/data/bin/
cp -ra ../bin/maker          ./ifw/packages/com.tsinghua.rpudesigner/data/bin/
cp -ra ../bin/rpudesigner    ./ifw/packages/com.tsinghua.rpudesigner/data/bin/
cp -ra ../bin/rpudesigner.sh ./ifw/packages/com.tsinghua.rpudesigner/data/bin/
cp -ra ../lib/librcpapi.so*  ./ifw/packages/com.tsinghua.rpudesigner/data/lib/
cp -ra ../lib/libsingleapplication.so* ./ifw/packages/com.tsinghua.rpudesigner/data/lib/
cp -ra ../lib/libsystemc*    ./ifw/packages/com.tsinghua.rpudesigner/data/lib/
cp -ra ../lib/librpusim*     ./ifw/packages/com.tsinghua.rpudesigner/data/lib/
cp -ra ../lib/libqscintilla* ./ifw/packages/com.tsinghua.rpudesigner/data/lib/
cp -ra ../doc ../etc ../image ../script ../plugins ../version.txt ./ifw/packages/com.tsinghua.rpudesigner/data/
iconv -f gbk -t utf8 ../version.txt -o ./ifw/packages/com.tsinghua.rpudesigner/data/version.txt

# com.tsinghua.rpusimulator
#mkdir ./ifw/packages/com.tsinghua.rpusimulator/data/bin
#cp -r ../bin/rcs ./ifw/packages/com.tsinghua.rpusimulator/data/bin/
#rm ./ifw/packages/com.tsinghua.rpusimulator/data/bin/*.dll ./ifw/packages/com.tsinghua.rpusimulator/data/bin/*.err ./ifw/packages/com.tsinghua.rpusimulator/data/bin/*.exe ./ifw/packages/com.tsinghua.rpusimulator/data/bin/*_debug ./ifw/packages/com.tsinghua.rpusimulator/data/bin/*.debug ./ifw/packages/com.tsinghua.rpusimulator/data/plugins/*.dll

# com.tsinghua.firmwareupdate
#mkdir ./ifw/packages/com.tsinghua.firmwareupdate/data/bin
#cp -r ../bin/firmwaretool ../bin/firmwaretool.sh ../bin/firmwaretool_withqt.sh ./ifw/packages/com.tsinghua.firmwareupdate/data/bin/
#rm ./ifw/packages/com.tsinghua.firmwareupdate/data/bin/*.dll ./ifw/packages/com.tsinghua.firmwareupdate/data/bin/*.err ./ifw/packages/com.tsinghua.firmwareupdate/data/bin/*.exe ./ifw/packages/com.tsinghua.firmwareupdate/data/bin/*_debug ./ifw/packages/com.tsinghua.firmwareupdate/data/bin/*.debug ./ifw/packages/com.tsinghua.firmwareupdate/data/plugins/*.dll

find ./ifw/packages/com.tsinghua.main/data/ -name '.svn' | xargs rm -rf
find ./ifw/packages/com.tsinghua.rpudesigner/data/ -name '.svn' | xargs rm -rf
#find ./ifw/packages/com.tsinghua.rpusimulator/data/ -name '.svn' | xargs rm -rf
#find ./ifw/packages/com.tsinghua.firmwareupdate/data/ -name '.svn' | xargs rm -rf

datetime=`date +%Y%m%d%H%M%S`
releaseDate=`date +%Y-%m-%d`

mkdir -p release
filename="./release/rpudesigner_linux_x86_64_v"$version"_"$datetime".run"

sed -i '4c <Version>'$version'</Version>' ifw/config/config.xml
sed -i '5c <Version>'$version'</Version>' ifw/packages/com.tsinghua.main/meta/package.xml
sed -i '6c <ReleaseDate>'$releaseDate'</ReleaseDate>' ifw/packages/com.tsinghua.main/meta/package.xml
sed -i '5c <Version>'$version'</Version>' ifw/packages/com.tsinghua.rpudesigner/meta/package.xml
sed -i '6c <ReleaseDate>'$releaseDate'</ReleaseDate>' ifw/packages/com.tsinghua.rpudesigner/meta/package.xml
#sed -i '5c <Version>'$version'</Version>' ifw/packages/com.tsinghua.rpusimulator/meta/package.xml
#sed -i '6c <ReleaseDate>'$releaseDate'</ReleaseDate>' ifw/packages/com.tsinghua.rpusimulator/meta/package.xml
#sed -i '5c <Version>'$version'</Version>' ifw/packages/com.tsinghua.firmwareupdate/meta/package.xml
#sed -i '6c <ReleaseDate>'$releaseDate'</ReleaseDate>' ifw/packages/com.tsinghua.firmwareupdate/meta/package.xml

/opt/Qt/QtIFW-3.2.2/bin/binarycreator -c ifw/config/config.xml -p ifw/packages $filename

echo "Package done."
