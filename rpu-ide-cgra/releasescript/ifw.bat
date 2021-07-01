:: Program: Create rpudesigner installer.
:: Version: 1.0.0
:: Autor: ZhangJun
:: History: 2018\04\27 ZhangJun first release.

@echo off
if "%1" == "" (
	set /p version=please input the rpu designer version:
	if not defined version (
		echo Usage: %0 version
		pause
		exit 1
	)
)  else (
	set version=%1
)

::cd D:\rpu-ide-cicada\releasescript
syncrpudesigner.ffs_batch
::cd ..\release\RPUDesigner\bin
::C:\Qt\Qt5.10.0\5.10.0\mingw53_32\bin\windeployqt.exe rpudesigner.exe
::cd ..\..\..\releasescript

rd /s /q .\ifw\packages\com.tsinghua.main\data
rd /s /q .\ifw\packages\com.tsinghua.rpudesigner\data
rd /s /q .\ifw\packages\com.tsinghua.rpusimulator\data
rd /s /q .\ifw\packages\com.tsinghua.firmwareupdate\data


md .\ifw\packages\com.tsinghua.rpusimulator\data
md .\ifw\packages\com.tsinghua.firmwareupdate\data

:: com.tsinghua.main核心组件
md .\ifw\packages\com.tsinghua.main\data\bin
xcopy /C /E /Q  ..\release\RPUDesigner\bin .\ifw\packages\com.tsinghua.main\data\bin
del .\ifw\packages\com.tsinghua.main\data\bin\*.exe

:: com.tsinghua.rpudesigner
md .\ifw\packages\com.tsinghua.rpudesigner\data\bin
copy /Y ..\release\RPUDesigner\bin\*.exe .\ifw\packages\com.tsinghua.rpudesigner\data\bin
del .\ifw\packages\com.tsinghua.rpudesigner\data\bin\firmwaretool.exe
del .\ifw\packages\com.tsinghua.rpudesigner\data\bin\rcs.exe
md .\ifw\packages\com.tsinghua.rpudesigner\data\doc
xcopy /C /E /Q  ..\doc .\ifw\packages\com.tsinghua.rpudesigner\data\doc
md .\ifw\packages\com.tsinghua.rpudesigner\data\etc
xcopy /C /E /Q  ..\etc .\ifw\packages\com.tsinghua.rpudesigner\data\etc
md .\ifw\packages\com.tsinghua.rpudesigner\data\image
xcopy /C /E /Q  ..\image .\ifw\packages\com.tsinghua.rpudesigner\data\image
md .\ifw\packages\com.tsinghua.rpudesigner\data\script
xcopy /C /E /Q  ..\script .\ifw\packages\com.tsinghua.rpudesigner\data\script
md .\ifw\packages\com.tsinghua.rpudesigner\data\plugins
xcopy /C /E /Q  ..\plugins .\ifw\packages\com.tsinghua.rpudesigner\data\plugins
copy /Y  ..\version.txt .\ifw\packages\com.tsinghua.rpudesigner\data\

:: com.tsinghua.rpusimulator
md .\ifw\packages\com.tsinghua.rpusimulator\data\bin
copy /Y ..\release\RPUDesigner\bin\rcs.exe .\ifw\packages\com.tsinghua.rpusimulator\data\bin

:: com.tsinghua.firmwareupdate
md .\ifw\packages\com.tsinghua.firmwareupdate\data\bin
copy /Y ..\release\RPUDesigner\bin\firmwaretool.exe .\ifw\packages\com.tsinghua.firmwareupdate\data\bin

set datetime=%date:~0,4%%date:~5,2%%date:~8,2%%time:~0,2%%time:~3,2%%time:~6,2%
set releaseDate=%date:~0,4%-%date:~5,2%-%date:~8,2%

set filename=../release/rpudesigner_win_x86_v%version%_%datetime%.exe

call :ReplaceVersion ifw\config\config.xml 4
call :ReplaceVersion ifw\packages\com.tsinghua.main\meta\package.xml 5
call :ReplaceReleaseDate ifw\packages\com.tsinghua.main\meta\package.xml 6
call :ReplaceVersion ifw\packages\com.tsinghua.rpudesigner\meta\package.xml 5
call :ReplaceReleaseDate ifw\packages\com.tsinghua.rpudesigner\meta\package.xml 6
call :ReplaceVersion ifw\packages\com.tsinghua.rpusimulator\meta\package.xml 5
call :ReplaceReleaseDate ifw\packages\com.tsinghua.rpusimulator\meta\package.xml 6
call :ReplaceVersion ifw\packages\com.tsinghua.firmwareupdate\meta\package.xml 5
call :ReplaceReleaseDate ifw\packages\com.tsinghua.firmwareupdate\meta\package.xml 6

C:/Qt/QtIFW-3.1.1/bin/binarycreator.exe -c ifw/config/config.xml -p ifw/packages %filename%
pause
goto :eof
::exit 0

:ReplaceVersion
setlocal enabledelayedexpansion
set fn=%1
set n=%2
for /f "tokens=*" %%i in ('type "%fn%"')  do (
set /a m+=1
if !m!==%n% (
echo ^<Version^>%version%^</Version^>
) else (
echo %%i  )) >>temp.txt

move /y temp.txt "%fn%"
goto :eof

:ReplaceReleaseDate
setlocal enabledelayedexpansion
set fn=%1
set n=%2
for /f "tokens=*" %%i in ('type "%fn%"')  do (
set /a m+=1
if !m!==%n% (
echo ^<ReleaseDate^>%releaseDate%^</ReleaseDate^>
) else (
echo %%i  )) >>temp.txt

move /y temp.txt "%fn%"
goto :eof