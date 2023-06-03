@echo Initialize environment
@set OldPath=%cd%
::x86_amd64的路径要自己找
 
@cd /d F:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\x86_amd64
@if not %errorlevel% == 0 goto :end
call vcvarsx86_amd64.bat
@if not %errorlevel% == 0 goto :end
cd /d %OldPath%
@if not %errorlevel% == 0 goto :end


@echo Clean Project
devenv ./MFCInject/MFCInject.sln /Clean
@if not %errorlevel% == 0 goto :errBuild
 
@echo Start Build MFCInject
devenv ./MFCInject/MFCInject.sln /ReBuild "Debug|x64"
@if not %errorlevel% == 0 goto :errBuild
 
devenv ./MFCInject/MFCInject.sln /ReBuild "Release|x64"
@if not %errorlevel% == 0 goto :errBuild
 
devenv ./MFCInject/MFCInject.sln /ReBuild "Debug|x86"
@if not %errorlevel% == 0 goto :errBuild
 
devenv ./MFCInject/MFCInject.sln /ReBuild "Release|x86"
@if not %errorlevel% == 0 goto :errBuild

::如果需要拷贝文件就看下copy命令
::如果需要删除文件就看下del命令
@echo Build successed
 
goto :end
@pause
:errBuild
@echo Build error
@pause
:end