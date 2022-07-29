@echo off

REM Compile Here

SETLOCAL
set MODE=%1

set basePath=%cd%

set bin_int=%basePath%\bin-int\debug

IF NOT EXIST %bin_int% mkdir %bin_int%

set outputs=/Fp"%bin_int%\main.pch" /Fo%bin_int%\ /Fd"%bin_int%\main.pdb"

set common_flags=/nologo /diagnostics:caret /D _UNICODE /D UNICODE /std:c++17
rem /GF
set debug_flags=/W3 /WX /Od /MTd /Zi /FC /Gm-
rem /fsanitize=address
rem review the below
rem /EHs /RTC1 /MTd /GS /fp:precise /Zc:wchar_t  /Zc:forScope /Zc:inline /std:c++17 /fsanitize=address

rem important hidden specifier
rem /d1reportAllClassLayout

REM Once we get rid of all c/c++ headers, we will enable this to see the layout of the struct
rem /d1reportAllClassLayout
set release_flags=/Ox

set debug_link_option=/DEBUG:FULL /NOLOGO


set flags=%common_flags% %debug_flags% 
IF /I "%1"=="release" ( set flags=%common_flags% %release_flags% ) 
echo %flags%

rem set DirectXIncludePath=-I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\shared" -I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\um"

set IncludePath="%basePath%\include"

set IncludeDir=-I%IncludePath% -I"%basePath%\vendor\imgui"
set INCLUDE_PATHS=%DirectXIncludePath%  %IncludeDir%

set ImguiPATH=%basePath%\vendor\imgui\
set toBuild=%ImguiPATH%backends\imgui_impl_glfw.cpp %ImguiPATH%backends\imgui_impl_opengl3.cpp %ImguiPATH%imgui*.cpp

set WindowsLibDir=/LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\um\x64"
set LibDir=/LIBPATH:"%basePath%\lib"


set LIB_PATHS= %WindowsLibDir% %LibDir%
rem %toBuild% 
set source_files=main.cpp 
rem %IncludePath%\GLAD\glad.c
set object_files=main.obj imgui*.obj
rem glad.obj


pushd bin\
if exist main.exe (
   rm main.exe
)
popd


REM compile main
pushd src
rem echo cl /c  %INCLUDE_PATHS% %flags% %source_files% %outputs%
cl /c  %INCLUDE_PATHS% %flags% %source_files% %outputs%
popd

IF ERRORLEVEL 1 GOTO errorHandling

rem clang /c  %INCLUDE_PATHS% %flags% .\source\main.cpp %outputs%

REM LINK 
pushd %bin_int%
set LINKER_OPTIONS=/NODEFAULTLIB:MSVCRT /NODEFAULTLIB:LIBCMT /SUBSYSTEM:CONSOLE /INCREMENTAL:NO
set LINKER_OUTPUT=/OUT:"%basePath%/bin/main.exe"
link %object_files% %LINKER_OPTIONS% %LINKER_OUTPUT% %LIB_PATHS% %debug_link_option%
popd

IF ERRORLEVEL 1 GOTO errorHandling

:FINISH
ENDLOCAL
echo The build Succeeded
exit /b 0

:errorHandling
echo The build Faild
exit /b 1
