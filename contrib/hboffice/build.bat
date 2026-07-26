::
:: HBOffice build script
::
:: Will generate the hboffice.lib with the Harbour wrapper.
:: Visual Studio (msvc64) or MinGW (mingw64) or Clang allowed
:: Only for Visual Studio
:: "%ProgramFiles%\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
:: build -b [Debug|Release] -comp [msvc64|mingw64|clang]

@echo off

::
:: Input parameters
::
set COMPILER=mingw64
set BUILD=Release
set "CWD=%cd%"

:parse
IF "%~1"=="" GOTO endparse
IF "%~1"=="-comp" GOTO compiler
IF "%~1"=="-b" GOTO build
SHIFT
GOTO parse

:build
SHIFT
set BUILD=%~1
SHIFT
GOTO parse

:compiler
SHIFT
set COMPILER=%~1
SHIFT
GOTO parse

:endparse

::
:: Beginning
::
echo ---------------------------
echo Generating HBOFFICE
echo Main path: %CWD%
echo Build type: %BUILD%
echo COMPILER: %COMPILER%
echo ---------------------------

set HBMK_PATH=..\\..\\bin\\win\\%COMPILER%
set HBMK_FLAGS=

IF "%BUILD%"=="Debug" GOTO hbmk2_debug
GOTO hbmk2

:hbmk2_debug
set HBMK_FLAGS=-debug

:hbmk2
echo HBMK HOME: %HBMK_PATH%
call %HBMK_PATH%\\hbmk2.exe %HBMK_FLAGS% -comp=%COMPILER% %CWD%\hboffice.hbp || goto error_hboffice

echo ---------------------------
echo HBOFFICE LIB build succeed
echo ---------------------------
goto end

::
:: Errors
::
:error_hboffice
echo Error building HBOFFICE
goto end

:end
