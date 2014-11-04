::关闭回显
@echo off
::设置各变量名
set	ProjectPath=%cd%
@echo on
@echo .
@echo －－－－－－－－－－－－－－为Java重命名函数名－－－－－－－－－－－－－－－
del "%ProjectPath%\IVS_SDK\Rename2Java.h"
call ThirdParty\Rename2Java.exe "%ProjectPath%\inc\IVS_SDK.h" "%ProjectPath%\IVS_SDK\Rename2Java.h"
@echo .
@echo off
