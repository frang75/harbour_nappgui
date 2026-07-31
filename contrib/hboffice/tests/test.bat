::
:: HBOffice Harbour examples build script
::
:: Compiles all the example .prg tests and sets PATH to the vendored
:: officesdk.dll required by the resulting executables.
::
:: test -comp [mingw64|clang|msvc64]
::

@echo off

::
:: Input parameters
::
set COMPILER=mingw64
set "CWD=%cd%"

:parse
IF "%~1"=="" GOTO endparse
IF "%~1"=="-comp" GOTO compiler
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
echo Compiling HBOffice Harbour examples
echo COMPILER: %COMPILER%
echo ---------------------------

set HBMK_PATH=..\..\..\bin\win\%COMPILER%

:: officesdk.dll is vendored and shared by every compiler (flat C API, no
:: C++ ABI split needed, unlike AWS-SDK in hbaws).
set "PATH=%CWD%\..\officesdk\bin\win;%PATH%"

for %%P in (sheet1 sheet2 doc1 doc2) do (
    echo Compiling %%P.prg...
    call %HBMK_PATH%\hbmk2.exe %%P.prg hboffice.hbc -comp=%COMPILER% || goto error_hboffice
)

echo ---------------------------
echo HBOFFICE examples build succeed
echo ---------------------------
goto end

::
:: Errors
::
:error_hboffice
echo Error building HBOFFICE examples
goto end

:end
