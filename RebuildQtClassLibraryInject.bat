@echo Initialize environment
@set OldPath=%cd%
::x86_amd64的路径要自己找
 
@cd /d C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build
@if not %errorlevel% == 0 goto :end
call vcvarsx86_amd64.bat
@if not %errorlevel% == 0 goto :end
cd /d %OldPath%
@if not %errorlevel% == 0 goto :end


@echo Clean Project
devenv ./QtClassLibraryInject/QtClassLibraryInject.sln /Clean
@if not %errorlevel% == 0 goto :errBuild
 
@echo Start Build QtClassLibraryInject
devenv ./QtClassLibraryInject/QtClassLibraryInject.sln /ReBuild "Debug|x64"
@if not %errorlevel% == 0 goto :errBuild
 
devenv ./QtClassLibraryInject/QtClassLibraryInject.sln /ReBuild "Release|x64"
@if not %errorlevel% == 0 goto :errBuild
 
devenv ./QtClassLibraryInject/QtClassLibraryInject.sln /ReBuild "Debug|x86"
@if not %errorlevel% == 0 goto :errBuild
 
devenv ./QtClassLibraryInject/QtClassLibraryInject.sln /ReBuild "Release|x86"
@if not %errorlevel% == 0 goto :errBuild

@echo Build successed
 
goto :end
@pause
:errBuild
@echo Build error
@pause
:end