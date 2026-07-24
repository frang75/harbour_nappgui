::
:: OfficeSDK build script
::
:: Maintainer-only: (re)generates the vendored officesdk.dll/.lib
:: (LibreOffice-SDK C-Wrapper) into bin\win. Visual Studio required.
:: End users consuming hboffice don't need to run this script.
::
:: build -b [Debug|Release]

@echo off

::
:: Input parameters
::
set BUILD=Release
set "CWD=%cd%"

:parse
IF "%~1"=="" GOTO endparse
IF "%~1"=="-b" GOTO build
SHIFT
GOTO parse

:build
SHIFT
set BUILD=%~1
SHIFT
GOTO parse

:endparse

::
:: Beginning
::
echo ---------------------------
echo Generating OfficeSDK
echo Main path: %CWD%
echo Build type: %BUILD%
echo ---------------------------

set CMAKE_ARGS=-Ax64 -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl -DCMAKE_WARN_VS11=OFF
set CMAKE_BUILD=--config %BUILD%
call cmake %CMAKE_ARGS% -S %CWD% -B %CWD%\build || goto error_cmake
call cmake --build %CWD%\build %CMAKE_BUILD% || goto error_build

echo ---------------------------
echo OFFICESDK DLL build succeed
echo ---------------------------
goto end

::
:: Errors
::
:error_cmake
echo Error in OfficeSDK CMake generate
goto end

:error_build
echo Error building OfficeSDK
goto end

:end
