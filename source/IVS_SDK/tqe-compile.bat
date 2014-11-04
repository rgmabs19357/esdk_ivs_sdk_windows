::关闭回显
@echo off

set	ProjectPath=%cd%

@echo －－－－－－－－－－－－－－开始拷贝MP库－－－－－－－－－－－－－－－
::call ThirdParty\getcomponent.bat 0	
@echo .
@echo －－－－－－－－－－－－－－为Java重命名函数名－－－－－－－－－－－－－－－
del "%ProjectPath%\IVS_SDK\Rename2Java.h"
call ThirdParty\Rename2Java.exe "%ProjectPath%\inc\IVS_SDK.h" "%ProjectPath%\IVS_SDK\Rename2Java.h"
@echo .
@echo －－－－－－－－－－－－－－开始编译Release解决方案－－－－－－－－－－－－－－－
@echo .

"%VS100COMNTOOLS%\..\IDE\devenv.exe" "%ProjectPath%\IVS_SDK.sln" /rebuild "Release|Win32" /out tqeoutput.txt
