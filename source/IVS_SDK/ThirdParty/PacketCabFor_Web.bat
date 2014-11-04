::关闭回显
@echo off

::获得当前时间，作为生成版本的目录名
for /F "tokens=1-4 delims=-/ " %%i in ('date /t') do (
   set Year=%%i
   set Month=%%j
   set Day=%%k
   set DayOfWeek=%%l
)
for /F "tokens=1-2 delims=: " %%i in ('time /t') do (
   set Hour=%%i
   set Minute=%%j
)

::设置各变量名
set DateTime=%Year%-%Month%-%Day%-%Hour%-%Minute%
set	WinRarRoot=C:\Program Files\WinRAR
set	ProjectPath=%cd%

@echo on
echo %DateTime%
@echo .

@echo －－－－－－－－－－－－－－开始打包版本－－－－－－－－－－－－－－－－	
@echo .
mkdir tempversion\IVS_SDK
@echo .

"%WinRarRoot%\WinRAR.exe" x -y "IVS_SDK.rar"  tempversion\IVS_SDK
::拷贝支撑库到相应目录中
xcopy /E /y /i /r /s "tempversion\IVS_SDK\config"      			"tempversion\IVS_SDK\bin\config"
xcopy /E /y /i /r /s "tempversion\IVS_SDK\lib"	        		"tempversion\IVS_SDK\bin\lib"

xcopy /y /i /r /s "basicfiles\Alarm.wav"	                    "tempversion\IVS_SDK\bin\"
xcopy /y /i /r /s "basicfiles\IEShims.dll"	                    "tempversion\IVS_SDK\bin\"
xcopy /y /i /r /s "basicfiles\IVS_OCXPlayer.inf"	            "tempversion\IVS_SDK\bin\"
xcopy /y /i /r /s "basicfiles\License_Information.docx"	        "tempversion\IVS_SDK\bin\"
xcopy /y /i /r /s "basicfiles\mfc90.dll"	                    "tempversion\IVS_SDK\bin\"
xcopy /y /i /r /s "basicfiles\Microsoft.VC90.CRT.manifest"	    "tempversion\IVS_SDK\bin\"
xcopy /y /i /r /s "basicfiles\msvcp90.dll"	                    "tempversion\IVS_SDK\bin\"
xcopy /y /i /r /s "basicfiles\msvcp100.dll"	                    "tempversion\IVS_SDK\bin\"
xcopy /y /i /r /s "basicfiles\msvcr90.dll"	                    "tempversion\IVS_SDK\bin\"
xcopy /y /i /r /s "basicfiles\msvcr100.dll"	                    "tempversion\IVS_SDK\bin\"
xcopy /y /i /r /s "basicfiles\wer.dll"	                        "tempversion\IVS_SDK\bin\"

cd tempversion\IVS_SDK\bin
%ProjectPath%\cabarc -r -p N %ProjectPath%\IVS_OCX_%DateTime%.cab *

cd %ProjectPath%
rd /Q /S tempversion
@echo off
